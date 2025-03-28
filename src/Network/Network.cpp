/*==================================================================================
 *Class - Network
 *Author - Zach Walden
 *Created - 4/17/2024
 *Last Changed - 4/17/2023
 *Description - OO libcurl wrapper
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

#include "Network.hpp"
#include <cstdint>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <cstring>
#include <string>
#include <sys/socket.h>

#include "../Logging/Logging.hpp"

namespace ZwNetwork
{

Network::Network()
{

}

Network::Network(uint16_t port)
{
	//create socket
	//ipv4, tcp, ip protocol
	int status = this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(status != SUCCESS)
	{
		LOG("Server socket creation failed");
		exit(EXIT_FAILURE);
	}
	status = setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
	if(status != SUCCESS)
	{
		LOG("Socket options failed to set");
		exit(EXIT_FAILURE);
	}
	status = bind(this->server_fd, const struct sockaddr *addr, socklen_t addrlen);
	if(status != SUCCESS)
	{
		LOG("Socket failed to bind");
		exit(EXIT_FAILURE);
	}
	//Listen on socket for connections with a maximum of 5 connections in the backlog.
	status = listen(this->server_fd, 5);
	if(status != SUCCESS)
	{
		LOG("Socket failed to begin listening");
		exit(EXIT_FAILURE);
	}
}

Network::~Network()
{
}

bool Network::waitForConnection()
{
	this->client_fd = accept(this->server_fd, struct sockaddr *__restrict addr, socklen_t *__restrict addr_len);
	//verify connection
	//if not verified, recycle conenction, ret false
	//if verified return true.
}

SinkPacket Network::readPacket()
{
	//read header
	//ensure the data payload is reasonably sized.
	//allocate on the heap for payload.
	//return packet.
}

bool Network::writePacket(uint8_t num_bytes, uint8_t* data)
{
	return true;
}


}



/*
<++> Network::<++>()
{

}
*/
