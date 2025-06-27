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
 *Last Changed - 5/28/24
 *Description - Config Parser
====================================================================================*/

#pragma once

#include "Config.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/config.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>

namespace ZwConfig
{

Config::Config()
{
	this->is_valid = this->readConfigFile();
}
Config::~Config()
{
	std::vector<PanelMap*>::iterator itr = this->panels.begin();
	for(; itr < this->panels.end(); itr++)
	{
		delete *itr;
	}
}

bool Config::readConfigFile()
{
	//TODO add in config sanitizing
	bool valid_config = true;
	//debug config path
	std::string config_path = "config/testconfig.json";
	//get config path
#ifndef DEBUG
	//use $XDG_CONFIG_HOME
	config_path = this->getConfigHome() + "/config";
#endif

	std::ifstream config;
	config.open(config_path);

	Json::Value root;
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errs;

	if(!Json::parseFromStream(builder, config, &root, &errs))
	{
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}

	//get port
	this->port = root["port"].asInt();
	//get hardware mapping: "regular", "adafruit-hat", "adafruit-hat-pwm", "compute-moudle"
	this->hardware_mapping = root["hardware_mapping"].asCString();
	//get src hres
	this->hres = root["hres"].asInt();
	//get src vres
	this->vres = root["vres"].asInt();
	//get panel hres
	this->panel_hres = root["panel_hres"].asInt();
	//get panel vres
	this->panel_vres = root["panel_vres"].asInt();
	//get num chains
	this->num_chains = root["num_chains"].asInt();
	//get chain length
	this->chain_length = root["chain_length"].asInt();

	Json::Value panel_maps = root["panel_maps"];
	//loop over panels
	for(int i = 0; i < panel_maps.size(); i++)
	{
		Json::Value cur_src = panel_maps[i]["source"];
		Json::Value cur_dst = panel_maps[i]["destination"];

		RotationConstants rot_constants = getRotConstants(panel_maps[i]["rot"].asInt());

		this->panels.push_back(new PanelMap(ZwGraphics::Rectangle(
												ZwGraphics::Point(
													cur_src["x1"].asInt(),
													cur_src["y1"].asInt()
												),
												ZwGraphics::Point(
													cur_src["x2"].asInt(),
													cur_src["y2"].asInt()
												)
											),
											ZwGraphics::Rectangle(
												ZwGraphics::Point(
													cur_dst["x1"].asInt(),
													cur_dst["y1"].asInt()
												),
												ZwGraphics::Point(
													cur_dst["x2"].asInt(),
													cur_dst["y2"].asInt()
												)
											),
											rot_constants
											));
	}

	return valid_config;
}

std::string Config::getConfigHome()
{
	std::string value = "";
	char *val = getenv("$XDG_CONFIG_HOME");
	if(val != NULL)
		value = val;
	return value;
}

RotationConstants Config::getRotConstants(int rot)
{
	int h_offset = 0, v_offset = 0, h_increment = 0, v_increment = 0;
	bool row_major = true;
	if(rot == 0)
	{
		h_offset = 0; v_offset = 0; h_increment = 1; v_increment = 1;
		row_major = true;
	}
	//(0,0) -> (h_res, 0)
	else if (rot == 90 || rot == -270) {
		h_offset = this->panel_hres; v_offset = 0; h_increment = -1; v_increment = 1;
		row_major = false;
	}
	//(0,0) -> (h_res, v_res)
	else if (rot == 180 || rot == -180) {
		h_offset = this->panel_hres; v_offset = this->panel_vres; h_increment = -1; v_increment = -1;
		row_major = true;
	}
	//(0,0) -> (0, v_res)
	else if (rot == 270 || rot == -90) {
		h_offset = 0; v_offset = this->panel_vres; h_increment = 1; v_increment = -1;
		row_major = false;
	}
	else
	{
		LOG("Invalid Rotation constant. Valid values are: 0, +-90, +-180, +-270");
		exit(EXIT_FAILURE);
	}

	return RotationConstants(ZwGraphics::Vec2I(h_offset, v_offset), ZwGraphics::Vec2I(h_increment, v_increment), row_major);
}

std::vector<PanelMap*>* Config::getPanelMaps()
{
	return &(this->panels);
}

const char* Config::getHardwareMapping()
{
	return this->hardware_mapping.c_str();
}

}













/*
<++> Config::<++>()
{

}
*/
