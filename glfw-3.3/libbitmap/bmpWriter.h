#pragma once
#include "bitmapWriter.h"
#include <fstream>
#include <istream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace detail
{
	// -- commons ------------------------------------------------------------------------------------ //

	template < size_t align >
	size_t align_size(size_t s)
	{
		return (align & (align - 1)) ?
			((s + align - 1) / align) * align :  // align is not 2 ^ n
			(s + align - 1) & ~(align - 1);        // align is 2 ^ n
	}

	template < typename T >
	static inline T limit(const T& x, const T& low, const T& hi)
	{
		if (x < low)
			return low;
		else if (x > hi)
			return hi;
		else return x;
	}

	template < typename T >
	inline static void zero_object(T& object)
	{
		memset(&object, 0, sizeof(object));
	}

	template < typename T >
	inline static bool read_object(std::istream& s, T& object, std::streamsize size = sizeof(T))
	{
		return s.read((char*)&object, size).good();
	}

	template < class T >
	static inline bool write_object(std::ostream& s, const T& object, std::streamsize size = sizeof(T))
	{
		return s.write((char*)&object, size).flush().good();
	}


	// -- bitmap types ------------------------------------------------------------------------------- //

	enum mark_t : uint16_t
	{
		markBM = 0x4D42 /*'MB'*/,
		markBA = 0x4142 /*'AB'*/,
		markCI = 0x4943 /*'IC'*/,
		markCP = 0x5043 /*'PC'*/,
		markIC = 0x4349 /*'CI'*/,
		markPT = 0x5450 /*'TP'*/,
	};

	enum color_mode_t : uint32_t
	{
		modeRGB = 0,
		modeRLE8 = 1, modeRLE4 = 2,
		modeBitFields = 3, modeAlphaBitFields = 6,
		modeJPEG = 4, modePNG = 5,
	};

	enum depth_t : uint16_t
	{
		bpp0 = 0, bpp1 = 1, bpp2 = 2, bpp4 = 4, bpp8 = 8,
		bpp16 = 16, bpp24 = 24, bpp32 = 32, bpp48 = 48, bpp64 = 64,
	};

	enum version_t : uint32_t
	{
		core = 12,   // sizeof(BITMAPCOREHEADER)
		info3 = 40,  // sizeof(BITMAPINFOHEADER)
		info31 = 52, // sizeof(BITMAPINFOHEADER)+ sizeof(masks)
		info32 = 56, // sizeof(BITMAPINFOHEADER)+ sizeof(masks) + + sizeof(mask_alpha)
		info4 = 108, // sizeof(BITMAPINFOV4HEADER)
		info5 = 124, // sizeof(BITMAPINFOV4HEADER)
	};

	// -- bitmap size calculations ------------------------------------------------------------------- //


	inline size_t get_line_size(size_t width, size_t bpp)
	{
		return align_size < 8 >(width * bpp) >> 3;
	}

	inline size_t get_stride(size_t width, size_t bpp)
	{
		return align_size < 32 >(width * bpp) >> 3;
	}

	inline size_t get_padding(size_t width, size_t bpp)
	{
		return get_stride(width, bpp) - get_line_size(width, bpp);
	}

	inline size_t get_pixel_array_size(size_t width, size_t height, size_t bpp)
	{
		return get_stride(width, bpp) * height;
	}

	// -- bit fields processing ---------------------------------------------------------------------- //

	class BitField
	{
	private:

		size_t   m_lead_zeroes, m_tail_zeroes, m_width;
		uint32_t m_max_value;

	public:

		BitField(void) { zero_object(*this); }
		~BitField(void) {}

	public: // initializations

		bool init(uint32_t m)
		{
			if (m == 0)
			{
				zero_object(*this);
				return true;
			}

			static const size_t s = sizeof(uint32_t) << 3;

			size_t l = 0, t = 0;
			for (size_t bit = s >> 1; bit > 0; bit >>= 1)
			{
				if (m >> (l | bit))
					l |= bit;
				if (m << (t | bit))
					t |= bit;
			}

			m_lead_zeroes = s - 1 - l;
			m_tail_zeroes = s - 1 - t;
			m_width = s - m_lead_zeroes - m_tail_zeroes;
			m_max_value = (uint32_t(1) << m_width) - 1;

			return m == mask();
		}

	public: // read-only accessors

		inline uint32_t mask(void) const { return m_max_value << m_tail_zeroes; }

	public:

		template < typename T >
		void decode(T value, uint8_t& output) const
		{
			const T v = (value >> m_tail_zeroes)& m_max_value;
			output = v << (sizeof(uint8_t) * 8 - m_width);
		}
		template < typename T >
		void encode(uint8_t input, T& value) const
		{
			const double v = limit(0.5 + input * m_max_value / 255.0, 0.0, double(m_max_value));
			value |= T(v) << m_tail_zeroes;
		}
	};


	// -- bitmap file header class ------------------------------------------------------------------- //

	class BitmapFileHeader // like BITMAPFILEHEADER
	{
	private: // attributes

		enum details : size_t { buffer_size = 12, };

		mark_t m_mark;

		union
		{
			char m_buffer[buffer_size];
			struct
			{
				uint32_t m_length;      // the size, in bytes, of the bitmap file
				uint16_t m_reserved[2]; // reserved, must be zero
				uint32_t m_offset;      // the offset, in bytes, from the beginning of the file to the bitmap data bits.
			};
		};

	public: // constructor

		BitmapFileHeader(size_t o = 0, size_t l = 0)
		{
			clean();
			m_length = int32_t(l);
			m_offset = int32_t(o);
		}

	public: // stream read/write

		inline static size_t stream_size(void) { return sizeof(mark_t) + buffer_size; }

		inline bool read(std::istream& s)
		{
			clean();
			return read_object(s, m_mark) && read_object(s, m_buffer) && validate();
		}

		inline bool write(std::ostream& s) const
		{
			return validate() && write_object(s, m_mark) && write_object(s, m_buffer);
		}

	public: // validation

		inline bool validate(void) const
		{
			return (m_mark == markBM && m_length > 26 && m_offset >= 26 && m_length > m_offset);
		}

	public: // read-only accessors

		inline size_t offset(void) const { return size_t(m_offset); }
		inline size_t length(void) const { return size_t(m_length); }

	private:

		inline void clean(void)
		{
			m_mark = markBM;
			zero_object(m_buffer);
		}
	};


	// -- bitmap info & core headers class ----------------------------------------------------------- //

	struct ColorXYZ { int32_t x, y, z; };

	class BitmapHeader
	{
	private: // attributes

		enum details : size_t
		{
			core_buffer_size = 8, info_buffer_size = 120, palette_length = 256,
			palette_core_entry = 3, palette_info_entry = 4,
		};

	private:

		union
		{
			char m_core_buffer[core_buffer_size];
			struct
			{
				// BITMAPCOREHEADER
				uint16_t width, height, planes;
				depth_t  depth;
			}
			m_core;
			char m_info_buffer[info_buffer_size];
			struct
			{
				// BITMAPINFOHEADER, BITMAPV4INFOHEADER, BITMAPV5INFOHEADER
				int32_t  width, height;
				uint16_t planes;
				depth_t  depth;
				color_mode_t   mode;
				uint32_t image_size;
				int32_t  resolution[2]; // x, y
				uint32_t colors_used, colors_important;

				// BITMAPV4INFOHEADER, BITMAPV5INFOHEADER
				uint32_t mask[3];       // r, g, b, a
				uint32_t mask_alpha;
				uint32_t cs_type;
				ColorXYZ end_point[3];  // r, g, b
				uint32_t gamma[3];      // r, g, b

										// BITMAPV5INFOHEADER
				uint32_t intent, profile_offset, profile_size;
				uint32_t reserved;
			}
			m_info;

		};

	private:

		version_t m_version;
		BitField  m_bit_field[4];
		size_t    m_colors;

	private:

		uint8_t m_palette[palette_length][palette_info_entry];

	public: // constructor

		BitmapHeader(
			version_t v = info3,
			size_t w = 0, size_t h = 0, bool td = false,
			depth_t d = bpp24, color_mode_t m = modeRGB
		)
		{
			clean();

			m_version = v;

			if (core_version())
			{
				m_core.width = uint16_t(w);
				m_core.height = uint16_t(h);
				m_core.planes = 1;
				m_core.depth = d;

				if (m_core.depth <= bpp8)
					m_colors = size_t(1) << m_core.depth;
			}
			else
			{
				m_info.width = int32_t(w);
				m_info.height = td ? -int32_t(h) : int32_t(h);
				m_info.planes = 1;
				m_info.depth = d;
				m_info.mode = m;

				if (m_info.depth == bpp16)
				{
					m_info.mask[0] = 0x00007C00u;    m_bit_field[0].init(m_info.mask[0]);
					m_info.mask[1] = 0x000003E0u;    m_bit_field[1].init(m_info.mask[1]);
					m_info.mask[2] = 0x0000001Fu;    m_bit_field[2].init(m_info.mask[2]);
					m_info.mask_alpha = 0x00000000u; m_bit_field[3].init(m_info.mask_alpha);
				}
				else if (m_info.depth == bpp32)
				{
					m_info.mask[0] = 0x00FF0000u;    m_bit_field[0].init(m_info.mask[0]);
					m_info.mask[1] = 0x0000FF00u;    m_bit_field[1].init(m_info.mask[1]);
					m_info.mask[2] = 0x000000FFu;    m_bit_field[2].init(m_info.mask[2]);
					m_info.mask_alpha = 0xFF000000u; m_bit_field[3].init(m_info.mask_alpha);
				}

				m_info.resolution[0] = m_info.resolution[1] = 11811; // ~300 dpi

				if (m_info.depth <= bpp8) // use palette
				{
					m_info.colors_used = 0;
					m_colors = size_t(1) << m_info.depth;
				}
			}
		}

	public: // stream read/write

		inline size_t stream_size(void) const
		{
			return sizeof(version_t) + (core_version() ? stream_size_core() : stream_size_info());
		}

		inline bool read(std::istream& s)
		{
			clean();

			if (!read_object(s, m_version))
				return false;

			switch (m_version)
			{
			case core:
				return read_core(s);

			case info3:
			case info31:
			case info32:
			case info4:
			case info5:
				return read_info(s);

			default:
				return false;
			}
		}

		inline bool write(std::ostream& s) const
		{
			if (!write_object(s, m_version))
				return false;

			switch (m_version)
			{
			case core:
				return write_core(s);

			case info3:
			case info31:
			case info32:
			case info4:
			case info5:
				return write_info(s);

			default:
				return false;
			}
		}


	public: // validation

		inline bool validate(void) const
		{
			return core_version() ? validate_core() : validate_info();
		}

	public: // read-only accessors

		inline size_t width(void) const
		{
			return core_version() ? size_t(m_core.width) : size_t(std::abs(m_info.width));
		}
		inline size_t height(void) const
		{
			return core_version() ? size_t(m_core.height) : size_t(std::abs(m_info.height));
		}
		inline bool topdown(void) const
		{
			return core_version() ? false : m_info.height < 0;
		}
		inline depth_t depth(void) const
		{
			return core_version() ? m_core.depth : m_info.depth;
		}
		inline color_mode_t mode(void) const
		{
			return core_version() ? modeRGB : m_info.mode;
		}

	public: // bit_field / mask manipulations

		inline bool bit_field(size_t i, uint32_t m)
		{
			if (core_version())
				return false;

			if (m_info.mode == modeRGB || (m_info.depth != bpp16 && m_info.depth != bpp32))
				return false;

			if (!m_bit_field[i % 4].init(m))
				return false;

			m_info.mask[i % 4] = m_bit_field[i % 4].mask();
			return true;
		}

		inline uint32_t bit_field(size_t i) const
		{
			return m_bit_field[i % 4].mask();
		}

	public: // palette / colors manipulations

		inline bool colors(size_t c)
		{
			if (core_version() || m_info.depth > bpp8)
				return false;

			const size_t m = size_t(1) << m_info.depth;
			m_colors = (c != 0 && c < m) ? c : m;
			m_info.colors_used = (m_colors == m) ? 0 : uint32_t(m_colors);
			return true;
		}

		inline size_t colors(void) const { return m_colors; }

		inline uint8_t* palette(size_t i) { return m_palette[i % m_colors]; }
		inline const uint8_t* palette(size_t i) const { return m_palette[i % m_colors]; }

	public: // image size

		inline bool image_size(size_t s)
		{
			if (core_version() || !use_image_size_info() || s == 0)
				return false;

			m_info.image_size = uint32_t(s);
			return true;
		}

		inline size_t image_size(void) const
		{
			return core_version() || m_info.image_size == 0 ?
				get_pixel_array_size(width(), height(), depth()) : size_t(m_info.image_size);
		}

	public: // helpers

		inline bool use_palette(void) const
		{
			return (depth() <= bpp8);
		}
		inline bool use_bit_fields(void) const
		{
			return (depth() == bpp16 || depth() == bpp32);
		}

	private:

		inline bool core_version(void) const
		{
			return (m_version == core);
		}
		inline bool use_bit_fields_info(void) const
		{
			return (m_info.depth == bpp16 || m_info.depth == bpp32);
		}
		inline bool use_image_size_info(void) const
		{
			return (mode() == modeRLE4 || mode() == modeRLE8 || mode() == modeJPEG || mode() == modePNG);
		}

		inline void clean(void) { zero_object(*this); }

	private: // validation

		inline bool validate_core(void) const
		{
			if (m_core.planes != 1)
				return false; // planes must be 1

			if (m_core.width == 0 || m_core.height == 0)
				return false; // invalid image dimentions

			if (m_core.depth != bpp1 && m_core.depth != bpp2
				&& m_core.depth != bpp4 && m_core.depth != bpp8 && m_core.depth != bpp24)
				return false; // unsupported depth

			return true;
		}

		inline bool validate_info(void) const
		{
			if (m_info.planes != 1)
				return false; // planes must be 1

			if (m_info.width <= 0 || m_info.height == 0)
				return false; // invalid image dimentions

			if (m_info.depth != bpp0
				&& m_info.depth != bpp1 && m_info.depth != bpp2
				&& m_info.depth != bpp4 && m_info.depth != bpp8
				&& m_info.depth != bpp16 && m_info.depth != bpp24 && m_info.depth != bpp32
				&& m_info.depth != bpp48 && m_info.depth != bpp64)
				return false; // unsupported depth

			if (m_info.depth <= bpp8 && m_info.colors_used > (uint32_t(1) << m_info.depth))
				return false; // invalid colors_used field

			switch (m_info.mode)
			{
			case modeRGB:            return (m_info.depth != bpp0);
			case modeRLE8:           return (m_info.depth == bpp8 && m_info.height > 0 && m_info.image_size != 0);
			case modeRLE4:           return (m_info.depth == bpp4 && m_info.height > 0 && m_info.image_size != 0);
			case modeBitFields:
			case modeAlphaBitFields: return (m_info.depth == bpp16 || m_info.depth == bpp32);
			case modePNG:
			case modeJPEG:           return (m_info.depth == bpp0 && m_info.height < 0 && m_info.image_size != 0);

			default:
				return false; // unsupported mode
			}
		}

	private: // stream read/write

		inline size_t stream_size_info(void) const
		{
			size_t size = size_t(m_version) - sizeof(m_version);

			if (m_version == info3 && (m_info.mode == modeBitFields || m_info.mode == modeAlphaBitFields))
				size += sizeof(m_info.mask);

			if ((m_version == info3 || m_version == info31) && m_info.mode == modeAlphaBitFields)
				size += sizeof(m_info.mask_alpha);

			if (m_info.depth <= bpp8) // use palette
				size += m_colors * palette_info_entry;

			return size;
		}

		inline size_t stream_size_core(void) const
		{
			size_t size = size_t(m_version) - sizeof(m_version);

			if (m_core.depth <= bpp8)
				size += m_colors * palette_core_entry;

			return size;
		}

		inline bool read_info(std::istream& s)
		{
			const std::streamsize size = std::streamsize(size_t(m_version) - sizeof(m_version));

			if (!read_object(s, m_info_buffer, size))
				return false;

			if (!validate_info())
				return false;

			if (m_info.depth == bpp16)
			{
				if (m_version < info31)
				{
					m_info.mask[0] = 0x00007C00u;
					m_info.mask[1] = 0x000003E0u;
					m_info.mask[2] = 0x0000001Fu;
				}
				if (m_version < info32)
					m_info.mask_alpha = 0x00000000u;
			}
			else if (m_info.depth == bpp32)
			{
				if (m_version < info31)
				{
					m_info.mask[0] = 0x00FF0000u;
					m_info.mask[1] = 0x0000FF00u;
					m_info.mask[2] = 0x000000FFu;
				}
				if (m_version < info32)
					m_info.mask_alpha = 0xFF000000u;
			}

			if (m_version == info3 && (m_info.mode == modeBitFields || m_info.mode == modeAlphaBitFields))
				if (!read_object(s, m_info.mask))
					return false;

			if ((m_version == info3 || m_version == info31) && m_info.mode == modeAlphaBitFields)
				if (!read_object(s, m_info.mask_alpha))
					return false;

			if (m_info.depth == bpp16 || m_info.depth == bpp32)
			{
				m_bit_field[0].init(m_info.mask[0]);
				m_bit_field[1].init(m_info.mask[1]);
				m_bit_field[2].init(m_info.mask[2]);
				m_bit_field[3].init(m_info.mask_alpha);
			}

			if (m_info.depth <= bpp8) // use palette
			{
				const size_t m = size_t(1) << m_info.depth;
				m_colors = m_info.colors_used && size_t(m_info.colors_used) < m ?
					size_t(m_info.colors_used) : m;

				for (size_t i = 0; i < m_colors; ++i)
					if (!read_object(s, m_palette[i], palette_info_entry))
						return false;
			}

			return true;
		}

		inline bool read_core(std::istream& s)
		{
			const std::streamsize size = std::streamsize(size_t(m_version) - sizeof(m_version));

			if (!read_object(s, m_core_buffer, size))
				return false;

			if (!validate_core())
				return false;

			if (m_core.depth <= bpp8)
			{
				m_colors = size_t(1) << m_core.depth;

				for (size_t i = 0; i < m_colors; ++i)
					if (!read_object(s, m_palette[i]))
						return false;
			}

			return true;
		}

		inline bool write_info(std::ostream& s) const
		{
			if (!validate_info())
				return false;

			if (m_info.depth <= bpp8) // use palette
			{
				if (m_info.colors_used == 0 && m_colors != size_t(1) << m_info.depth)
					return false; // m_colors_used & m_colors are not syncronized

				if (m_info.colors_used != 0 && m_colors != size_t(m_info.colors_used))
					return false; // m_colors_used & m_colors are not syncronized
			}

			if (m_info.depth == bpp16 || m_info.depth == bpp32)
			{
				if (m_info.mask[0] != m_bit_field[0].mask()
					|| m_info.mask[1] != m_bit_field[1].mask()
					|| m_info.mask[2] != m_bit_field[2].mask()
					|| m_info.mask_alpha != m_bit_field[3].mask())
					return false; // m_masks & m_bit_filds are not syncronized
			}

			const std::streamsize size = std::streamsize(size_t(m_version) - sizeof(m_version));

			if (!write_object(s, m_info_buffer, size))
				return false;

			if (m_version < info31 && (m_info.mode == modeBitFields || m_info.mode == modeAlphaBitFields))
				if (!write_object(s, m_info.mask))
					return false;

			if (m_version < info32 && m_info.mode == modeAlphaBitFields)
				if (!write_object(s, m_info.mask_alpha))
					return false;

			if (m_info.depth <= bpp8) // use palette
			{
				for (size_t i = 0; i < m_colors; ++i)
					if (!write_object(s, m_palette[i], palette_info_entry))
						return false;
			}

			return true;
		}

		inline bool write_core(std::ostream& s) const
		{
			if (!validate_core())
				return false;

			if (m_core.depth <= bpp8)
			{
				if (m_colors != (size_t(1) << m_core.depth))
					return false; // m_colors & m_depth are not syncronized
			}

			const std::streamsize size = std::streamsize(size_t(m_version) - sizeof(m_version));

			if (!write_object(s, m_core_buffer, size))
				return false;

			if (m_core.depth <= bpp8)
			{
				for (size_t i = 0; i < m_colors; ++i)
					if (!write_object(s, m_palette[i], palette_core_entry))
						return false;
			}

			return true;
		}

	};

}
class BMPWriter: public BitmapWriter
{
public:
	Bitmap* loadJPG(const std::string& path) override
	{
		int width, height, bpp;
		uint8_t* rgb_image = stbi_load(path.c_str(), &width, &height, &bpp, 3);
		//swap channels to fit bitmap
		for (int i = 0; i < width * height;i++)
			std::swap(rgb_image[i * 3], rgb_image[i * 3 + 2]);
		
		Bitmap* bmp = new Bitmap();
		bmp->m_width = width;
		bmp->m_height = height;
		bmp->m_buffer = rgb_image;
		
		return  bmp;
	}
	Bitmap* loadBMP(const std::string& path) override
	{
		using namespace detail;

		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (!file.good())
			throw -1;

		if (!file.seekg(0, std::ios_base::beg).good())
			throw - 1;

		BitmapFileHeader file_header;

		if (!file_header.read(file))
			throw - 1;

		BitmapHeader header;

		if (!header.read(file))
			throw - 1;

		const depth_t internal_depth = bpp24;
		const size_t  width = header.width();
		const size_t  height = header.height();
		const bool    topdown = header.topdown();
		const color_mode_t  mode = header.mode();
		const depth_t depth = header.depth();


		const size_t internal_size = get_pixel_array_size(width, height, internal_depth);

		std::unique_ptr <uint8_t> buffer = std::unique_ptr<uint8_t>(new uint8_t[internal_size]);

		const ptrdiff_t internal_stride = topdown ?
			get_stride(width, internal_depth) :
			-ptrdiff_t(get_stride(width, internal_depth));
		uint8_t* const start = buffer.get() + (topdown ? 0 : internal_size + internal_stride);

		uint32_t dummy;

		if (mode == modeRGB)
		{
			if (depth == bpp24)
			{
				const std::streamsize padding = std::streamsize(get_padding(width, depth));
				const std::streamsize line_size = std::streamsize(get_line_size(width, depth));

				for (size_t y = 0; y < height; ++y)
				{
					if (!read_object(file, start[y * internal_stride], line_size))
						throw -1;
					if (padding && !read_object(file, dummy, padding))
						throw -1;
				}

				uint8_t* data = buffer.release();
				Bitmap* image = new Bitmap();
				image->m_buffer = new uint8_t[width * height * 3];
				image->m_width = width;
				image->m_height = height;
				
				for(size_t i=0;i<width;++i)
					for(size_t j=0;j<height;++j)
					{
						size_t id = getPixelId(width, i, j ,0);
						image->m_buffer[width * j * 3 + i * 3] = data[id];
						image->m_buffer[width * j * 3 + i * 3+1] = data[id+1];
						image->m_buffer[width * j * 3 + i * 3+2] = data[id+2];
					}
				return image;
			}

		}
		
		
		throw - 1;
	}

	bool save(const Bitmap& image, std::string file_name) override
	{
		using namespace detail;

		std::ofstream file(file_name, std::ios::out | std::ios::binary);

		if (!file.good())
			return false;

		if (!file.seekp(0, std::ios_base::beg).good())
			return false;

		BitmapHeader header(core, image.m_width, image.m_height);

		size_t offset = BitmapFileHeader::stream_size() + header.stream_size();

		BitmapFileHeader file_header(offset, offset + header.image_size());

		if (!file_header.write(file))
			return false;

		if (!header.write(file))
			return false;
		uint8_t* data = new uint8_t[image.m_width * image.m_height * 3 + image.m_height * padding(image.m_width)];
		for (size_t i = 0; i < image.m_width; ++i)
			for (size_t j = 0; j < image.m_height; ++j)
			{
				size_t id = getPixelId(image.m_width, i, j, 0);
				data[id]   = image.getPixelChannel(i, j, 0);
				data[id+1] = image.getPixelChannel(i, j, 1);
				data[id+2] = image.getPixelChannel(i, j, 2);
			}
		const bool topdown = header.topdown();
		const uint32_t dummy = 0;

		const size_t buffer_size = get_pixel_array_size(image.m_width, image.m_height, 24);
		const ptrdiff_t buffer_stride = topdown ? get_stride(image.m_width, 24) : -ptrdiff_t(get_stride(image.m_width, 24));
		uint8_t* const buffer_start = topdown ? data : data + buffer_size + buffer_stride;

		const std::streamsize padding = std::streamsize(get_padding(image.m_width, 24));
		const std::streamsize line_size = std::streamsize(get_line_size(image.m_width, 24));

		for (size_t y = 0; y < image.m_height; ++y)
		{
			if (!write_object(file, buffer_start[y * buffer_stride], line_size))
				return false;
			if (padding && !write_object(file, dummy, padding))
				return false;
		}



		return true;
	}
private:
	size_t getPixelId(int m_width, int x, int y, int channel) const
	{
		return y * m_width * 3 + y * padding(m_width) + 3 * x + channel;
	}
	//bytes
	int padding(int m_width) const
	{
		const int size = int(ceil(24.0f * m_width / 32)) * 4;
		return size - m_width * 3 * sizeof(uint8_t);
	}
};
