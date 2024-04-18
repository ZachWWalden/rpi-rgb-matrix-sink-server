// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)
#include <cstddef>
#include <cstdlib>
#include <unistd.h>
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
#include "Network/Network.hpp"
#include "stdint.h"
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

#define CONFIG_PATH "stocks/stocks.json"
#define IMAGE_PATH "stocks/"

#define CHAIN_LENGTH 2
#define DISP_V_RES 64
#define DISP_H_RES 64

volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = DISP_V_RES;
  defaults.cols = DISP_H_RES;
  defaults.chain_length = CHAIN_LENGTH;
  defaults.parallel = 1;
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

  ZwGraphics::Graphics graphics_mgr(canvas, DISP_V_RES, DISP_H_RES);
  ZwGraphics::Font font916 = graphics_mgr.fontFactory(ZwGraphics::Font9x16);
  ZwGraphics::Font font79 = graphics_mgr.fontFactory(ZwGraphics::Font7x9);

  //open list of tickers and ticker parameters.
  std::ifstream ifs;
  ifs.open(CONFIG_PATH);

  while(!interrupt_received)
  {
	//Wait for a frame
	//When a frame is received, map each of it's regions to a panel in the chain.
	//Canvas V_RES = DISP_V_RES, Canvas H_RES = DISP_H_RES	* CHAIN_LENGTH - 1
	//write appropriate data to the canvas
  }


  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;
  return 0;
}

