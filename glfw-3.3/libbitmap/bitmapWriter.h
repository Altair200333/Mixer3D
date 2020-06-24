#pragma once
#include "libbitmap.h"

class BitmapWriter
{
public:	
	virtual bool save(const Bitmap& img, std::string file_name) = 0;
	virtual Bitmap loadBMP(const std::string& path) = 0;
	virtual Bitmap loadJPG(const std::string& path) = 0;

};