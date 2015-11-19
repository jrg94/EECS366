
/*
===============================================================================
	Headers
===============================================================================
*/

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "read_tga.h"

/*
===============================================================================
	TGA
===============================================================================
*/

static bool TGAReadError = false;

void ReadData(std::ifstream &file, char* data, uint size)
{
    if (!file.is_open())
        return;
    uint currentLocation = file.tellg();
    currentLocation += size;
    file.read(data, size);
    if (currentLocation != file.tellg())
    {
        TGAReadError = true;
    }
}

TGA::TGA()
{
    this->pixels		= NULL;
	this->pixelDepth	= 0;
	this->alphaDepth	= 0;
	this->width			= 0;
	this->height		= 0;
	this->type			= ImageType_Undefined;
	this->loaded		= false;
}

TGA::TGA(const char *fileName)
{
    this->pixels		= NULL;
	this->pixelDepth	= 0;
	this->alphaDepth	= 0;
	this->width			= 0;
	this->height		= 0;
	this->type			= ImageType_Undefined;
	this->loaded		= false;

    bool success = this->LoadFromFile(fileName);

}

TGA::~TGA()
{
    Clear();
}

bool TGA::LoadFromFile(const char *fileName)
{
	/*
	This function loads a subset of TGA images. The TGA header is shown below
	for reference.

	byte	IDLength;			// Size of ID field that follows 18 byte header (0 usually)
	byte	IDColorMapType;		// Type of colour map 0=none, 1=has palette
	byte	IDImageType;		// Type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    short	colourmapstart;     // Ignored
    short	colourmaplength;    // Ignored
    byte	colourmapbits;      // Ignored

    short	xstart;             // Assumed 0
    short	ystart;             // Assumed 0
    short	width;              // Image width in pixels
    short	height;             // Image height in pixels
    byte	bits;               // Image bits per pixel 8,16,24,32
    byte	descriptor;         // The last four bits of the descriptor are the alpha bits
	*/

    if (this->loaded == true)
	{
        this->Clear();
	}
    this->loaded = false;

    std::ifstream file;
    file.open(fileName, std::ios::binary);
    if (!file.is_open())
	{
        return false;
	}

    bool rle			= false;
    bool truecolor		= false;
    uint currentPixel	= 0;
    byte ch_buf1, ch_buf2;
    byte buf1[1000];

    byte IDLength;
    byte IDColorMapType;
    byte IDImageType;

	// Read the first three header fields

    ReadData(file, (char*)&IDLength,		1);
    ReadData(file, (char*)&IDColorMapType,	1);
    ReadData(file, (char*)&IDImageType,		1);

	// No support for palettes

	if (IDColorMapType == 1)
	{
        return false;
	}

    switch (IDImageType)
    {
    case 2:
            truecolor = true;
            break;
    case 3:
            this->type = ImageType_Greyscale;
            break;
    case 10:
            rle = true;
            truecolor = true;
            break;
    case 11:
            rle = true;
            this->type = ImageType_Greyscale;
            break;
    default:
            return false;
    }

	// Ignore palette info

    file.seekg(5, std::ios::cur);

	// Assume origin to be (0, 0)

    file.seekg(4, std::ios::cur);

	// Read the image dimensions and pixel depth

    ReadData(file, (char*)&(this->width),		2);
    ReadData(file, (char*)&(this->height),		2);
    ReadData(file, (char*)&(this->pixelDepth),	1);

    if (! ((this->pixelDepth == 8)	|| (this->pixelDepth ==  24) ||
		   (this->pixelDepth == 16) || (this->pixelDepth == 32))	)
	{
        return false;
	}

	// Read descriptor:	00vhaaaa
	//		h horizontal flip
	//		v vertical flip
	//		a alpha bits

    ReadData(file, (char*)&ch_buf1, 1); 
    
    ch_buf2				= 15; //00001111;
    this->alphaDepth	= ch_buf1 & ch_buf2;

    if (! ((this->alphaDepth == 0) || (this->alphaDepth == 8)))
	{
        return false;
	}

    if (truecolor)
    {
        this->type = ImageType_RGB;
        if (pixelDepth == 32)
		{
            this->type = ImageType_RGBA;
		}
    }

    if (this->type == ImageType_Undefined)
	{
        return false;
	}

	// Move the file pointer to the beginning of the image data

    file.seekg(IDLength, std::ios::cur);

	// Allocate memory for pixel data

	int imageDataSize = this->width * this->height * (this->pixelDepth/8);
	pixels = new byte[imageDataSize];

    if (!rle)
	{
        ReadData(file, (char*)pixels, imageDataSize);
	}

    else
    {
        while (currentPixel < this->width * this->height -1)
        {
            ReadData(file, (char*)&ch_buf1, 1);
            if ((ch_buf1 & 128) == 128)
            {   
				// This is an rle packet
                ch_buf2 = (byte)((ch_buf1 & 127) + 1);   // how many pixels are encoded using this packet
                ReadData(file, (char*)buf1, this->pixelDepth/8);
                for (uint i=currentPixel; i<currentPixel+ch_buf2; i++)
				{
                    for (uint j=0; j<pixelDepth/8; j++)
					{
                        pixels[i*pixelDepth/8+j] = buf1[j];
					}
				}
                currentPixel += ch_buf2;
            }
            else
            {   
				// This is a raw packet
                ch_buf2 = (byte)((ch_buf1 & 127) + 1);
                ReadData(file, (char*)buf1, pixelDepth/8*ch_buf2);
                for (uint i=currentPixel; i<currentPixel+ch_buf2; i++)
				{
                    for (uint j=0; j<this->pixelDepth/8; j++)
					{
                        pixels[i*this->pixelDepth/8+j] =  buf1[(i-currentPixel)*this->pixelDepth/8+j];
					}
				}
                currentPixel += ch_buf2;
            }
        }
    }

    if (TGAReadError == true)
    {
        Clear();
        return false;
    }
    loaded = true;

    // Swap BGR(A) to RGB(A)

    byte swap;
    if ((type == ImageType_RGB) || (type == ImageType_RGBA))
        if ((this->pixelDepth == 24) || (this->pixelDepth == 32))
            for (uint i= 0; i<this->width*this->height; i++)
            {
                swap = pixels[i*this->pixelDepth/8];
                pixels[i*this->pixelDepth/8] = pixels[i*this->pixelDepth/8+2];
                pixels[i*this->pixelDepth/8+2] = swap;
            }

    return true;
}

void TGA::Clear()
{
    if (this->pixels != NULL)
	{
		delete [] pixels;
	}

	this->pixels		= NULL;
	this->pixelDepth	= 0;
	this->alphaDepth	= 0;
	this->width			= 0;
	this->height		= 0;
	this->type			= ImageType_Undefined;
	this->loaded		= false;
}

byte * TGA::GetPixels(void)
{
	
	return this->pixels;
}

uint TGA::GetPixelDepth(void)
{
	return this->pixelDepth;
}

uint TGA::GetAlphaDepth(void)
{
	return this->alphaDepth;
}

uint TGA::GetWidth(void)
{
	return this->width;
}

uint TGA::GetHeigth(void)
{
	return this->height;
}

ImageType TGA::GetImageType(void)
{
	return this->type;
}