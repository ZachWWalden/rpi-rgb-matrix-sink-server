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
 *Class - SocketStream
 *Author - Zach Walden
 *Created - 06/29/24
 *Last Changed - 06/29/24
 *Description - UDP Socket Wrapper that allows for communication between two processes.
====================================================================================*/

#include <cstdint>

namespace ZwNetwork{

enum SocketType {
	SERVER, CLIENT
};

struct StreamPacket {
	uint64_t num_bytes;
	const char* data;
};

class SocketStream
{
	//Attributes
public:
	static const int SERVER_PORT = 4090, CLIENT_PORT = 4080;
private:
	int sock_fd;
	//Methods
public:
	SocketStream();
	SocketStream(SocketType type);
	~SocketStream();

	bool send(const char* data, uint64_t num_bytes);
	StreamPacket* receive();


private:
	bool initServer();
	bool initClient();

	sockaddr* createAddrStruct(const int port);
};

}
