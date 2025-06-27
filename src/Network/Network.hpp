/*==================================================================================
 *Class - Network
 *Author - Zach Walden
 *Created - 4/17/2024
 *Last Changed - 6/7/2025
 *Description - Stateful manager of frame sources using bare linux sockets
====================================================================================*/

/*
 * This program source code file is part of rpi-rgb-matrix-sink-server
 *
 * Copyright (C) 2024-2025 Zachary Walden zachary.walden@eagles.oc.edu
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

#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MAX_H_RES 256
#define MAX_V_RES 256
#define MAX_BYTES_PER_PIXEL 4

#define PROTOCOL_VERSION 0x00

namespace ZwNetwork{

//8 byte header
//Ensure there is no padding in structures bewteen the pac(push) and pack(pop).
#pragma pack(push,1)
struct SinkPacketHeader
{
	//Protocol Version
	uint8_t protocol_vers;
	//Priority
	uint8_t priority;
	//resolution
	uint8_t h_res;
	uint8_t v_res;
	//Bytes for pixels
	uint8_t bytes_per_pixel;
	//color mode
	uint8_t color_mode;
	//Location
	uint8_t h_loc;
	uint8_t v_loc;
	//Display intensity
	float intensity;
};

struct HandshakeHeader
{
	uint8_t req_protocol_vers;
	uint8_t success;
};
#pragma pack(pop)
//

struct SinkPacket
{
	SinkPacketHeader header;
	uint8_t *data;
};

class Network
{
	//Attributes
public:

private:
	int server_fd, client_fd;
	sockaddr_in addr;
	SinkPacketHeader header;
	//Methods
public:
	Network();
	Network(uint16_t port);
	~Network();

	bool waitForConnection();
	SinkPacket readPacket();
	bool writePacket(uint8_t num_bytes, uint8_t* data);

	int closeConnection();

private:
};

}
