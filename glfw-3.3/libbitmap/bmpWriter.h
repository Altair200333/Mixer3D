#pragma once
#include "bitmapWriter.h"
#include <fstream>
#include <istream>


class BMPWriter: public BitmapWriter
{
public:
	Bitmap* loadJPG(const std::string& path) override;
	Bitmap* loadBMP(const std::string& path) override;

	bool save(const Bitmap& image, std::string file_name) override;
private:
	size_t getPixelId(int m_width, int x, int y, int channel) const;
	//bytes
	int padding(int m_width) const;
};
