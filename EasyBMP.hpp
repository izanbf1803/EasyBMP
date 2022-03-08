#ifndef EasyBMP_H_
#define EasyBMP_H_

/*
Original repository: https://github.com/izanbf1803/EasyBMP
License: MIT
*/
#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>

namespace EasyBMP 
{
    using std::string;
    using std::int64_t;
    using std::uint32_t;
    using std::uint8_t;
    using std::ofstream;
    using std::ifstream;
    using std::stringstream;


    class RGBColor 
    {
    public:
        RGBColor() {}; // Don't use void constructor, just for variable definition
        ~RGBColor() {};
        RGBColor(uint8_t _r, uint8_t _g, uint8_t _b);
        void SetColor(uint8_t _r, uint8_t _g, uint8_t _b);

        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    RGBColor::RGBColor(uint8_t _r, uint8_t _g, uint8_t _b) 
    {
        SetColor(_r, _g, _b);
    } 

    void RGBColor::SetColor(uint8_t _r, uint8_t _g, uint8_t _b) 
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
        Image(const string& _inFileName);
        Image(const string& _inFileName, const string& _outFileName);
        Image(int64_t _width, int64_t _height);
        Image(int64_t _width, int64_t _height, const string& _outFileName);
        Image(int64_t _width, int64_t _height, const RGBColor& _backgroundColor);
        Image(int64_t _width, int64_t _height, const string& _outFileName, const RGBColor& _backgroundColor);
        void SetPixel(int64_t x, int64_t y, const RGBColor& color, bool ignore_err);
        const RGBColor& GetPixel(int64_t x, int64_t y) const;
        void DrawLine(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color);
        void DrawCircle(int64_t x0, int64_t y0, int64_t r, const RGBColor& color, bool fill);
        void SetFileName(const string& _outFileName);
        void Write(const string& _outFileName);
        void Write();
        inline int64_t w() const { return width; }
        inline int64_t h() const { return height; }
        inline bool isValidCoordinate(int64_t x, int64_t y) const {
            return x >= 0 and y >= 0 and x < width and y < height;
        }

    private:
        void Init(int64_t _width, int64_t _height);
        void Setup();
        void FillBG();
        void DrawLineLow(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color);
        void DrawLineHigh(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color);
        void OpenSetup(const string& _inFileName);

        int64_t width;
        int64_t height;
        ofstream outFile;
        string outFileName;
        RGBColor backgroundColor;
        RGBColor** buffer;
    };

    // Free and close used classes
    Image::~Image() 
    {
        if (buffer != NULL) {
            for (int64_t i = 0; i < height; ++i) {
                delete[] buffer[i];
            }
            delete[] buffer;
            buffer = NULL;
        }
    }

    Image::Image(const string& _inFileName) {
        OpenSetup(_inFileName);
    }

    Image::Image(const string& _inFileName, const string& _outFileName) {
        OpenSetup(_inFileName);
        SetFileName(_outFileName);
    }

    Image::Image(int64_t _width, int64_t _height) 
    {
        Init(_width, _height);
        Setup();
        FillBG();
    }

    Image::Image(int64_t _width, int64_t _height, const string& _outFileName)
    {
        Init(_width, _height);
        SetFileName(_outFileName);
        Setup();
        FillBG();
    }

    Image::Image(int64_t _width, int64_t _height, const RGBColor& _backgroundColor) 
    {
        Init(_width, _height);
        backgroundColor = _backgroundColor;
        Setup();
        FillBG();
    }

    Image::Image(int64_t _width, int64_t _height, const string& _outFileName, const RGBColor& _backgroundColor) 
    {
        Init(_width, _height);
        SetFileName(_outFileName);
        backgroundColor = _backgroundColor;
        Setup();
        FillBG();
    }

    // Load constant values
    void Image::Init(int64_t _width, int64_t _height)
    {
        if(_width <= 0 || _height <= 0)
            throw std::invalid_argument("EasyBMP ERROR: Image width and heigth must be greater than zero");
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
            for (int64_t y = 0; y < height; ++y) {
                buffer[y] = new RGBColor[width];
            }
        }
        catch (std::bad_alloc& ba) {
            std::stringstream ss;
            ss << "EasyBMP ERROR: bad_alloc error (Can't create image buffer) -> " << ba.what();
            throw std::runtime_error(ss.str());
        }
    }

    void Image::FillBG() {
        for (int64_t y = 0; y < height; ++y) {
            for (int64_t x = 0; x < width; ++x) {
                buffer[y][x] = backgroundColor;
            }
        }
    }

    void Image::SetPixel(int64_t x, int64_t y, const RGBColor& color, bool ignore_err=false)
    {
        if (!isValidCoordinate(x, y)) {
            if(!ignore_err)
                throw std::out_of_range("EasyBMP ERROR: pixel coordinate is out of range.");
        } else {
            buffer[y][x] = color;
        }
    }

    const RGBColor& Image::GetPixel(int64_t x, int64_t y) const
    {
        if (!isValidCoordinate(x, y))
            throw std::out_of_range("EasyBMP ERROR: pixel coordinate is out of range.");
        return buffer[y][x];
    }

    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    void Image::DrawLine(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color)
    {   
        if (abs(y1 - y0) < abs(x1 - x0)) {
            if (x0 > x1) {
                DrawLineLow(x1, y1, x0, y0, color);
            }
            else {
                DrawLineLow(x0, y0, x1, y1, color);
            }
        }
        else {
            if (y0 > y1) {
                DrawLineHigh(x1, y1, x0, y0, color);
            }
            else {
                DrawLineHigh(x0, y0, x1, y1, color);
            }
        }
    }

    void Image::DrawLineLow(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color)
    {
        int64_t dx = x1 - x0;
        int64_t dy = y1 - y0;
        int64_t yi = 1;
        if (dy < 0) {
            yi = -1;
            dy = -dy;
        }
        int64_t D = (dy << 1) - dx;
        int64_t y = y0;
        for (int64_t x = x0; x <= x1; ++x) {
            SetPixel(x, y, color, true);
            if (D > 0) {
                y += yi;
                D -= (dx << 1);
            }
            D += (dy << 1);
        }
    }

    void Image::DrawLineHigh(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const RGBColor& color)
    {
        int64_t dx = x1 - x0;
        int64_t dy = y1 - y0;
        int64_t xi = 1;
        if (dx < 0) {
            xi = -1;
            dx = -dx;
        }
        int64_t D = (dx << 1) - dy;
        int64_t x = x0;
        for (int64_t y = y0; y <= y1; ++y) {
            SetPixel(x, y, color, true);
            if (D > 0) {
                x += xi;
                D -= (dy << 1);
            }
            D += (dx << 1);
        }
    }

    void Image::DrawCircle(int64_t x0, int64_t y0, int64_t r, const RGBColor& color, bool fill = false)
    {
        if(x0 < 0 and y0 < 0 and x0 >= width and y0 >= height)
            throw std::out_of_range("EasyBMP ERROR: circle center coordinate is out of range.");
    
        if (fill) {
            int64_t sq_r = r*r;
            for (int64_t dx = -r; dx <= r; ++dx) {
                for (int64_t dy = -r; dy <= r; ++dy) {
                    int64_t sq_dist = dx*dx + dy*dy;
                    if (sq_dist <= sq_r) {
                        int64_t x = x0 + dx;
                        int64_t y = y0 + dy;
                        SetPixel(x, y, color, true);
                    }
                }
            }
        }
        else {
            // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
            int64_t x = r - 1;
            int64_t y = 0;
            int64_t dx = 1;
            int64_t dy = 1;
            int64_t err = dx - (r << 1);

            while (x >= y) {
                SetPixel(x0 + x, y0 + y, color, true);
                SetPixel(x0 + y, y0 + x, color, true);
                SetPixel(x0 - y, y0 + x, color, true);
                SetPixel(x0 - x, y0 + y, color, true);
                SetPixel(x0 - x, y0 - y, color, true);
                SetPixel(x0 - y, y0 - x, color, true);
                SetPixel(x0 + y, y0 - x, color, true);
                SetPixel(x0 + x, y0 - y, color, true);

                if (err <= 0) {
                    y++;
                    err += dy;
                    dy += 2;
                }
                
                if (err > 0) {
                    x--;
                    dx += 2;
                    err += dx - (r << 1);
                }
            }
        }
    }

    void Image::SetFileName(const string& _outFileName) 
    {
        if(_outFileName.size() == 0)
            throw std::out_of_range("EasyBMP ERROR: File name must contain at least one character.");
        outFileName = _outFileName;
    }

    void Image::Write(const string& _outFileName) 
    {
        SetFileName(_outFileName);
        Write();
    }

    // BMP headers code: https://en.wikipedia.org/wiki/User:Evercat/Buddhabrot.c
    void Image::Write()
    {

        outFile.open(outFileName, ofstream::binary);
        if (not outFile.is_open()) {
            throw std::runtime_error("EasyBMP ERROR: Can't open file to write data.");
        }

        unsigned int headers[13];
        int paddedSize, extraBytes;
        
        extraBytes = 4 - ((width * 3) % 4);         // How many bytes of padding to add to each
                                                    // horizontal line - the size of which must
                                                    // be a multiple of 4 bytes.

        if (extraBytes == 4) {
            extraBytes = 0;
        }

        paddedSize = ((width * 3) + extraBytes) * height;

        // Headers...
        // Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".
                             
        headers[0]  = paddedSize + 54;      // bfSize (whole file size)
        headers[1]  = 0;                    // bfReserved (both)
        headers[2]  = 54;                   // bfOffbits
        headers[3]  = 40;                   // biSize
        headers[4]  = width;  // biWidth
        headers[5]  = height; // biHeight

        // Would have biPlanes and biBitCount in position 6, but they're shorts.
        // It's easier to write them out separately (see below) than pretend
        // they're a single int, especially with endian issues...

        headers[7]  = 0;                    // biCompression
        headers[8]  = paddedSize;           // biSizeImage
        headers[9]  = 0;                    // biXPelsPerMeter
        headers[10] = 0;                    // biYPelsPerMeter
        headers[11] = 0;                    // biClrUsed
        headers[12] = 0;                    // biClrImportant

        //
        // Headers begin...
        // When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
        //

        outFile << 'B' << 'M';

        for (int i = 0; i <= 5; ++i) {
            outFile << (uint8_t)((headers[i] & 0x000000ff));
            outFile << (uint8_t)((headers[i] & 0x0000ff00) >> 8);
            outFile << (uint8_t)((headers[i] & 0x00ff0000) >> 16);
            outFile << (uint8_t)((headers[i] & (uint32_t)0xff000000) >> 24);
        }

        // These next 4 characters are for the biPlanes and biBitCount fields.

        outFile << (uint8_t)1 << (uint8_t)0 << (uint8_t)24 << (uint8_t)0;

        for (int i = 7; i <= 12; ++i) { 
            outFile << (uint8_t)((headers[i] & 0x000000ff));
            outFile << (uint8_t)((headers[i] & 0x0000ff00) >> 8);
            outFile << (uint8_t)((headers[i] & 0x00ff0000) >> 16);
            outFile << (uint8_t)((headers[i] & (uint32_t)0xff000000) >> 24);
        }

        //
        // Headers done, now write the data...
        //

        // BMP image format is written from bottom to top...
        for (int64_t y = height - 1; y >= 0; --y) {
            for (int64_t x = 0; x < width; ++x) {
                // Also, it's written in (b,g,r) format...
                outFile << buffer[y][x].b << buffer[y][x].g << buffer[y][x].r;
            }
            // See above - BMP lines must be of lengths divisible by 4.
            if (extraBytes) {
                for (int i = 1; i <= extraBytes; ++i) {
                    outFile << (uint8_t)0;
                }
            }
        }
        
        outFile.close();
    }

    // Reverse of Write()
    void Image::OpenSetup(const string& _inFileName) {
        ifstream inFile;
        inFile.open(_inFileName, ifstream::binary);

        if(!inFile.is_open()) {
            throw std::runtime_error("EasyBMP ERROR: Can't open file to read data.");
        }

        // Get image dimensions
        uint8_t imgInfo[54];
        inFile.read((char*)imgInfo, 54);
        width  = *(uint32_t*)&imgInfo[18];
        height = *(uint32_t*)&imgInfo[22];

        uint16_t bytesPerPixel = *(uint16_t*)&imgInfo[28] / 8;
        uint32_t compression = *(uint32_t*)&imgInfo[30];

        if(compression != 0) {
            throw std::runtime_error("EasyBMP ERROR: Bitmap compression not supported.");
        }

        if(bytesPerPixel != 3) {
            throw std::runtime_error("EasyBMP ERROR: Bytes per pixel of bitmap must be 3.");
        }
            

        // Get pixel data offset
        uint32_t offset = *(uint32_t*)&imgInfo[10];

        // Get row padding
        int extraBytes = 4 - ((width * 3) % 4);
        if (extraBytes == 4) extraBytes = 0;

        // Create image buffer
        Setup();

        // Ignore pixel data offset
        inFile.ignore(offset-54);

        // Put image pixels into buffer
        for(int64_t y = height - 1; y >= 0; --y) {
            for(int64_t x = 0; x < width; ++x) {
                uint8_t b = inFile.get();
                uint8_t g = inFile.get();
                uint8_t r = inFile.get();
                RGBColor pixelColor(r, g, b);
                buffer[y][x] = pixelColor;
            }
            if (extraBytes) { // BMP padding
                inFile.ignore(extraBytes);
            }
        }

        inFile.close();
    }
}

#endif
