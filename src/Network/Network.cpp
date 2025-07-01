/*==================================================================================
 *Class - Network
 *Author - Zach Walden
 *Created - 4/17/2024
 *Last Changed - 6/7/2024
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

#include "Network.hpp"

#include "../Logging/Logging.hpp"
#include <cerrno>
#include <cstdlib>

namespace ZwNetwork
{

Network::Network()
{

}

Network::Network(uint16_t port)
{
	//create socket
	//ipv4, tcp, ip protocol
	if((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		LOG("Server socket creation failed");
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if(setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		LOG("Socket options failed to set");
		if(errno == EBADF)
			LOG("EBADF");
		else if(errno == EFAULT)
			LOG("EFAULT");
		else if(errno == EINVAL)
			LOG("EINVAL");
		else if(errno == ENOPROTOOPT)
			LOG("ENOPROTOOPT");
		else if(errno == ENOTSOCK)
			LOG("ENOTSOCK");
		//exit(EXIT_FAILURE);
	}

	//Set options for the port address.
	this->addr.sin_family = AF_INET;				//Internet Protocol
	this->addr.sin_addr.s_addr = INADDR_ANY;		//Any IP Addr can conenct
	this->addr.sin_port = htons(port);				//Bind to passed port
	if(bind(this->server_fd, (struct sockaddr *)(&(this->addr)), sizeof(this->addr)))
	{
		LOG("Socket failed to bind");
		exit(EXIT_FAILURE);
	}
	//Listen on socket for connections with a maximum of 5 connections in the backlog.
	if(listen(this->server_fd, 5))
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
	bool ret_val = false;
	socklen_t addr_len = sizeof(this->addr);
	if((this->client_fd = accept(this->server_fd, (struct sockaddr *)(&(this->addr)), &addr_len)) < 0)
	{
		LOG("Failed to connect to client");
		exit(EXIT_FAILURE);
	}
	//verify connection
	HandshakeHeader hndshk_hdr;
	//read handsake packet.
	int valsend;
	int valread = this->recv_all(this->client_fd, &hndshk_hdr, sizeof(HandshakeHeader));
	if(valread == -1)
	{
		LOG("Read first handshake from client failed");
		if(errno == EAGAIN)
			LOG("EAGAIN");
		else if(errno == EWOULDBLOCK)
			LOG("EWOULDBLOCK");
		else if(errno == EBADF)
			LOG("EBADF");
		else if(errno == EFAULT)
			LOG("EFAULT");
		else if(errno == EINTR)
			LOG("EINTR");
		else if(errno == EINVAL)
			LOG("EINVAL");
		else if(errno == EIO)
			LOG("EIO");
		else if(errno == EISDIR)
			LOG("EISDIR");
		else
			LOG_INT(errno);
		exit(EXIT_FAILURE);
	}

	//if handshake packet indicates a compatible protocol version, send handshake packet with success = 1
	if(hndshk_hdr.req_protocol_vers == PROTOCOL_VERSION)
	{
		hndshk_hdr.success = 1;
		valsend = this->send_all(this->client_fd, &hndshk_hdr, sizeof(HandshakeHeader), 0);
		if(valsend == -1)
		{
			LOG("Handshake packet success send failed");
			exit(EXIT_FAILURE);
		}
		ret_val = true;
	}
	//if not compatible, send client a handshake packet with a compatible protocol version and success = 0
	//read handshake packet. If success = 1, set protocol version to the received number.
	if(!ret_val)
	{
		hndshk_hdr.success = 0;
		hndshk_hdr.req_protocol_vers = 1;
		valsend = this->send_all(this->client_fd, &hndshk_hdr, sizeof(HandshakeHeader), 0);
		if(valsend == -1)
		{
			LOG("Handshake packet retry send failed");
			exit(EXIT_FAILURE);
		}

		valread = this->recv_all(this->client_fd, &hndshk_hdr, sizeof(HandshakeHeader));
		if(valread == -1)
		{
			LOG("Read second handshake from client failed");
			if(errno == EAGAIN)
				LOG("EAGAIN");
			else if(errno == EWOULDBLOCK)
				LOG("EWOULDBLOCK");
			else if(errno == EBADF)
				LOG("EBADF");
			else if(errno == EFAULT)
				LOG("EFAULT");
			else if(errno == EINTR)
				LOG("EINTR");
			else if(errno == EINVAL)
				LOG("EINVAL");
			else if(errno == EIO)
				LOG("EIO");
			else if(errno == EISDIR)
				LOG("EISDIR");
			else
				LOG_INT(errno);
			exit(EXIT_FAILURE);
		}
		if(hndshk_hdr.req_protocol_vers == PROTOCOL_VERSION)
		{
			ret_val = true;
		}
	}
	//else, assume that the handshake has failed.
	if(!ret_val)
		close(client_fd);
	//if verified return true.
	return ret_val;
}

SinkPacket Network::readPacket()
{
	bool valid_data = true;
	//read header
	SinkPacketHeader pckt;
	int valread = this->recv_all(this->client_fd, &pckt, sizeof(SinkPacketHeader));
	if(valread == -1)
	{
		LOG("Read frame packet from client failed");
		exit(EXIT_FAILURE);
	}
	LOG("");
	LOG("");
	LOG_POINT(pckt.h_res, pckt.v_res);
	LOG_INT(pckt.bytes_per_pixel);
	LOG("");
	LOG("");
	//ensure the data payload is reasonably sized.
	int h_res_full = pckt.h_res + 1, v_res_full = pckt.v_res + 1;
	if((pckt.bytes_per_pixel * ((h_res_full) * (v_res_full))) > (MAX_BYTES_PER_PIXEL * MAX_H_RES * MAX_V_RES))
	{
		//client is trying to send over 1 MiB of data we define anything more as a malicious.
		LOG("Packet too big");
		LOG("");
		LOG("");
		LOG_POINT(pckt.h_res, pckt.v_res);
		LOG_INT(pckt.bytes_per_pixel);
		LOG("");
		LOG("");
		valid_data = false;
	}
	SinkPacket packet;
	packet.header = pckt;
	packet.data = nullptr;
	if(valid_data)
	{
		//allocate on the heap for payload.
		int num_bytes = (int)pckt.bytes_per_pixel * (v_res_full * h_res_full);
		LOG_INT(num_bytes);
		LOG("Start malloc in network obj");
		uint8_t *data = new uint8_t(num_bytes);
		LOG("End malloc in network obj");
		valread = this->recv_all(client_fd, data, num_bytes);
		if(valread == -1)
		{
			LOG("Read frame data from client failed");
			exit(EXIT_FAILURE);
		}
		packet.data = data;
	}
	//return packet.
	return packet;
}

bool Network::writePacket(uint8_t num_bytes, uint8_t* data)
{
	send(this->client_fd, data, num_bytes, 0);
	return true;
}


int Network::closeConnection()
{
	int ret_val  = close(this->client_fd);

	return ret_val;
}

int Network::send_all(int sock_fd, const void* buf, size_t size, int flags)
{
	long bytes_sent = 0;
	do
	{
		bytes_sent += send(sock_fd, ((uint8_t* )buf) + bytes_sent, size - bytes_sent, flags);
		if(bytes_sent == -1)
			break;
	}while(((unsigned long)bytes_sent) < size);

	return bytes_sent;
}

int Network::recv_all(int sockfd, void *buf, size_t size)
{
	long bytes_recvd = 0;
	do
	{
		bytes_recvd += read(sockfd, ((uint8_t*)buf) + bytes_recvd, size - bytes_recvd);
		if(bytes_recvd == -1)
			break;
	}while(((unsigned long)bytes_recvd) < size);

	return bytes_recvd;
}

}



/*
<++> Network::<++>()
{

}
*/
