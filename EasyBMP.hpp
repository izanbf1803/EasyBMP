#include <array>
#include <cassert>
#include <fstream>

namespace EasyBMP 
{
	class Image 
	{
	public:
		~Image();
		Image();
	private:
		void init();
		void setup();

		ofstream outFile;
		string outFileName;
	}

	Image::~Image() 
	{
		outFile.close();
	}

	Image::Image() 
	{
		init();
		setup();
	}

	Image::Image(string _outFileName) 
	{
		init();
		outFileName = _outFileName;
		setup();
	}

	void Image::init()
	{
		outFileName = "image.bmp";
	}

	void Image::setup()
	{
		outFile.open();
	}
}