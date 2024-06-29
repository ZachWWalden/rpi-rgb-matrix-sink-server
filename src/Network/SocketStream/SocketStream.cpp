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
 *Description -
====================================================================================*/

#pragma once

#include "SocketStream.hpp"

namespace ZwNetwork{

SocketStream::SocketStream()
{

}
SocketStream::SocketStream(SocketType type)
{
	if(type == SocketType::SERVER)
		this->initServer();
	else
		this->initClient();
}
SocketStream::~SocketStream()
{

}

bool send(const char* data, uint64_t num_bytes)
{

}
StreamPacket* receive()
{

}
bool initServer()
{

}
bool initClient()
{

}
}















/*
<++> SocketStream::<++>()
{

}
*/
