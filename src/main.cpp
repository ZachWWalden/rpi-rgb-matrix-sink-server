// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)
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

#include "Graphics/Graphics.hpp"
#include "Utils/triplepointer.hpp"
#include "Config/Config.hpp"
#include "Network/Network.hpp"
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

void* networkThread(void* arg);
pthread_mutex_t lock;

volatile bool connection_valid = false;
volatile bool interrupt_received = false;

volatile int port;


static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
	ZwConfig::Config* config = new ZwConfig::Config();
	port = config->port;

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

	//start network thread and wait for data.
	long unsigned int tid;
	pthread_create(&tid, NULL, networkThread, &tid);

	while(!interrupt_received)
	{
			if(connection_valid)
			{
				//Wait for a frame
				//When a frame is received, map each of it's regions to a panel in the chain.
				//Canvas V_RES = DISP_V_RES, Canvas H_RES = DISP_H_RES	* CHAIN_LENGTH - 1
				//write appropriate data to the canvas
			}
	}


	// Animation finished. Shut down the RGB matrix.
	canvas->Clear();
	delete canvas;
	return 0;
}

void* networkThread(void* arg)
{
	//Initialze
	ZwNetwork::Network interface(port, );
	while(!interrupt_received)
	{
		//wait for a connection
		connection_valid = true;
		while(!interrupt_received && connection_valid)
		{
			//Handle a single connection.
			//wait for frame
		}
	}
	pthread_exit(0);
}
