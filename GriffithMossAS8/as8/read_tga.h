
#ifndef READTGA__H
#define READTGA__H



#include <string>


typedef unsigned char byte;
typedef unsigned int  uint;

enum ImageType
{
	ImageType_Undefined,
	ImageType_RGB,
	ImageType_RGBA,
	ImageType_Greyscale
};

class TGA
{
public:
    TGA();
    TGA(const char *fileName);
    virtual ~TGA();
     
	byte *		GetPixels(void);
	uint		GetPixelDepth(void);
	uint		GetAlphaDepth(void);
	uint		GetWidth(void);
	uint		GetHeigth(void);
	ImageType	GetImageType(void);
	
private:
	byte	   *pixels;
	uint		pixelDepth;
	uint		alphaDepth;
	uint		width;
	uint		height;
	ImageType	type;
	bool		loaded;

	bool LoadFromFile(const char *fileName);
	void Clear();
};

#endif READTGA__H