/*==================================================================================
 *Class - Network
 *Author - Zach Walden
 *Created - 4/17/2024
 *Last Changed - 4/17/2024
 *Description - Stateful manager of frame sources using bare linux sockets
====================================================================================*/

/*
 * This program source code file is part of PROJECT_NAME
 *
 * Copyright (C) 2024 Zachary Walden zachary.walden@eagles.oc.edu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
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
#pragma once

#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace ZwNetwork{

//8 byte header
//Ensure there is no padding in structures bewteen the pac(push) and pack(pop).
#pragma pack(push,1)
struct SinkPacketHeader
{
	//resolution
	uint8_t h_res;
	uint8_t v_res;
	//Bytes for pixels
	uint8_t bytes_per_pixel;
	//color mode
	uint8_t color_mode;
	//Display intensity
	float intensity;
};
#pragma pack(pop)
//

struct SinkPacket
{
	SinkPacketHeader header;
	uint8_t *data;
};

#pragma pack(push,1)
struct MsgHeader
{
	SinkPacket* frame_header;
};
#pragma pack(pop)


class Network
{
	//Attributes
public:

private:
	int server_fd;
	sockaddr_in addr;
	SinkPacketHeader header;
	//Methods
public:
	Network();
	Network(uint16_t port);
	~Network();

	bool listen();
	SinkPacket read();
	bool write(uint8_t num_bytes, uint8_t* data);

private:
};

}
