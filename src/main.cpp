// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "Graphics/Graphics.hpp"
#include "Utils/triplepointer.hpp"
#include "Config/Config.hpp"
#include "Network/Network.hpp"

using rgb_matrix::RGBMatrix;
using rgb_matrix::RuntimeOptions;
using rgb_matrix::Canvas;

#define MSG_QUEUE_NAME "/rpimatrixsinkserver"

void* networkThread(void* arg);
pthread_mutex_t lock;

volatile bool connection_valid = false;
volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
	ZwConfig::Config* config = new ZwConfig::Config();

	if(!config->is_valid)
	{
		LOG("Invalid Config");
		return EXIT_FAILURE;
	}

	RGBMatrix::Options defaults;
	RuntimeOptions rt_defaults;
	defaults.hardware_mapping = config->getHardwareMapping();  // or e.g. "adafruit-hat"
	defaults.rows = config->panel_vres;
	defaults.cols = config->panel_hres;
	defaults.chain_length = config->chain_length;
	defaults.parallel = config->num_chains;
	defaults.pwm_bits = config->led_pwm_bits;
	defaults.pwm_lsb_nanoseconds = config->led_pwm_lsb_nanoseconds;
	rt_defaults.gpio_slowdown = config->led_slowdown_gpio;
	//This sets the default brightness.
	defaults.brightness = 100;
	defaults.scan_mode = 0;
	defaults.show_refresh_rate = true;
	Canvas *canvas = RGBMatrix::CreateFromOptions(defaults, rt_defaults);
	if (canvas == NULL)
		return 1;

	//create message queue and and open read only
	mqd_t mq_create;
	int mq_ret;
	mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(ZwNetwork::SinkPacket);
	attr.mq_curmsgs = 0;

	mq_create = mq_open(MSG_QUEUE_NAME, O_CREAT|O_RDONLY, 0744, &attr);
	if (mq_create != 3)
	{
		LOG("Creation of receiver queue failed");
		return EXIT_FAILURE;
	}
	// It is always good to set up a signal handler to cleanly exit when we
	// receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
	// for that.
	signal(SIGTERM, InterruptHandler);
	signal(SIGINT, InterruptHandler);

	if(pthread_mutex_init(&lock, NULL) != 0)
	{
		LOG("Failed to initialize mutex");
		return EXIT_FAILURE;
	}

	ZwGraphics::Graphics *graphics_mgr = new ZwGraphics::Graphics(canvas, config->vres, config->hres);
	ZwGraphics::Font font916 = graphics_mgr->fontFactory(ZwGraphics::Font9x16);
	ZwGraphics::Font font79 = graphics_mgr->fontFactory(ZwGraphics::Font7x9);

	LOG_INT(config->port);
	//start network thread and wait for data.
	long unsigned int tid;
	pthread_create(&tid, NULL, networkThread, &(config->port));

	char buf[attr.mq_msgsize];

	while(!interrupt_received)
	{
			if(connection_valid)
			{
				// LOG("Render: Valid Connection");
				//Wait for a frame
				//recv msg
				mq_ret = mq_receive(mq_create,(char*)buf, attr.mq_msgsize, NULL);
				if(mq_ret == -1)
				{
					LOG("Render: Message receive failed");
					exit(EXIT_FAILURE);
				}
				// LOG("Render: MSG Received");
				ZwNetwork::SinkPacket *msg = (ZwNetwork::SinkPacket*) buf;
				// LOG("Render: Calling frame render");
				graphics_mgr->drawWithMapsFlat555(config->getPanelMaps(), *msg);
				// LOG("Render: frame rendered");
				// graphics_mgr->setRenderTarget(graphics_mgr->convertFlatBufferToTriplePointer(*msg));
				// LOG("Buffer converted");
				//When a frame is received, map each of it's regions to a panel in the chain. And draw to canvas
				// graphics_mgr->drawWithMaps(config->getPanelMaps());
			}
	}

	//close the msg queue
	mq_ret = mq_close(mq_create);
	if(mq_ret == -1)
	{
		LOG("Render thread mq failed to close.");
	}
	//unlink the msg queue
	int unlink_status = mq_unlink(MSG_QUEUE_NAME);
	if(unlink_status != 0)
	{
		//unlink failed
		LOG("Mqueue unlink failed");
		if(errno == EACCES)
			LOG("EACCES");
		else if (errno == ENAMETOOLONG)
			LOG("ENAMETOOLONG");
		else if (errno == ENOENT)
			LOG("ENOENT");
	}

	// Animation finished. Shut down the RGB matrix.
	canvas->Clear();
	delete canvas;
	return 0;
}

void* networkThread(void* arg)
{
	//Initialze, port numnber was passed to this thread through arg. cast arg into uint16_t pointer then dereference
	ZwNetwork::Network* interface = new ZwNetwork::Network(*((uint16_t*)arg));
	//open msg queue write only
	mqd_t mq_wronly;
	mq_wronly = mq_open(MSG_QUEUE_NAME, O_WRONLY);
	int mq_ret = -1;
	int conn_rcvd = 0;
	while(!interrupt_received)
	{
		//wait for a connection
		interface->waitForConnection();
		// LOG("Network: connection found");
		conn_rcvd++;
		LOG_INT(conn_rcvd);
		connection_valid = true;
		bool msg_sent = false;
		while(!interrupt_received && connection_valid)
		{
			//Handle a single connection.
			//wait for frame
			//call to interface->read() transfers ownership of all dynmically allocated memory accessible using pointers within the ZwNetwork::SinkPacket it returns.
			// LOG("Network: Wait for packet");
			ZwNetwork::SinkPacket packet = interface->readPacket();
			// LOG("Network: Packet received");
			//Check if termination packet has been sent.
			if(packet.header.color_mode == 0xFF || packet.data == nullptr)
			{
				LOG("Connection terminated");
				connection_valid = false;
			}
			//send message.
			if(connection_valid)
			{
				// LOG("Network: Con valid/ msg send");
				mq_ret = mq_send(mq_wronly,(const char *)&packet,sizeof(ZwNetwork::SinkPacket),0);
				if(mq_ret != 0)
				{
					LOG("Network: Message did not send");
					if(errno == EAGAIN)
						LOG("EAGAIN");
					else if(errno == EBADF)
						LOG("EBADF");
					else if(errno == EINTR)
						LOG("EINTR");
					else if(errno == EINVAL)
						LOG("EINVAL");
					else if(errno == EMSGSIZE)
						LOG("EMSGSIZE");
					else if(errno == ETIMEDOUT)
						LOG("ETIMEDOUT");
					else
						LOG_INT(errno);
					msg_sent = false;
				}
				else
				{
					// LOG("Network: Msg sent");
					msg_sent = true;
				}
			}
			if(!msg_sent && packet.data != nullptr)
			{
				//free any allocated memory that did not recv an ownership transfer to render thread.
				// LOG("Network: delete unused frame data");
				delete packet.data;
			}
			//reset this flag for next connection.
			msg_sent = false;
		}
		// LOG("Network: Close client connection");
		interface->closeClientConnection();
		//Send a blank frame to across
	}
	interface->closeServerConnection();
	//Close the msq queue.
	mq_ret = mq_close(mq_wronly);
	if(mq_ret != 0)
	{
		LOG("Sending queue failed to close, exiting.");
	}
	pthread_exit(0);
}
