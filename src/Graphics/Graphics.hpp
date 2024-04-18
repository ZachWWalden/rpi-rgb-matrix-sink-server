#pragma once
#include "stdint.h"
#include "../../rpi-rgb-led-matrix/include/led-matrix.h"

using rgb_matrix::Canvas;

namespace ZwGraphics{
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

class Graphics
{
	//attributes
	public:
		static const Color BLACK;
		static const Color WHITE;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color YELLOW;
		static const Color MAGENTA;
		static const Color CYAN;
		static const Color TRANSPARENT;
	private:
		Canvas* canvas = nullptr;
		uint8_t height;
		uint8_t width;
		uint8_t*** render_target = nullptr;

		static uint8_t console_font_4x6[];
		static uint8_t console_font_5x8[];
		static uint8_t console_font_7x9[];
		static uint8_t console_font_9x16[];

	//methods
	public:
		Graphics(Canvas* canvas, uint8_t height, uint8_t width);
		Graphics(uint8_t*** render_target, uint8_t height, uint8_t width);
		~Graphics();

		void draw();

		void PlotPoint(uint8_t x, uint8_t y, Color color);

		void PlotPoint(ZwGraphics::Point point, Color color);

		void PlotLine(Point p1,Point p2, Color color);
		void PlotLineHorizontal(Point p1,Point p2, Color color);
		void PlotLineVertical(Point p1,Point p2, Color color);

		void PlotRectangle(Rectangle rect,Color color);
		void PlotRectangleFilled(Rectangle rect,Color color);

		void PlotTriangle(Triangle, Color color);
		void PlotTriangleFilled(Triangle, Color color);

		void PlotCircle(Circle circle, Color color);
		void PlotCircleFilled(Circle circle, Color color);

		void BlendPixels(int y, int x, uint8_t*** buf_one, uint8_t*** buf_two, uint8_t alpha_one, uint8_t alpha_two);
		void BlendBuffers(int v_res, int h_res, uint8_t*** buf_one, uint8_t*** buf_two, uint8_t alpha_one, uint8_t alpha_two);
		void Gradient1D(Gradient grad, Rectangle rect);
		void Gradient2D(Gradient grad_left_right, Gradient grad_top_bot, Rectangle rect);

		uint8_t getHeight();
		void setHeight(uint8_t height);

		uint8_t getWidth();
		void setWidth(uint8_t width);

		FontStatus PlotChar(Point position, uint8_t letter, Font font, Color color);
		FontStatus PlotString(Point position, const char *string, Font font, Color color);

		Font fontFactory(Fonts font_name);

		void setRenderTarget(uint8_t*** render_target);
		void clearRenderTarget();
	private:

		void PlotLineHigh(Point p1, Point p2, Color color);
		void PlotLineLow(Point p1, Point p2, Color color);

		void SetCanvasPixel(uint8_t x, uint8_t y, Color color);

		bool isPointOnScreen(Point pt);

		uint8_t sadd8(uint8_t a, uint8_t b);
};

}
