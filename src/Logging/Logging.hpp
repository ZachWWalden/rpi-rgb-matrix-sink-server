/*
 * This program source code file is part of rpi-rgb-matrix-sink-server
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

#include <iostream>

#define DEBUG 1

#if DEBUG
#define LOG(x) std::cout << x << std::endl
#define LOG_INT(x) std::cout << std::endl << std::hex << (int)x << std::endl
#define LOG_POINT(x, y) std::cout << std::hex << "X = " << (int)x << ", Y = " << (int)y << std::endl
#define LOGV(x,y) std::cout << x << y << std::endl
#define LOG_IO(x,y,z) std::cout << "Error with File:" << x << x << z << std::endl
#else
#define LOG(X)
#define LOG_INT(x)
#define LOG_POINT(x, y)
#define LOGV(x,y)
#define LOG_IO(x, y, z)
#endif
