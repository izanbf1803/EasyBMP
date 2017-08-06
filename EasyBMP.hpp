#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace EasyBMP 
{
	static const int BMP_FILE_HEADER_SIZE = 14;
	static const int BMP_INFO_HEADER_SIZE = 40;


	class RGBColor 
	{
	public:
		RGBColor() {}; // Don't use void constructor, just for variable definition
		~RGBColor() {};
		RGBColor(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b);

		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
	};

	RGBColor::RGBColor(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b) 
	{
		r = _r;
		g = _g; 
		b = _b;
	} 


	class Image 
	{
	public:
		~Image();
		Image() { width = height = 0; }; // Don't use default constructor, just for variable definition
		Image(std::uint32_t _width, std::uint32_t _height);
		Image(std::uint32_t _width, std::uint32_t _height, std::string _outFileName);
		Image(std::uint32_t _width, std::uint32_t _height, RGBColor _backgroundColor);
		Image(std::uint32_t _width, std::uint32_t _height, std::string _outFileName, RGBColor _backgroundColor);

	private:
		void Init(std::uint32_t _width, std::uint32_t _height);
		void Setup();

		std::uint32_t width;
		std::uint32_t height;
		std::ofstream outFile;
		std::string outFileName;
		RGBColor backgroundColor;
		RGBColor** buffer;
	};

	// Free and close used classes
	Image::~Image() 
	{
		if (outFile.is_open()) {
			outFile.close();
		}
		if (buffer != NULL) {
			for (std::uint32_t i = 0; i < height; ++i) {
				delete[] buffer[i];
			}
			delete[] buffer;
		}
	}

	Image::Image(std::uint32_t _width, std::uint32_t _height) 
	{
		Init(_width, _height);
		Setup();
	}

	Image::Image(std::uint32_t _width, std::uint32_t _height, std::string _outFileName) 
	{
		Init(_width, _height);
		outFileName = _outFileName;
		Setup();
	}

	Image::Image(std::uint32_t _width, std::uint32_t _height, RGBColor _backgroundColor) 
	{
		Init(_width, _height);
		backgroundColor = _backgroundColor;
		Setup();
	}

	Image::Image(std::uint32_t _width, std::uint32_t _height, std::string _outFileName, RGBColor _backgroundColor) 
	{
		Init(_width, _height);
		outFileName = _outFileName;
		backgroundColor = _backgroundColor;
		Setup();
	}

	// Load constant values
	void Image::Init(std::uint32_t _width, std::uint32_t _height)
	{
		width = _width;
		height = _height;
		buffer = NULL;
		outFileName = "image.bmp";
		backgroundColor = RGBColor(255, 255, 255);
	}

	// Setup all classes before start using
	void Image::Setup()
	{
		try {
			buffer = new RGBColor*[height];
			for (std::uint32_t i = 0; i < height; ++i) {
				buffer[i] = new RGBColor[width];
			}
		}
		catch (std::bad_alloc& ba) {
			std::cerr << "EasyBMP ERROR: bad_alloc error (Can't create image buffer) -> " << ba.what() << std::endl;
			assert(false);
		}
		std::cout << "Alloc sucess" << std::endl;
	}
}