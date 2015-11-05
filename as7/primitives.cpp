#include "primitives.h"

void BresenhamLine(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c)
{
	double deltax, deltay;			// For Bresenham's
	double slope;				// Ditto

	deltax = x_2 - x_1;		// Change in x
	deltay = y_2 - y_1;		// Change in y

	if (deltax == 0)		// Need to avoid div by zero
	   linePosSteep(fb, x_1, y_1, x_2, y_2, c);

	else {
	   slope = deltay / deltax;

	   if (slope > 0.0) {
		if (slope > 1.0)
		   linePosSteep(fb, x_1, y_1, x_2, y_2, c);
		else
		   linePosShallow(fb, x_1, y_1, x_2, y_2, c);
	   } else {
		if (slope > -1.0)
		   lineNegShallow(fb, x_1, y_1, x_2, y_2, c);
		else
		   lineNegSteep(fb, x_1, y_1, x_2, y_2, c);
	   }
	}
}



void linePosSteep(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c)
 {
	int dx, dy, p, twodx, twodxdy, x, y, yend;

	dx = abs(x_1 - x_2);
	dy = abs(y_1 - y_2);
	twodx = 2 * dx;
	p = twodx - dy;
	twodxdy = 2 * (dx - dy);
	y = y_1;		// Dummy initialization
	yend = y_2;		// Ditto
	x = x_1;

	if (x_1 == x_2)		// Vertical line
	{
	   if (y_2 > y_1) {
		yend = y_2;
		y = y_1;
	   } else {
		yend = y_1;
		y = y_2;
	   }
	   for (; y <= yend; y++)
		fb->buffer[x_1][y] = c;
	}
	if (x_1 > x_2) {
	   x = x_2;
	   y = y_2;
	   yend = y_1;
	}
	if (x_1 < x_2) {
	   x = x_1;
	   y = y_1;
	   yend = y_2;
	}
	if (x_1 != x_2)
	{
	   fb->buffer[x_1][y_1] = c;
	   while (y < yend) {
		y++;
		if (p < 0)
		   p += twodx;
		else {
		   x++;
		   p += twodxdy;
		}
		fb->buffer[x][y] = c;
	   }
	}
}

void linePosShallow(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c)
 {
	int dx, dy, p, twody, twodydx, x, y, xend;

	dx = abs(x_1 - x_2);
	dy = abs(y_1 - y_2);
	twody = 2 * dy;
	p = twody - dx;
	twodydx = 2 * (dy - dx);

	if (x_1 > x_2) {
	   x = x_2;
	   y = y_2;
	   xend = x_1;
	} else {
	   x = x_1;
	   y = y_1;
	   xend = x_2;
	}
	fb->buffer[x][y] = c;

	while (x < xend) {
	   x++;
	   if (p < 0)
		p += twody;
	   else {
		y++;
		p += twodydx;
	   }
	   fb->buffer[x][y] = c;
	}
}

void lineNegShallow(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c)
{
	int dx, dy, p, twody, twodydx, x, y, xend;

	dx = abs(x_2 - x_1);
	dy = abs(y_2 - y_1);
	p = 2 * dy - dx;
	twody = 2 * dy;
	twodydx = 2 * (dy - dx);

	if (x_1 > x_2) {
	   x = x_2;
	   y = y_2;
	   xend = x_1;
	} else {
	   x = x_1;
	   y = y_1;
	   xend = x_2;
	}
	fb->buffer[x][y] = c;

	while (x < xend) {
	   x++;
	   if (p < 0)
		p += twody;
	   else {
		y--;
		p += twodydx;
	   }
	   fb->buffer[x][y] = c;
	}
}

void lineNegSteep(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c)
{
	int dx, dy, p, twodx, twodxdy, x, y, yend;

	dx = abs (x_2 - x_1);
	dy = abs (y_2 - y_1);
	p = 2 * dx - dy;
	twodx = 2 * dx;
	twodxdy = 2 * (dx - dy);

	if (x_1 > x_2) {
	   x = x_2;
	   y = y_2;
	   yend = y_1;
	} else {
	   x = x_1;
	   y = y_1;
	   yend = y_2;
	}
	fb->buffer[x][y] = c;

	while (y > yend) {
	   y--;
	   if (p < 0)
		p += twodx;
	   else {
		x++;
		p += twodxdy;
	   }
	   fb->buffer[x][y] = c;
	}
}