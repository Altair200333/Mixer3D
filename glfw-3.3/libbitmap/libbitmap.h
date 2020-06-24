#ifndef ___LIB_BITMAP_H___
#define ___LIB_BITMAP_H___

#include <cstdint>
#include <string>

class Bitmap
{
public:
	uint8_t * m_buffer;
	size_t    m_width, m_height;

	Bitmap() : m_buffer(nullptr), m_width(0), m_height(0)
	{}
	virtual ~Bitmap()
	{
		clear();
	}

	uint8_t getPixelChannel(int x, int y, int channel) const;
	size_t getPixelId(int x, int y, int channel) const;

	void clear(void)
	{
		m_width = m_height = 0;
		delete [] m_buffer;
		m_buffer = nullptr;
	}

private:
	Bitmap & operator = (const Bitmap &) = delete;
};

#endif // ___LIB_BITMAP_H___
