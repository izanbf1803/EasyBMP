#include "EasyBMP.hpp"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct point
{
  double x, y;
  point(double _x, double _y) : x(_x), y(_y) { }
  double distanceTo(const point& o) {
    double dx = (x - o.x);
    double dy = (y - o.y);
    return sqrt(dx * dx + dy * dy);
  }
};

int main()
{
  vector< point > centers = {
    {256, 256},
    {192, 192}
  };
  vector< double > charges = {1024.0, 2048.0};
	// sizeX, sizeY, FileName, BackgroundColor
  EasyBMP::RGBColor black(0, 0, 0);
	EasyBMP::Image img(512, 512, "sample.bmp", black);

  double diagonal_distance = point(0., 0.).distanceTo(point(512., 512.));

	for (int y = 0; y < 512; ++y) {
		for (int x = 0; x < 512; ++x) {
      double intensity = 0.0;
      for(int i = 0; i < 2; ++i) {
        double dist = max(centers[i].distanceTo(point(x, y)), 0.001);
        intensity += charges[i] / (dist * dist);
      }
      int final_color = min(255, int(255. * intensity));
			// PositionX, PisitionY, Color
			img.SetPixel(x, y, EasyBMP::RGBColor(final_color, final_color, 0));
		}
	}

	img.Write();

	return 0;
}
