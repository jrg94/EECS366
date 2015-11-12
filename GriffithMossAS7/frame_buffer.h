#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <vector>
#include "color.h"

using namespace std;

class Pixel
{
public:
	Color color;
	double z_value;

	Pixel()
	{
		color = Color(0,0,0);
		z_value = 0;
	}
	Pixel(Color cc)
	{
		color = cc;
		z_value = 0;
	}
	Pixel(Color cc, double depth)
	{
		color = cc;
		z_value = depth;
	}
};

class FrameBuffer
{
	Pixel *storage_array;

public:
	Pixel **buffer;
	int x_res, y_res;

	// pixels can be accessed as fb->buffer[x][y]
	// or by fb->GetPixel(x,y) and fb->SetPixel(x,y)
	// SetPixel is bounds-checked

	FrameBuffer(int x_dimension, int y_dimension)
	{
		storage_array = new Pixel[x_dimension * y_dimension];
		buffer = new Pixel *[x_dimension];

		for(unsigned int n = 0; n < x_dimension; ++n)
			buffer[n] = &storage_array[n * x_dimension];

		x_res = x_dimension;
		y_res = y_dimension;

	}

	~FrameBuffer()
	{
		delete [] buffer[0];
		delete [] buffer;
	}

	// resizing the buffer deletes its current contents!
	void Resize(int x_dimension, int y_dimension)
	{
		delete [] buffer[0];
		delete [] buffer;

		storage_array = new Pixel[x_dimension * y_dimension];
		buffer = new Pixel *[x_dimension];

		for(unsigned int n = 0; n < x_dimension; ++n)
			buffer[n] = &storage_array[n * x_dimension];

		x_res = x_dimension;
		y_res = y_dimension;
	}

	Pixel GetPixel(int x, int y)
	{
		return buffer[x][y];
	}

	void SetPixel(int x, int y, Color c)
	{
		if(x < 0 || x >= x_res || y < 0 || y >= y_res)
			return;

		buffer[x][y].color = c;
		buffer[x][y].z_value = 0;
	}
	void SetPixel(int x, int y, Color c, double depth)
	{
		buffer[x][y].color = c;
		buffer[x][y].z_value = depth;
	}

	int GetWidth()
	{
		return x_res;
	}

	int GetHeight()
	{
		return y_res;
	}
};


#endif