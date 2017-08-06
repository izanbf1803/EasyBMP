#include "EasyBMP.hpp"
#include <iostream>
#include <random>

using namespace std;

int main()
{
	default_random_engine gen;
	uniform_int_distribution<int> dist(0, 255);

	// sizeX, sizeY, FileName, BackgroundColor
	EasyBMP::Image img(512, 512, "sample.bmp", EasyBMP::RGBColor(255, 255, 0));

	for (int y = 30; y <= 512 - 30; ++y) {
		for (int x = 30; x <= 512 - 30; ++x) {
			// PositionX, PisitionY, Color
			img.SetPixel(x, y, EasyBMP::RGBColor(dist(gen), dist(gen), dist(gen)));
		}
	}

	img.Write();

	return 0;
}