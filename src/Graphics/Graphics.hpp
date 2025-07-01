#pragma once
#include "../../rpi-rgb-led-matrix/include/led-matrix.h"

#include "../Config/Config.hpp" //this file includes GraphicsStructs.hpp
#include "../Network/Network.hpp"
#include <cstdint>

using rgb_matrix::Canvas;

namespace ZwGraphics{
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

		static const uint8_t five_bit_to_eight_bit[];

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
		void drawWithMaps(std::vector<ZwConfig::PanelMap*>* panels);

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

		uint8_t*** convertFlatBufferToTriplePointer(ZwNetwork::SinkPacket frame_packet);
	private:
		void freeRenderTarget();

		void PlotLineHigh(Point p1, Point p2, Color color);
		void PlotLineLow(Point p1, Point p2, Color color);

		void SetCanvasPixel(uint8_t x, uint8_t y, Color color);

		bool isPointOnScreen(Point pt);

		uint8_t sadd8(uint8_t a, uint8_t b);

		uint8_t*** rgb555torgb888Intensity(ZwNetwork::SinkPacket frame_packet);
		uint8_t*** rgb555torgb888(ZwNetwork::SinkPacket frame_packet);
		uint8_t*** flatRgb888torgb888TriplePointer(ZwNetwork::SinkPacket frame_packet);
};

}
