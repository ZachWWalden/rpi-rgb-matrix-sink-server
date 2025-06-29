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
		return EXIT_FAILURE;

	RGBMatrix::Options defaults;
	defaults.hardware_mapping = config->getHardwareMapping();  // or e.g. "adafruit-hat"
	defaults.rows = config->panel_vres;
	defaults.cols = config->panel_hres;
	defaults.chain_length = config->chain_length;
	defaults.parallel = config->num_chains;
	//This sets the default brightness.
	defaults.brightness = 50;
	defaults.scan_mode = 0;
	defaults.show_refresh_rate = true;
	Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
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

	while(!interrupt_received)
	{
			if(connection_valid)
			{
				LOG("Valid Connection");
				//Wait for a frame
				//recv msg
				ZwNetwork::SinkPacket msg;
				mq_ret = mq_receive(mq_create, (char*)(&msg), attr.mq_msgsize, NULL);
				graphics_mgr->setRenderTarget(graphics_mgr->convertFlatBufferToTriplePointer(msg));
				//When a frame is received, map each of it's regions to a panel in the chain. And draw to canvas
				graphics_mgr->drawWithMaps(config->getPanelMaps());
			}
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
	while(!interrupt_received)
	{
		//wait for a connection
		interface->waitForConnection();
		connection_valid = true;
		bool msg_sent = false;
		while(!interrupt_received && connection_valid)
		{
			//Handle a single connection.
			//wait for frame
			//call to interface->read() transfers ownership of all dynmically allocated memory accessible using pointers within the ZwNetwork::SinkPacket it returns.
			ZwNetwork::SinkPacket packet = interface->readPacket();
			//Check if termination packet has been sent.
			if(packet.header.color_mode == 0xFF || packet.data == nullptr)
			{
				LOG("Connection terminated");
				connection_valid = false;
			}
			//send message.
			if(connection_valid)
			{
				mq_ret = mq_send(mq_wronly,(const char *)&packet,sizeof(ZwNetwork::SinkPacket) + 1,0);
				if(mq_ret != 0)
				{
					LOG("Message did not send");
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
					msg_sent = true;
			}
			if(!msg_sent)
			{
				//free any allocated memory that did not recv an ownership transfer to render thread.
				delete packet.data;
			}
		}
		interface->closeConnection();
	}
	//Close the msq queue.
	mq_ret = mq_close(mq_wronly);
	if(mq_ret != 0)
	{
		LOG("Sending queue failed to close, exiting.");
	}
	pthread_exit(0);
}
