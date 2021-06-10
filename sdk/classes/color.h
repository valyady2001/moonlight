#pragma once
#include <d3dx9.h>
#include <cstdint>

struct color {
	int a, r, g, b;
	color() = default;
	color( const float r , const float g , const float b , const float a = 1.f )
	{
		this->r = r*255;
		this->g = g*255;
		this->b = b*255;
		this->a = a*255;
	}
	color(const int r, const int g, const int b, const int a = 255) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	explicit color(const uint32_t color) {
		this->a = (color >> 24) & 0xff;
		this->r = (color >> 16) & 0xff;
		this->g = (color >> 8) & 0xff;
		this->b = (color & 0xff);
	}

	static color from_uint(const uint32_t uint) {
		return color(uint);
	}

	static D3DCOLOR to_d3d(const color col) {
		return D3DCOLOR_ARGB(col.a, col.r, col.g, col.b);
	}

	static color from_hsb(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return color(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2)
		{
			return color(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3)
		{
			return color(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4)
		{
			return color(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return color(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else
		{
			return color(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}

	static color black(const int a = 255) { return {0, 0, 0, a}; }
	static color white(const int a = 255) { return {255, 255, 255, a}; }
	static color red(const int   a = 255) { return {255, 0, 0, a}; }
	static color green(const int a = 255) { return {0, 255, 0, a}; }
	static color blue(const int  a = 255) { return {0, 0, 255, a}; }
	static color yellow(const int  a = 255) { return {255, 255, 0, a}; }
	static color from_float(float* clr, const int  a = 255) { return {int(clr[0] * 255.f),int(clr[1] * 255.f) ,int(clr[2] * 255.f) ,a}; }
};