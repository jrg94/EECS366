#ifndef COLOR_H
#define COLOR_H


class Color
{
public:
	double r,g,b;

	Color() { r = 0; g = 0; b = 0;}
	Color(double rr, double gg, double bb)
	{
		r = rr;
		g = gg;
		b = bb;
	}
	Color operator*(double num)
	{
		return Color(r * num, g * num, b * num);
	}
	Color operator*(Color c)
	{
		return Color(r * c.r, g * c.g, b * c.b);
	}

	Color operator+(Color c)
	{
		return Color(c.r + r, c.g + g, c.b + b);
	}
	Color operator-(Color c)
	{
		return Color(c.r - r, c.g - g, c.b - b);
	}
};

#endif