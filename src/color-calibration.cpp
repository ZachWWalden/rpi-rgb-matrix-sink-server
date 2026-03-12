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
#include "Graphics/GraphicsStructs.hpp"
#include "Utils/triplepointer.hpp"
#include "Config/Config.hpp"
#include "Network/Network.hpp"

using rgb_matrix::RGBMatrix;
using rgb_matrix::RuntimeOptions;
using rgb_matrix::Canvas;


volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
}

struct ColorRange
{
	ZwGraphics::Color start, end;
	int r_inc, g_inc, b_inc;
	ColorRange(ZwGraphics::Color n_start, ZwGraphics::Color n_end)
	{
		start = n_start;
		end = n_end;
		r_inc = n_end.red - n_start.red;
		g_inc = n_end.green - n_start.green;
		b_inc = n_end.blue - n_start.blue;
	}
};

bool isColorEqual(ZwGraphics::Color col_one, ZwGraphics::Color col_two);

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
	defaults.pwm_dither_bits = config->led_pwm_dither_bits;
	defaults.brightness = config->brightness;
	defaults.scan_mode = config->scan_mode;
	defaults.row_address_type = config->row_address_type;
	defaults.multiplexing = config->multiplexing;
	defaults.disable_hardware_pulsing = config->disable_hardware_pulsing;
	defaults.show_refresh_rate = config->show_refresh_rate;
	defaults.inverse_colors = config->inverse_colors;
	defaults.led_rgb_sequence = config->getRgbSequence();
	defaults.panel_type = config->getPanelType();
	defaults.limit_refresh_rate_hz = config->limit_refresh_rate;
	defaults.disable_busy_waiting = config->disable_busy_waiting;
	Canvas *canvas = RGBMatrix::CreateFromOptions(defaults, rt_defaults);
	if (canvas == NULL)
		return 1;

	ZwGraphics::Graphics *graphics_mgr = new ZwGraphics::Graphics(canvas, config->vres, config->hres);

	ColorRange cols[4] = {ColorRange(graphics_mgr->BLACK, graphics_mgr->WHITE),
						  ColorRange(graphics_mgr->BLACK, graphics_mgr->RED),
						  ColorRange(graphics_mgr->BLACK, graphics_mgr->GREEN),
						  ColorRange(graphics_mgr->BLACK, graphics_mgr->BLUE)
						};

	// It is always good to set up a signal handler to cleanly exit when we
	// receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
	// for that.
	signal(SIGTERM, InterruptHandler);
	signal(SIGINT, InterruptHandler);

	LOG("Start Loop\n");

	while(!interrupt_received)
	{
		//loop through calibration colors, use range variables.
		for(int i = 0; i < 4; i++)
		{
			ZwGraphics::Color cur_col = cols[i].start;
			do
			{
				LOG_COLOR(cur_col.red, cur_col.green, cur_col.blue);
				//write to matrix
				for(int x = 0; x < config->hres; x++)
				{
					for(int y = 0; y < config->vres; y++)
					{
						canvas->SetPixel(x, y, graphics_mgr->five_bit_to_eight_bit[cur_col.red >> 3],
											   graphics_mgr->five_bit_to_eight_bit[cur_col.green >> 3],
											   graphics_mgr->five_bit_to_eight_bit[cur_col.blue >> 3]);
					}
				}

				//sleep
				usleep(1000 * 10);

				//inc color
				cur_col.red += cols[i].r_inc;
				cur_col.green += cols[i].g_inc;
				cur_col.blue += cols[i].b_inc;
			}while(!isColorEqual(cur_col, cols[i].end));
		}

	}

	// Animation finished. Shut down the RGB matrix.
	canvas->Clear();
	delete canvas;
	return 0;
}

bool isColorEqual(ZwGraphics::Color col_one, ZwGraphics::Color col_two)
{
	return col_one.red == col_two.red && col_one.green == col_two.green && col_one.blue == col_two.blue;
}
