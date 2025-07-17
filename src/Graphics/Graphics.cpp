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

#pragma once
#include "Graphics.hpp"
#include "stdlib.h"
#include <cmath>
#include <cstdint>
#include <sys/types.h>

#include "../Logging/Logging.hpp"
#include "../Utils/triplepointer.hpp"

namespace ZwGraphics
{
	const Color Graphics::BLACK(255,0,0,0);
	const Color Graphics::WHITE(255,255,255,255);
	const Color Graphics::RED(255,255,0,0);
	const Color Graphics::GREEN(255,0,255,0);
	const Color Graphics::BLUE(255,0,0,255);
	const Color Graphics::YELLOW(255,255,255,0);
	const Color Graphics::MAGENTA(255,255,0,255);
	const Color Graphics::CYAN(255,0,255,255);
	const Color Graphics::TRANSPARENT(0,0,0,0);


	// const uint8_t Graphics::five_bit_to_eight_bit[] = {0,   2,   8,   16,  24,  30,  38,  50,
	// 												   62,  72,  80,  88,  96,  104, 112, 120,
	// 												   128, 136, 144, 152, 160, 168, 176, 184,
	// 												   192, 208, 216, 224, 232, 240, 248, 255};
	const uint8_t Graphics::five_bit_to_eight_bit[] = {
     0,   0,   1,   2,   4,   6,   9,  13,  17,  21,  26,  31,  37,  44,  51,  58,
    66,  75,  84,  93, 103, 114, 125, 137, 149, 161, 174, 188, 202, 217, 232, 248,
  };
Graphics::Graphics(Canvas* canvas, uint8_t height, uint8_t width)
{
		this->canvas = canvas;
		this->height = height;
		this->width = width;
}

Graphics::Graphics(uint8_t*** render_target, uint8_t height, uint8_t width)
{
	this->render_target = render_target;
	this->height = height;
	this->width = width;
}

Graphics::~Graphics()
{

}

void Graphics::draw()
{
	if(this->canvas == nullptr)
	{
		LOG("Graphics Instance does not have a Canvas to draw to");
		return;
	}
	if(this->render_target == nullptr)
	{
		LOG("render_target is null");
		return;
	}

	for(int rows = 0; rows < this->height; rows++)
	{
		for(int cols = 0; cols < this->width; cols++)
		{
			this->SetCanvasPixel(cols, rows, Color(0xFF,	this->render_target[rows][cols][0],
															this->render_target[rows][cols][1],
															this->render_target[rows][cols][2])
												  );
		}
	}
}

void Graphics::drawWithMaps(std::vector<ZwConfig::PanelMap*>* panels)
{
	// LOG("drawWithMaps");
	if(this->canvas == nullptr)
	{
		LOG("Graphics Instance does not have a Canvas to draw to");
		return;
	}
	if(this->render_target == nullptr)
	{
		LOG("render_target is null");
		return;
	}
	// LOG("drawWithMaps not null");

	// LOG_INT(panels->size());
	for(ZwConfig::PanelMap* panel_map: *panels)
	{
		// panel_map->disp();
		int x = panel_map->source.p_top_left.x + panel_map->rot_constants.offset.x;
		int y = panel_map->source.p_top_left.y + panel_map->rot_constants.offset.y;

		// LOG_COLOR(this->render_target[y][x][0],
		// 		  this->render_target[y][x][1],
		// 		  this->render_target[y][x][2]);
		//Loop can switch between column and row major.
		for(int cols = panel_map->destination.p_top_left.x; cols <= panel_map->destination.p_bot_right.x; cols++)
		{
			// LOG_INT(rows);
			for(int rows = panel_map->destination.p_top_left.y ;rows <= panel_map->destination.p_bot_right.y; rows++)
			{
				// LOG_COLOR(this->render_target[y][x][0],
				// 		  this->render_target[y][x][1],
				// 		  this->render_target[y][x][2]);
				//Write pixel to canvas
				this->SetCanvasPixel(cols, rows, Color(0xFF,	this->render_target[y][x][0],
																this->render_target[y][x][1],
																this->render_target[y][x][2])
													   );
				if(panel_map->rot_constants.row_major)
					x += panel_map->rot_constants.increment.x;
				else
					y += panel_map->rot_constants.increment.y;
			}
			if(panel_map->rot_constants.row_major)
			{
				y += panel_map->rot_constants.increment.y;
				//reset the other
				x = panel_map->source.p_top_left.x + panel_map->rot_constants.offset.x;
			}
			else
			{
				x += panel_map->rot_constants.increment.x;
				//reset the other
				y = panel_map->source.p_top_left.y + panel_map->rot_constants.offset.y;
			}
		}
	}
	//as soon as we are done copying to the Canvas object, free the triple pointer.
	this->freeRenderTarget();
}
void Graphics::drawWithMapsFlat555(std::vector<ZwConfig::PanelMap*>* panels, ZwNetwork::SinkPacket pckt)
{
	if(this->canvas == nullptr)
	{
		LOG("Graphics Instance does not have a Canvas to draw to");
		return;
	}
	LOG("DWM: check frame data for null");
	if(pckt.data == nullptr)
	{
		LOG("render_target is null");
		return;
	}
	uint16_t *flt_buf = (uint16_t*)pckt.data;
	for(ZwConfig::PanelMap* panel_map: *panels)
	{
		// panel_map->disp();
		int x = panel_map->source.p_top_left.x + panel_map->rot_constants.offset.x;
		int y = panel_map->source.p_top_left.y + panel_map->rot_constants.offset.y;

		//Loop can switch between column and row major.
		for(int cols = panel_map->destination.p_top_left.x; cols <= panel_map->destination.p_bot_right.x; cols++)
		{
			// LOG_INT(rows);
			for(int rows = panel_map->destination.p_top_left.y ;rows <= panel_map->destination.p_bot_right.y; rows++)
			{
				//Write pixel to canvas
				this->SetCanvasPixel(cols, rows, Color(0xFF,	this->five_bit_to_eight_bit[(flt_buf[(y*256+x)] & 0x1f)],
																this->five_bit_to_eight_bit[((flt_buf[(y*256+x)] >> 5) & 0x1f)],
																this->five_bit_to_eight_bit[((flt_buf[(y*256+x)] >> 10) & 0x1f)])
													   );
				if(panel_map->rot_constants.row_major)
					x += panel_map->rot_constants.increment.x;
				else
					y += panel_map->rot_constants.increment.y;
			}
			if(panel_map->rot_constants.row_major)
			{
				y += panel_map->rot_constants.increment.y;
				//reset the other
				x = panel_map->source.p_top_left.x + panel_map->rot_constants.offset.x;
			}
			else
			{
				x += panel_map->rot_constants.increment.x;
				//reset the other
				y = panel_map->source.p_top_left.y + panel_map->rot_constants.offset.y;
			}
		}
	}
	free(pckt.data);
}

void Graphics::PlotPoint(uint8_t x, uint8_t y, Color color)
{
	if(x >= this->width || y >= height || this->render_target == nullptr)
		return;

	//Plot point into this->render_target
	this->render_target[y][x][0] = color.red;
	this->render_target[y][x][1] = color.green;
	this->render_target[y][x][2] = color.blue;
}

void Graphics::BlendPixels(int y, int x, uint8_t*** buf_one, uint8_t*** buf_two, uint8_t alpha_one, uint8_t alpha_two)
{
	this->render_target[y][x][0] = ((((uint16_t)buf_one[y][x][0] * alpha_one)/255) + ((uint16_t)buf_two[y][x][0] * alpha_two)/255);
	this->render_target[y][x][1] = ((((uint16_t)buf_one[y][x][1] * alpha_one)/255) + ((uint16_t)buf_two[y][x][1] * alpha_two)/255);
	this->render_target[y][x][2] = ((((uint16_t)buf_one[y][x][2] * alpha_one)/255) + ((uint16_t)buf_two[y][x][2] * alpha_two)/255);
}
void Graphics::BlendBuffers(int v_res, int h_res, uint8_t*** buf_one, uint8_t*** buf_two, uint8_t alpha_one, uint8_t alpha_two)
{
	if(buf_one == nullptr || buf_two == nullptr)
	{
		return;
	}
	for(int rows = 0; rows < v_res; rows++)
	{
		for(int cols = 0; cols < h_res; cols++)
		{
			BlendPixels(rows, cols, buf_one, buf_two, alpha_one, alpha_two);
		}
	}
}

void Graphics::PlotPoint(Point point, Color color)
{
	if(point.x >= this->width || point.y >= height || this->render_target == nullptr)
		return;

	//Plot point into this->render_target
	this->render_target[point.y][point.x][0] = color.red;
	this->render_target[point.y][point.x][1] = color.green;
	this->render_target[point.y][point.x][2] = color.blue;

}
void Graphics::PlotLine(Point p1,Point p2, Color color)
{
	int x1,x2,y1,y2;
	x1 = p1.x;
	x2 = p2.x;
	y1 = p1.y;
	y2 = p2.y;

	//Detect Horizontal/Vertical Lines
	if(x1 == x2)
	{
		//Vertical Line
		this->PlotLineVertical(p1, p2, color);
	}
	else if (y1 == y2)
	{
		//Horizontal Line
		this->PlotLineHorizontal(p1, p2, color);
	}
	else
	{
		//Normal Line
		if(std::abs(y2 - y1) < std::abs(x2-x1))
		{
			if(x1 > x2)
			{
				this->PlotLineLow(p2, p1, color);
			}
			else
			{
				this->PlotLineLow(p1, p2, color);
			}
		}
		else
		{
			if(y1 > y2)
			{
				this->PlotLineHigh(p2, p1, color);
			}
			else
			{
			this->PlotLineHigh(p1, p2, color);
			}
		}
	}
}
void Graphics::PlotLineHigh(Point p1,Point p2, Color color)
{
	//All coordinates will be in quadrant 1.
	///Cast to 32-bit signed integers.
	int dx, dy, D, x_inc, x, x1, x2, y1, y2;
	x1 = p1.x;
	x2 = p2.x;
	y1 = p1.y;
	y2 = p2.y;
	dx = x2 - x1;
	dy = y2 - y1;
	x_inc = 1;
	if(dx < 0)
	{
		x_inc = -1;
		dx = -dx;
	}
	D = (dx << 1) - dy;
	x = x1;

	for(int y = y1; y <= y2; y++)
	{
		this->PlotPoint(Point(x, y), color);
		if(D > 0)
		{
			x += x_inc;
			D += (dx - dy) << 1;
		}
		else
		{
			D += dx << 1;
		}
	}
}
void Graphics::PlotLineLow(Point p1,Point p2, Color color)
{
	//All coordinates will be in quadrant 1.
	///Cast to 32-bit signed integers.
	int dx, dy, D, y_inc, y, x1, x2, y1, y2;
	x1 = p1.x;
	x2 = p2.x;
	y1 = p1.y;
	y2 = p2.y;
	dx = x2 - x1;
	dy = y2 - y1;
	y_inc = 1;
	if(dy < 0)
	{
		y_inc = -1;
		dy = -dy;
	}
	D = (dy << 1) - dx;
	y = y1;

	for(int x = x1;  x<= x2; x++)
	{
		this->PlotPoint(Point(x, y), color);
		if(D > 0)
		{
			y += y_inc;
			D += (dy - dx) << 1;
		}
		else
		{
			D += dy << 1;
		}
	}
}
void Graphics::PlotLineHorizontal(Point p1,Point p2, Color color)
{
	uint8_t x_start, x_end;
	if(p2.x > p1.x)
	{
		x_start = p1.x;
		x_end = p2.x;
	}
	else
	{
		x_start = p2.x;
		x_end = p1.x;
	}

	//Y stays the same for the whole time
	for(uint8_t x = x_start; x <= x_end; x++)
	{
		this->PlotPoint(Point(x, p1.y), color);
	}
}
void Graphics::PlotLineVertical(Point p1,Point p2, Color color)
{
	uint8_t y_start, y_end;
	if(p2.y > p1.y)
	{
		y_start = p1.y;
		y_end = p2.y;
	}
	else
	{
		y_start = p2.y;
		y_end = p1.y;
	}

	//X stays the same for the whole time
	for(uint8_t y = y_start; y <= y_end; y++)
	{
		this->PlotPoint(Point(p1.x, y), color);
	}
}


void Graphics::PlotRectangle(Rectangle rect,Color color)
{
	uint8_t dx, dy;
	dx = rect.p_bot_right.x - rect.p_top_left.x;
	dy = rect.p_bot_right.y - rect.p_top_left.y;

	this->PlotLineVertical(rect.p_top_left, Point(rect.p_top_left.x, rect.p_top_left.y + dy), color);
	this->PlotLineVertical(Point(rect.p_top_left.x + dx,rect.p_top_left.y),rect.p_bot_right, color);
	this->PlotLineHorizontal(rect.p_top_left, Point(rect.p_top_left.x + dx, rect.p_top_left.y), color);
	this->PlotLineHorizontal(Point(rect.p_top_left.x,rect.p_top_left.y + dy),rect.p_bot_right, color);
}

void Graphics::PlotRectangleFilled(Rectangle rect,Color color)
{
	uint8_t dx, dy;
	dx = rect.p_bot_right.x - rect.p_top_left.x;
	dy = rect.p_bot_right.y - rect.p_top_left.y;

	for(uint8_t i = 0; i <= dx; i++)
	{
		this->PlotLineVertical(Point(rect.p_top_left.x + i, rect.p_top_left.y), Point(rect.p_top_left.x + i, rect.p_top_left.y + dy), color);
	}

}

void Graphics::PlotTriangle(Triangle tri, Color color)
{
	//Check that all point are with the screen
	if(!this->isPointOnScreen(tri.p1) && !this->isPointOnScreen(tri.p2) && !this->isPointOnScreen(tri.p3))
		return;

	this->PlotLine(tri.p1, tri.p2, color);
	this->PlotLine(tri.p1, tri.p3, color);
	this->PlotLine(tri.p3, tri.p2, color);
}
void Graphics::PlotTriangleFilled(Triangle tri, Color color)
{
	return; //TODO this function
}

void Graphics::PlotCircle(Circle circle, Color color)
{
	//only supports circles with odd radii, to support even radii, add 1 to radius, but do not render that midpoint.
	int r, x_mid, y_mid, y, x, err;
	r = circle.radius;
	x_mid = circle.center.x;
	y_mid = circle.center.y;
	x = 0;
	y = r;
	err = 3 - (r << 1);
	//Check for overflow issues
	if(!(x_mid + r < this->getWidth() && x_mid - r >= 0 && y_mid + r < this->getHeight() && y_mid - r >= 0))
		return;
	//Plot the middle points if radius is odd.
	this->PlotPoint(Point(x_mid, y_mid + r), color);
	this->PlotPoint(Point(x_mid, y_mid - r), color);
	this->PlotPoint(Point(x_mid + r, y_mid), color);
	this->PlotPoint(Point(x_mid - r, y_mid), color);

	while(y >= x)
	{
		x++;
		if(err > 0)
		{
			err += ((x - y) << 2) + 10;
			y--;
		}
		else
		{
			err += (x << 2) + 6;
		}
		//Plot the points
		this->PlotPoint(Point(x_mid + x,y_mid + y), color);
		this->PlotPoint(Point(x_mid + x,y_mid - y), color);
		this->PlotPoint(Point(x_mid - x,y_mid + y), color);
		this->PlotPoint(Point(x_mid - x,y_mid - y), color);
		this->PlotPoint(Point(x_mid + y,y_mid + x), color);
		this->PlotPoint(Point(x_mid + y,y_mid - x), color);
		this->PlotPoint(Point(x_mid - y,y_mid + x), color);
		this->PlotPoint(Point(x_mid - y,y_mid - x), color);

	}
}
void Graphics::PlotCircleFilled(Circle circle, Color color)
{
	//only supports circles with odd radii, to support even radii, add 1 to radius, but do not render that midpoint.
	int r, x_mid, y_mid, y, x, err;
	r = circle.radius;
	x_mid = circle.center.x;
	y_mid = circle.center.y;
	x = 0;
	y = r;
	err = 3 - (r << 1);
	//Check for overflow issues
	if(!(x_mid + r < this->getWidth() && x_mid - r >= 0 && y_mid + r < this->getHeight() && y_mid - r >= 0))
		return;
	//Plot the middle points if radius is odd.
	this->PlotLine(circle.center, Point(x_mid, y_mid + r), color);
	this->PlotLine(circle.center, Point(x_mid, y_mid - r), color);
	this->PlotLine(circle.center, Point(x_mid + r, y_mid), color);
	this->PlotLine(circle.center, Point(x_mid - r, y_mid), color);

	while(y >= x)
	{
		x++;
		if(err > 0)
		{
			err += ((x - y) << 2) + 10;
			y--;
		}
		else
		{
			err += (x << 2) + 6;
		}
		//Plot the points
		this->PlotLine(circle.center, Point(x_mid + x,y_mid + y), color);
		this->PlotLine(circle.center, Point(x_mid + x,y_mid - y), color);
		this->PlotLine(circle.center, Point(x_mid - x,y_mid + y), color);
		this->PlotLine(circle.center, Point(x_mid - x,y_mid - y), color);
		this->PlotLine(circle.center, Point(x_mid + y,y_mid + x), color);
		this->PlotLine(circle.center, Point(x_mid + y,y_mid - x), color);
		this->PlotLine(circle.center, Point(x_mid - y,y_mid + x), color);
		this->PlotLine(circle.center, Point(x_mid - y,y_mid - x), color);

	}
}

void Graphics::Gradient1D(Gradient grad, Rectangle rect)
{
	//Ensure gradient window is contained by the screen
	if(!this->isPointOnScreen(rect.p_top_left) || !this->isPointOnScreen(rect.p_bot_right))
		return;
	ZwGraphics::Color color = grad.start;

	int dx, dy, r_inc, g_inc, b_inc;
	dx = rect.p_bot_right.x - rect.p_top_left.x;
	dy = rect.p_bot_right.y - rect.p_top_left.y;

	r_inc = ((int)grad.end.red - (int)grad.start.red)/dx;
	g_inc = ((int)grad.end.green - (int)grad.start.green)/dx;
	b_inc = ((int)grad.end.blue - (int)grad.start.blue)/dx;

	for(int i = 0; i <= dx; i++)
	{
		this->PlotLineVertical(Point(rect.p_top_left.x + i, rect.p_top_left.y), Point(rect.p_top_left.x + i, rect.p_top_left.y + dy), color);
		color.red += r_inc;
		color.green += g_inc;
		color.blue += b_inc;
	}
}
void Graphics::Gradient2D(Gradient grad_left_right, Gradient grad_top_bot, Rectangle rect)
{
	//Ensure gradient window is contained by the screen
	if(!this->isPointOnScreen(rect.p_top_left) || !this->isPointOnScreen(rect.p_bot_right))
		return;
	Color color = grad_left_right.start;
	color.red = this->sadd8(color.red, grad_top_bot.start.red);
	color.green = this->sadd8(color.green, grad_top_bot.start.green);
	color.blue = this->sadd8(color.blue, grad_top_bot.start.blue);

	int dx, dy, rx_inc, gx_inc, bx_inc, ry_inc, gy_inc, by_inc, rx_tmp, gx_tmp, bx_tmp;
	dx = rect.p_bot_right.x - rect.p_top_left.x;
	dy = rect.p_bot_right.y - rect.p_top_left.y;

	rx_inc = ((int)grad_left_right.end.red - (int)grad_left_right.start.red)/dx;
	gx_inc = ((int)grad_left_right.end.green - (int)grad_left_right.start.green)/dx;
	bx_inc = ((int)grad_left_right.end.blue - (int)grad_left_right.start.blue)/dx;

	rx_tmp = grad_left_right.start.red;
	gx_tmp = grad_left_right.start.green;
	bx_tmp = grad_left_right.start.blue;

	ry_inc = ((int)grad_top_bot.end.red - (int)grad_top_bot.start.red)/dy;
	gy_inc = ((int)grad_top_bot.end.green - (int)grad_top_bot.start.green)/dy;
	by_inc = ((int)grad_top_bot.end.blue - (int)grad_top_bot.start.blue)/dy;

	for(int i = 0; i <= dx; i++)
	{
		for(int j = 0; j <= dy; j++)
		{
			this->PlotPoint(rect.p_top_left.x + i, rect.p_top_left.y + j, color);
			color.red = this->sadd8(color.red, ry_inc);
			color.green = this->sadd8(color.green, gy_inc);
			color.blue = this->sadd8(color.blue, by_inc);
		}
		rx_tmp += rx_inc;
		gx_tmp += gx_inc;
		bx_tmp += bx_inc;

		color.red = this->sadd8(rx_tmp, rx_inc);
		color.green = this->sadd8(gx_tmp, gx_inc);
		color.blue = this->sadd8(bx_tmp, bx_inc);
	}
}

/************************************************
 * WriteChar Function - function plots a single *
 * character.                                   *
 ************************************************/
FontStatus Graphics::PlotChar(Point position, uint8_t letter, Font font, Color color)
{
	//Check if the character is off of the screen
	if(position.x > (this->getWidth() - font.width) || position.y > (this->getHeight() - font.num_rows))
	{
		return FontPrintCutoff;
	}
	//from width and row num, find initial shift value.
	uint8_t shift_val = (font.row_size * 8) - font.width;
	//set initial x value
	uint8_t x = position.x + (font.width - 1);
	//row byte loop
	for(int row_byte = (font.row_size - 1); row_byte >= 0; row_byte--)
	{
		//loop through each mask value
		while(shift_val < 8)
		{
			//Draw column
			for(uint8_t y = position.y; (y - (uint8_t)position.y) < font.num_rows;y++)
			{
				if(((font.font[(letter * font.num_rows*font.row_size) + row_byte + ((y - position.y) * font.row_size)] >> shift_val) & 0x01) == 0x01)
				{
					this->PlotPoint(x,y,color);
				}
			}
			shift_val++;
			x--;
		}
		shift_val = 0;
	}
	return FontSuccess;
}
/************************************************
 * WriteString Function - function plots a string *
 ************************************************/
FontStatus Graphics::PlotString(Point position, const char *string, Font font, Color color){
    FontStatus error_code=FontSuccess;
    while(*string) // will loop until NULL is reached (0x00)
    {
        error_code=this->PlotChar(position, *string, font, color); // write the current character to the screen

        if(error_code!=FontSuccess) // if WriteChar returns an error
            return(error_code);      // stop and return it to the user

        position.x += font.width;                       // increment the x axis to make a spot for the new character
        //PlotVLine(x0,y0,7*tsize,backcolor);// plot a blank line between characters
        string++;                          // increment the current character
    }
    return(FontSuccess); // return success code
}


Font Graphics::fontFactory(Fonts font_name)
{
	switch (font_name)
	{
		case Font4x6 : return Font(6, 1, 4, this->console_font_4x6);
		case Font5x8 : return Font(8, 1, 5, this->console_font_5x8);
		case Font7x9 : return Font(9, 1, 7, this->console_font_7x9);
		case Font9x16 : return Font(16, 2, 9, this->console_font_9x16);
		default : return Font(0,0,0,nullptr);
	}
}

uint8_t Graphics::getHeight()
{
	return this->height;
}
void Graphics::setHeight(uint8_t height)
{
	this->height = height;
}
uint8_t Graphics::getWidth()
{
	return this->width;
}
void Graphics::setWidth(uint8_t width)
{
	this->width = width;
}

void Graphics::SetCanvasPixel(uint8_t x, uint8_t y, Color color)
{
	this->canvas->SetPixel(x, y, color.red, color.green, color.blue);
}

void Graphics::setRenderTarget(uint8_t*** render_target)
{
	if(render_target == nullptr)
	{
		return;
	}

	this->render_target = render_target;
}

void Graphics::clearRenderTarget()
{
	for(int y = 0; y < this->getHeight(); y++)
	{
		for(int x = 0; x < this->getWidth(); x++)
		{
			this->render_target[y][x][0] = 0;
			this->render_target[y][x][1] = 0;
			this->render_target[y][x][2] = 0;
		}
	}
}

void Graphics::freeRenderTarget()
{
	if(this->render_target != nullptr)
		deallocTriplePointer<uint8_t>(this->render_target, this->height, this->width);
}

bool Graphics::isPointOnScreen(Point pt)
{
	bool ret_val = true;

	if(pt.x >= this->getWidth() || pt.y >= this->getHeight())
		ret_val = false;

	return ret_val;
}

uint8_t Graphics::sadd8(uint8_t a, uint8_t b)
{
	return (a > 0xFF - b) ? 0xFF : a + b;
}
/*	const u16 intensity_u16 = (u16)(intensity * (float)(0xFFFF));
	u8  r = (u8)( (((outColor >>  0) & 0x1F) * intensity_u16) >> 16 );
	u8  g = (u8)( (((outColor >>  5) & 0x1F) * intensity_u16) >> 16 );
	u8  b = (u8)( (((outColor >> 10) & 0x1F) * intensity_u16) >> 16 );
*/
uint8_t*** Graphics::convertFlatBufferToTriplePointer(ZwNetwork::SinkPacket frame_packet)
{
	uint8_t ***ret_val = nullptr;

	switch (frame_packet.header.color_mode)
	{
		case 0x00 : {
			//NDS RGB555 with Intensity
			ret_val = this->rgb555torgb888Intensity(frame_packet);
			break;
		}
		case 0x01 : {
			//NDS RGB555 no intensity
			ret_val = this->rgb555torgb888(frame_packet);
			break;
		}
		case 0x02 : {
			//Flat RGB888 to Triple Pointer.
			ret_val = this->flatRgb888torgb888TriplePointer(frame_packet);
			break;
		}
		default : {
			break;
		}
	}

	this->width = frame_packet.header.h_res;
	this->height = frame_packet.header.v_res;

	return ret_val;
}

uint8_t*** Graphics::rgb555torgb888Intensity(ZwNetwork::SinkPacket frame_packet)
{
	if(frame_packet.data == nullptr)
		return nullptr;
	uint8_t ***rgb888 = allocTriplePointer<uint8_t>(frame_packet.header.v_res + 1, frame_packet.header.h_res + 1, 3, 0x00);
	uint16_t* buffer = (uint16_t*)frame_packet.data;
	int num_pixels = (frame_packet.header.h_res + 1) * (frame_packet.header.v_res + 1);
	int flat_idx = 0;
	//convert floating point intensity 0.0f <= intensity <= 1.0f to a uint16_t
	const uint16_t intensity = (uint16_t)(frame_packet.header.intensity * (float)(0xFFFF));

	for(int y = 0; y < frame_packet.header.v_res + 1; y++)
	{
		for(int x = 0; x < frame_packet.header.h_res + 1 && flat_idx < num_pixels; x++)
		{
			//Code borrowed from Desmume under GPL2
			rgb888[y][x][0] = (uint8_t)( (((buffer[flat_idx] >>  0) & 0x1F) * intensity) >> 16 );
			rgb888[y][x][1] = (uint8_t)( (((buffer[flat_idx] >>  5) & 0x1F) * intensity) >> 16 );
			rgb888[y][x][2] = (uint8_t)( (((buffer[flat_idx] >>  10) & 0x1F) * intensity) >> 16 );
			flat_idx++;
		}
	}

	delete frame_packet.data;

	return rgb888;
}

uint8_t*** Graphics::rgb555torgb888(ZwNetwork::SinkPacket frame_packet)
{
	// LOG("rgb555torgb888");
	if(frame_packet.data == nullptr)
		return nullptr;
	// LOG("rgb555torgb888 not null");
	uint8_t ***rgb888 = allocTriplePointer<uint8_t>(frame_packet.header.v_res + 1, frame_packet.header.h_res + 1, 3, 0x00);
	uint16_t* buffer = (uint16_t*)frame_packet.data;
	int num_pixels = (frame_packet.header.h_res + 1) * (frame_packet.header.v_res + 1);
	// LOG("Num Pixels:");
	// LOG_INT(num_pixels);
	int flat_idx = 0;

	for(int y = 0; y < frame_packet.header.v_res + 1; y++)
	{
		for(int x = 0; x < frame_packet.header.h_res + 1 && flat_idx < num_pixels; x++)
		{
			//Code borrowed from Desmume under GPL2
			rgb888[y][x][0] = this->five_bit_to_eight_bit[((buffer[flat_idx] >>  0) & 0x1F)];
			rgb888[y][x][1] = this->five_bit_to_eight_bit[((buffer[flat_idx] >>  5) & 0x1F)];
			rgb888[y][x][2] = this->five_bit_to_eight_bit[((buffer[flat_idx] >>  10) & 0x1F)];
			flat_idx++;
		}
	}

	delete frame_packet.data;

	return rgb888;
}

uint8_t*** Graphics::flatRgb888torgb888TriplePointer(ZwNetwork::SinkPacket frame_packet)
{
	if(frame_packet.data == nullptr)
		return nullptr;
	uint8_t ***rgb888 = allocTriplePointer<uint8_t>(frame_packet.header.v_res + 1, frame_packet.header.h_res + 1, 3, 0x00);
	uint8_t* buffer = (uint8_t*)frame_packet.data;
	int num_pixels = (frame_packet.header.h_res + 1) * (frame_packet.header.v_res + 1);
	int flat_idx = 0;

	for(int y = 0; y < frame_packet.header.v_res + 1; y++)
	{
		for(int x = 0; x < frame_packet.header.h_res + 1 && flat_idx < num_pixels; x++)
		{
			rgb888[y][x][0] = buffer[flat_idx];
			rgb888[y][x][1] = buffer[flat_idx + 1];
			rgb888[y][x][2] = buffer[flat_idx + 2];
			flat_idx += 3;
		}
	}

	delete frame_packet.data;

	return rgb888;
}

}

