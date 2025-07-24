/*
 * This program source code file is part of rpi-rgb-matrix-sink-server
 *
 * Copyright (C) 2024 Zachary Walden zachary.walden@eagles.oc.edu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/gpl-2.0.en.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/*==================================================================================
 *Class - Config
 *Author - Zach Walden
 *Created - 4/25/24
 *Last Changed - 5/29/24
 *Description - Reads and Unmarshalls a json configuration file.
====================================================================================*/
#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

#include "../Graphics/GraphicsStructs.hpp"

namespace ZwConfig
{

struct RgbAdjust
{
	int8_t red, green, blue;
	RgbAdjust()
	{

	}
	RgbAdjust(int8_t n_red, int8_t n_green, int8_t n_blue)
	{
		red = n_red;
		green = n_green;
		blue = n_blue;
	}
	void disp()
	{
		LOG_COLOR(red, green, blue);
	}
};

struct RotationConstants
{
	ZwGraphics::Vec2I offset, increment;
	RotationConstants()
	{

	}
	RotationConstants(ZwGraphics::Vec2I new_offset, ZwGraphics::Vec2I new_increment)
	{
		offset = new_offset;
		increment = new_increment;
	}
	void disp()
	{
		LOG("Offset");
		offset.disp();
		LOG("Increment");
		increment.disp();
	}
};

struct PanelMap
{
	ZwGraphics::Rectangle source, destination;
	RotationConstants rot_constants;
	RgbAdjust rgb_adj;
	PanelMap(ZwGraphics::Rectangle src, ZwGraphics::Rectangle dest, RotationConstants rotation_constants, RgbAdjust n_rgb_adj)
	{
		source = src;
		destination = dest;
		rot_constants = rotation_constants;
		rgb_adj = n_rgb_adj;
	}
	void disp()
	{
		LOG("Source");
		source.disp();
		LOG("Dest");
		destination.disp();
		LOG("Rot constants");
		rot_constants.disp();
		LOG("Rgb Adjust");
		rgb_adj.disp();
	}
};

class Config
{
	//Attributes
public:
	uint16_t hres, vres, panel_hres, panel_vres, port;
	uint16_t num_chains, chain_length, led_pwm_bits, led_slowdown_gpio, led_pwm_lsb_nanoseconds;
	uint16_t led_pwm_dither_bits, brightness, scan_mode, row_address_type, multiplexing;
	int16_t limit_refresh_rate;
	bool disable_hardware_pulsing, show_refresh_rate, inverse_colors, disable_busy_waiting;
	bool is_valid = false;

private:
	std::vector<PanelMap*> panels;
	std::string hardware_mapping, led_rgb_sequence, panel_type;
	//Methods
public:
	Config();
	~Config();

	std::vector<PanelMap*>* getPanelMaps();
	const char* getHardwareMapping();
	const char* getRgbSequence();
	const char* getPanelType();
	void disp();

private:
	bool readConfigFile();
	std::string getConfigHome();
	RotationConstants getRotConstants(int rot);

};

}

#endif
