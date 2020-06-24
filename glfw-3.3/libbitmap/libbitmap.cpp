
#include "libbitmap.h"
#include <algorithm>


size_t Bitmap::getPixelId(int x, int y, int channel) const
{	
	return y * m_width * 3+ 3 * x + channel;
}
uint8_t Bitmap::getPixelChannel(int x, int y, int channel) const
{
	if (x < 0 || x >= m_width || y<0 || y>=m_height)
		return m_buffer[
			getPixelId(std::clamp(x, 0, int(m_width)-1),
				std::clamp(y, 0, int(m_height) - 1), channel)];
	
	else
		return m_buffer[getPixelId(x,y,channel)];
}
