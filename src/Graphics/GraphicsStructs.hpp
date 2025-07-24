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
 *Header - GraphicsStructs
 *Author - Zach Walden
 *Created - 05/28/24
 *Last Changed - 05/29/24
 *Description -
====================================================================================*/

#ifndef GRAPHICSSTRUCTS_H
#define GRAPHICSSTRUCTS_H

#include <cstdint>

#include "../Logging/Logging.hpp"

namespace ZwGraphics
{

struct Color
{
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	Color()
	{

	}
	Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
	{
		Color();
		alpha = a;
		red = r;
		green = g;
		blue = b;
	}
	Color invert()
	{
		return Color(alpha, ~red, ~blue, ~green);
	}
};

struct Point
{
	uint8_t x;
	uint8_t y;
	Point()
	{

	}
	Point(uint8_t new_x, uint8_t new_y)
	{
		x = new_x;
		y = new_y;
	}
	void disp()
	{
		LOG_POINT(x, y);
	}
};

struct Vec2I
{
	int x;
	int y;
	Vec2I()
	{

	}
	Vec2I(int new_x, int new_y)
	{
		x = new_x;
		y = new_y;
	}
	void disp()
	{
		LOG_POINT(x, y);
	}
};

struct Rectangle
{
	Point p_top_left;
	Point p_bot_right;
	Rectangle()
	{

	}
	Rectangle(Point new_p1, Point new_p2)
	{
		p_top_left = new_p1;
		p_bot_right = new_p2;
	}
	bool is_contained(Point pnt)
	{
		if(pnt.x >= p_top_left.x && pnt.x <= p_bot_right.x && pnt.y >= p_top_left.y && pnt.y <= p_bot_right.y)
			return true;
		else
			return false;
	}
	void disp()
	{
		LOG("p_top_left");
		p_top_left.disp();
		LOG("p_bot_right");
		p_bot_right.disp();
	}
};

struct Triangle
{
	Point p1;
	Point p2;
	Point p3;
	Triangle()
	{

	}
	Triangle(Point new_p1, Point new_p2, Point new_p3)
	{
		p1 = new_p1;
		p2 = new_p2;
		p3 = new_p3;
	}
};

struct Circle
{
	Point center;
	uint8_t radius;
	Circle()
	{

	}
	Circle(uint8_t new_radius, Point new_center)
	{
		center = new_center;
		radius = new_radius;
	}
};

struct Gradient
{
	Color start;
	Color end;
	Gradient()
	{

	}
	Gradient(Color start_color, Color end_color)
	{
		start = start_color;
		end = end_color;
	}
};

enum FontStatus
{
	FontOutOfRange, FontSuccess, FontPrintCutoff
};

enum Fonts
{
	Font4x6, Font5x8, Font7x9, Font9x16
};

 struct Font
{
	uint8_t num_rows;
	uint8_t row_size;
	uint8_t width;
	uint8_t* font;
	Font()
	{

	}
	Font(uint8_t rows, uint8_t row_byte_num, uint8_t cols, uint8_t* fontAddr)
	{
		num_rows = rows;
		row_size = row_byte_num;
		width = cols;
		font = fontAddr;
	}
};
}

#endif
