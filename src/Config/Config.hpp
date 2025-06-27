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
#include "../Logging/Logging.hpp"

namespace ZwConfig
{

struct RotationConstants
{
	ZwGraphics::Vec2I offset, increment;
	bool row_major;
	RotationConstants()
	{

	}
	RotationConstants(ZwGraphics::Vec2I new_offset, ZwGraphics::Vec2I new_increment, bool new_row_major)
	{
		offset = new_offset;
		increment = new_increment;
		row_major = new_row_major;
	}
};

struct PanelMap
{
	ZwGraphics::Rectangle source, destination;
	RotationConstants rot_constants;
	PanelMap(ZwGraphics::Rectangle src, ZwGraphics::Rectangle dest, RotationConstants rotation_constants)
	{
		source = src;
		destination = dest;
		rot_constants = rotation_constants;
	}
};

class Config
{
	//Attributes
public:
	uint16_t hres, vres, panel_hres, panel_vres, port;
	uint8_t num_chains;
	uint8_t chain_length;
	bool is_valid = false;

private:
	std::vector<PanelMap*> panels;
	std::string hardware_mapping;
	//Methods
public:
	Config();
	~Config();

	std::vector<PanelMap*>* getPanelMaps();
	const char* getHardwareMapping();

private:
	bool readConfigFile();
	std::string getConfigHome();
	RotationConstants getRotConstants(int rot);

};

}

#endif
