#ifndef STB_IMAGE_WRAPPER_INCLUDE
#define STB_IMAGE_WRAPPER_INCLUDE


#include <iostream>
#include <exception>
#include <string>
#include <assert.h>
#include <fstream>

extern "C" {
    #include "stb/stb_image.h"
    #include "stb/stb_image_resize2.h"
    #include "stb/stb_image_write.h"
}

#define TODO(message) static_assert(0 && message);


class Image {
    uint8_t *data = nullptr;
    public:
    typedef enum {
        NONE    = 0,
        LOCAL   = 1,
        STB     = 2,
    } MemoryOwner;
    MemoryOwner owner = NONE;
    
    int width, height, channels;
    
    Image(const char* filepath, int desired_number_of_channels=0);
    Image(const std::string &filepath, int desired_number_of_channels=0);
    Image(int width, int height, int channels);
    
    ~Image();
    
    Image(const Image &other);
    Image(Image &&other);
    
    Image& operator=(const Image &other);
    Image& operator=(Image &&other);
    
    int save_jpg(const char* filepath, int quality=100);
    int save_jpg(const std::string &filepath, int quality=100);
    
    int save_png(const char* filepath);
    int save_png(const std::string &filepath);
    
    uint8_t* at(int x, int y);
    const uint8_t* at(int x, int y) const;
};

struct PixelGray {
    uint8_t value;
    PixelGray(uint8_t value);

    PixelGray operator*(float k);
    PixelGray operator/(float k);

    PixelGray operator+(const PixelGray& other);
    PixelGray operator-(const PixelGray& other);

    PixelGray& operator+=(const PixelGray& other);
    PixelGray& operator-=(const PixelGray& other);
};
PixelGray operator*(float k, PixelGray& pix);
std::ostream& operator<<(std::ostream& os, const PixelGray& p);

struct PixelRGB {
    uint8_t r, g, b;
    PixelRGB(uint8_t r, uint8_t g, uint8_t b);

    PixelRGB operator*(float k);
    PixelRGB operator/(float k);

    PixelRGB operator+(const PixelRGB& other);
    PixelRGB operator-(const PixelRGB& other);

    PixelRGB& operator+=(const PixelRGB& other);
    PixelRGB& operator-=(const PixelRGB& other);
};
PixelRGB operator*(float k, PixelRGB& pix);
std::ostream& operator<<(std::ostream& os, const PixelRGB& p);

struct PixelRGBA: public PixelRGB {
    uint8_t a;
    
    PixelRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    PixelRGBA operator*(float k);
    PixelRGBA operator/(float k);

    PixelRGBA operator+(const PixelRGBA& other);
    PixelRGBA operator-(const PixelRGBA& other);

    PixelRGBA& operator+=(const PixelRGBA& other);
    PixelRGBA& operator-=(const PixelRGBA& other);
};
PixelRGBA operator*(float k, PixelRGBA& pix);
std::ostream& operator<<(std::ostream& os, const PixelRGBA& p);

struct ColorRGBA;
struct ColorYCbCrA; 

struct ColorRGBA {
    double r, g, b;
    double a = 1;

    ColorRGBA();
    ColorRGBA(double r, double g, double b);
    ColorRGBA(double r, double g, double b, double a);
    ColorRGBA(const PixelGray& p);
    ColorRGBA(const PixelRGB& rgb);
    ColorRGBA(const PixelRGBA& rgba);
    ColorRGBA(const ColorYCbCrA& ycbcra);

    ColorRGBA operator*(double k);
    ColorRGBA operator/(double k);

    ColorRGBA operator+(const ColorRGBA& other);
    ColorRGBA operator-(const ColorRGBA& other);

    ColorRGBA& operator*=(double k);
    ColorRGBA& operator+=(const ColorRGBA& other);
    ColorRGBA& operator-=(const ColorRGBA& other);

    explicit operator PixelGray() const;
    explicit operator PixelRGB() const;
    explicit operator PixelRGBA() const;

    explicit operator ColorYCbCrA() const;
};
ColorRGBA operator*(double k, const ColorRGBA& pix);
std::ostream& operator<<(std::ostream& os, const ColorRGBA& p);

struct ColorYCbCrA {
    // Kr & Kb MAY! be edited, and it will affect forward and backward conversions
    static double Kr; 
    static double Kb;

    double y, cb, cr;
    double a = 1;

    ColorYCbCrA();
    ColorYCbCrA(double y, double cb, double cr);
    ColorYCbCrA(double y, double cb, double cr, double a);
    ColorYCbCrA(const ColorRGBA& clr);

    ColorYCbCrA operator*(double k);
    ColorYCbCrA operator/(double k);

    ColorYCbCrA operator+(const ColorYCbCrA& other);
    ColorYCbCrA operator-(const ColorYCbCrA& other);

    ColorYCbCrA& operator*=(double k);
    ColorYCbCrA& operator+=(const ColorYCbCrA& other);
    ColorYCbCrA& operator-=(const ColorYCbCrA& other);

    explicit operator ColorRGBA() const;
};
ColorYCbCrA operator*(double k, const ColorYCbCrA& pix);
std::ostream& operator<<(std::ostream& os, const ColorYCbCrA& p);

#endif //STB_IMAGE_WRAPPER_INCLUDE















#ifdef STB_IMAGE_WRAPPER_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
extern "C" {
    #include "stb/stb_image.h"
    #include "stb/stb_image_resize2.h"
    #include "stb/stb_image_write.h"
}

Image::Image(const char* filepath, int desired_number_of_channels): owner(STB) {
    this->data = stbi_load(filepath, &this->width, &this->height, &this->channels, desired_number_of_channels);
    if (!this->data) {
        throw std::runtime_error("Cannot load image " + std::string(filepath));
    }
    this->channels = desired_number_of_channels;
}

Image::Image(const std::string &filepath, int desired_number_of_channels): owner(STB) {
    this->data = stbi_load(filepath.c_str(), &this->width, &this->height, &this->channels, desired_number_of_channels);
    if (!this->data) {
        throw std::runtime_error("Cannot load image " + filepath);
    }
    this->channels = desired_number_of_channels;
}

Image::Image(int width, int height, int channels): owner(LOCAL) {
    assert(width>0);
    assert(height>0);
    assert(channels>0);

    this->width = width;
    this->height = height;
    this->channels = channels;

    this->data = new uint8_t[width*height*channels];
}

Image::~Image() {
    if (this->data) {
        switch (this->owner) {
        case NONE:
            break;
        case LOCAL:
            delete this->data;
            break;
        case STB:
            stbi_image_free(this->data);
            break;

        default:
            std::cerr << "Not handled owner type " << this->owner << std::endl;
            break;
        }

        this->data = NULL;
        this->owner = NONE;
    }
}

Image::Image(const Image &other) {
    if (this != &other) {
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;

        this->owner = LOCAL;

        this->data = new uint8_t[width*height*channels];
        memcpy(this->data, other.data, width*height*channels);
    }
}

Image::Image(Image &&other) {
    if (this != &other) {
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;
        
        this->owner = other.owner;
        other.owner = NONE;

        this->data = other.data;
        other.data = NULL;
    }
}

Image& Image::operator=(const Image &other) {
    if (this != &other) {
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;
        
        if (this->data) {
            delete this->data;
        }

        this->owner = LOCAL;

        this->data = new uint8_t[width*height*channels];
        memcpy(this->data, other.data, width*height*channels);
    }
    return *this;
}

Image& Image::operator=(Image &&other) {
    if (this != &other) {
        this->width = other.width;
        this->height = other.height;
        this->channels = other.channels;

        this->owner = other.owner;
        other.owner = NONE;

        this->data = other.data;
        other.data = NULL;
    }
    return *this;
}

int Image::save_jpg(const char* filepath, int quality) {
    return stbi_write_jpg(filepath, this->width, this->height, this->channels, this->data, quality);
}
int Image::save_jpg(const std::string &filepath, int quality) {
    return stbi_write_jpg(filepath.c_str(), this->width, this->height, this->channels, this->data, quality);
}

int Image::save_png(const char* filepath) {
    return stbi_write_png(filepath, this->width, this->height, this->channels, this->data, width * channels);
}
int Image::save_png(const std::string &filepath) {
    return stbi_write_png(filepath.c_str(), this->width, this->height, this->channels, this->data, width * channels);
}

uint8_t* Image::at(int x, int y) {
    if (x < 0 || x >= this->width) throw std::range_error("x is out of range: " + std::to_string(x));
    if (y < 0 || y >= this->height) throw std::range_error("y is out of range: " + std::to_string(y));
    
    return this->data + (x + this->width * y) * this->channels;
}

const uint8_t* Image::at(int x, int y) const {
    if (x < 0 || x >= this->width) throw std::range_error("x is out of range: " + std::to_string(x));
    if (y < 0 || y >= this->height) throw std::range_error("y is out of range: " + std::to_string(y));
    
    return this->data + (x + this->width * y) * this->channels;
}





PixelGray::PixelGray(uint8_t value): value(value) {}

PixelGray PixelGray::operator*(float k) {
    PixelGray res(this->value*k);
    return res;
}
PixelGray operator*(float k, PixelGray& pix) {
    PixelGray res(pix.value*k);
    return res;
}
PixelGray PixelGray::operator/(float k) {
    PixelGray res(this->value/k);
    return res;
}

PixelGray PixelGray::operator+(const PixelGray& other) {
    int _value = (int)this->value + (int)other.value;
    if (_value > 255) _value = 255;
    else if (_value < 0) _value = 0;

    PixelGray res(_value);
    return res;
}
PixelGray PixelGray::operator-(const PixelGray& other) {
    int _value = (int)this->value - (int)other.value;
    if (_value > 255) _value = 255;
    else if (_value < 0) _value = 0;

    PixelGray res(_value);
    return res;
}

PixelGray& PixelGray::operator+=(const PixelGray& other) {
    int _value = (int)this->value + (int)other.value;
    if (_value > 255) _value = 255;
    else if (_value < 0) _value = 0;

    this->value = _value;
    return *this;
}
PixelGray& PixelGray::operator-=(const PixelGray& other) {
    int _value = (int)this->value - (int)other.value;
    if (_value > 255) _value = 255;
    else if (_value < 0) _value = 0;

    this->value = _value;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const PixelGray& p) {
    os << "{value: " << p.value <<  "}";
    return os;
}




PixelRGB::PixelRGB(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}

std::ostream& operator<<(std::ostream& os, const PixelRGB& p) {
    os << "{r: " << (int) p.r << ", g: " << (int) p.g << ", b: " << (int) p.b << "}";
    return os;
}

PixelRGB PixelRGB::operator*(float k) {
    PixelRGB res(this->r*k,this->g*k,this->b*k);
    return res;
}

PixelRGB operator*(float k, PixelRGB& pix) {
    PixelRGB res(pix.r*k,pix.g*k,pix.b*k);
    return res;
}

PixelRGB PixelRGB::operator/(float k) {
    PixelRGB res(this->r/k,this->g/k,this->b/k);
    return res;
}


PixelRGBA::PixelRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a): PixelRGB(r, g, b), a(a) {}

std::ostream& operator<<(std::ostream& os, const PixelRGBA& p) {
    os << "{r: " << (int) p.r << ", g: " << (int) p.g << ", b: " << (int) p.b << ", a: " << (int) p.a << "}";
    return os;
}


PixelRGBA PixelRGBA::operator*(float k) {
    PixelRGBA res(this->r*k,this->g*k,this->b*k, this->a);
    return res;
}

PixelRGBA operator*(float k, PixelRGBA& pix) {
    PixelRGBA res(pix.r*k,pix.g*k,pix.b*k, pix.a);
    return res;
}

PixelRGBA PixelRGBA::operator/(float k) {
    PixelRGBA res(this->r/k,this->g/k,this->b/k, this->a);
    return res;
}



PixelRGB PixelRGB::operator+(const PixelRGB& other) {
    int _r = (int)this->r + (int)other.r;
    int _g = (int)this->g + (int)other.g;
    int _b = (int)this->b + (int)other.b;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    PixelRGB res(_r, _g, _b);
    return res;
}
PixelRGB PixelRGB::operator-(const PixelRGB& other) {
    int _r = (int)this->r - (int)other.r;
    int _g = (int)this->g - (int)other.g;
    int _b = (int)this->b - (int)other.b;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    PixelRGB res(_r,_g,_b);
    return res;
}

PixelRGB& PixelRGB::operator+=(const PixelRGB& other) {
    int _r = (int)this->r + (int)other.r;
    int _g = (int)this->g + (int)other.g;
    int _b = (int)this->b + (int)other.b;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    this->r = _r;
    this->g = _g;
    this->b = _b;
    return *this;
}

PixelRGB& PixelRGB::operator-=(const PixelRGB& other) {
    int _r = (int)this->r - (int)other.r;
    int _g = (int)this->g - (int)other.g;
    int _b = (int)this->b - (int)other.b;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    this->r = _r;
    this->g = _g;
    this->b = _b;
    return *this;
}



PixelRGBA PixelRGBA::operator+(const PixelRGBA& other) {
    int _r = (int)this->r + (int)other.r;
    int _g = (int)this->g + (int)other.g;
    int _b = (int)this->b + (int)other.b;
    int _a = ((int)this->a + (int)other.a)/2;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    PixelRGBA res(_r, _g, _b, _a);
    return res;
}
PixelRGBA PixelRGBA::operator-(const PixelRGBA& other) {
    int _r = (int)this->r - (int)other.r;
    int _g = (int)this->g - (int)other.g;
    int _b = (int)this->b - (int)other.b;
    int _a = ((int)this->a - (int)other.a)/2;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    PixelRGBA res(_r, _g, _b, _a);
    return res;
}

PixelRGBA& PixelRGBA::operator+=(const PixelRGBA& other) {
    int _r = (int)this->r + (int)other.r;
    int _g = (int)this->g + (int)other.g;
    int _b = (int)this->b + (int)other.b;
    int _a = ((int)this->a + (int)other.a)/2;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    this->r = _r;
    this->g = _g;
    this->b = _b;
    this->a = _a;

    return *this;
}

PixelRGBA& PixelRGBA::operator-=(const PixelRGBA& other) {
    int _r = (int)this->r - (int)other.r;
    int _g = (int)this->g - (int)other.g;
    int _b = (int)this->b - (int)other.b;
    int _a = ((int)this->a - (int)other.a)/2;
    if (_r > 255) _r = 255;
    else if (_r < 0) _r = 0;
    if (_g > 255) _g = 255;
    else if (_g < 0) _g = 0;
    if (_b > 255) _b = 255;
    else if (_b < 0) _b = 0;

    this->r = _r;
    this->g = _g;
    this->b = _b;
    this->a = _a;
    return *this;
}








ColorRGBA::ColorRGBA(): r(0), g(0), b(0) {}
ColorRGBA::ColorRGBA(double r, double g, double b): r(r), g(g), b(b) {}
ColorRGBA::ColorRGBA(double r, double g, double b, double a): r(r), g(g), b(b), a(a) {}
ColorRGBA::ColorRGBA(const PixelGray& p) {
    this->r = (double)p.value / 255.0;
    this->g = (double)p.value / 255.0;
    this->b = (double)p.value / 255.0;
}
ColorRGBA::ColorRGBA(const PixelRGB& rgb) {
    this->r = (double)rgb.r / 255.0;
    this->g = (double)rgb.g / 255.0;
    this->b = (double)rgb.b / 255.0;
}
ColorRGBA::ColorRGBA(const PixelRGBA& rgba) {
    this->r = (double)rgba.r / 255.0;
    this->g = (double)rgba.g / 255.0;
    this->b = (double)rgba.b / 255.0;
    this->a = (double)rgba.a / 255.0;
}
ColorRGBA::ColorRGBA(const ColorYCbCrA& ycbcra) {
    r = ycbcra.y + 2*(1-ColorYCbCrA::Kr)*(ycbcra.cr);
    b = ycbcra.y + 2*(1-ColorYCbCrA::Kb)*(ycbcra.cb);

    g = (ycbcra.y - ColorYCbCrA::Kr*r - ColorYCbCrA::Kb*b)/(1 - ColorYCbCrA::Kr - ColorYCbCrA::Kb);
}

std::ostream& operator<<(std::ostream& os, const ColorRGBA& c) {
    os << "{r: " << c.r << ", g: " << c.g << ", b: " << c.b << ", a: " << c.a << "}";
    return os;
}

ColorRGBA ColorRGBA::operator*(double k) {
    return ColorRGBA(k*this->r, k*this->g, k*this->b, this->a);
}

ColorRGBA operator*(double k, const ColorRGBA& pix) {
    return ColorRGBA(k*pix.r, k*pix.g, k*pix.b, pix.a);
}

ColorRGBA ColorRGBA::operator/(double k) {
    return ColorRGBA(this->r/k, this->g/k, this->b/k, this->a);
}

ColorRGBA ColorRGBA::operator+(const ColorRGBA& other) {
    return ColorRGBA(this->r + other.r, this->g + other.g, this->b + other.b, (this->a + other.a)/2);
}
ColorRGBA ColorRGBA::operator-(const ColorRGBA& other) {
    return ColorRGBA(this->r - other.r, this->g - other.g, this->b - other.b, (this->a + other.a)/2);
}

ColorRGBA& ColorRGBA::operator*=(double k) {
    this->r *= k;
    this->g *= k;
    this->b *= k;
    return *this;
}

ColorRGBA& ColorRGBA::operator+=(const ColorRGBA& other) {
    this->r += other.r;
    this->g += other.g;
    this->b += other.b;
    return *this;
}

ColorRGBA& ColorRGBA::operator-=(const ColorRGBA& other) {
    this->r -= other.r;
    this->g -= other.g;
    this->b -= other.b;
    return *this;
}

ColorRGBA::operator PixelGray() const  {
    double _value = (this->r + this->g + this->b)/3.0;

    if (_value < 0) _value = 0;
    if (_value > 1) _value = 1;

    return PixelGray(255*_value);
}

ColorRGBA::operator PixelRGB() const  {
    double _r = this->r;
    double _g = this->g;
    double _b = this->b;

    if (_r < 0) _r = 0;
    if (_r > 1) _r = 1;

    if (_g < 0) _g = 0;
    if (_g > 1) _g = 1;

    if (_b < 0) _b = 0;
    if (_b > 1) _b = 1;

    return PixelRGB(255*_r, 255*_g, 255*_b);
}

ColorRGBA::operator PixelRGBA() const {
    double _r = this->r;
    double _g = this->g;
    double _b = this->b;
    double _a = this->a;

    if (_r < 0) _r = 0;
    if (_r > 1) _r = 1;

    if (_g < 0) _g = 0;
    if (_g > 1) _g = 1;

    if (_b < 0) _b = 0;
    if (_b > 1) _b = 1;

    if (_a < 0) _a = 0;
    if (_a > 1) _a = 1;

    return PixelRGBA(255*_r, 255*_g, 255*_b, 255*_a);
}

ColorRGBA::operator ColorYCbCrA() const {
    return ColorYCbCrA(*this);
}


double ColorYCbCrA::Kr = 0.299;
double ColorYCbCrA::Kb = 0.587;
ColorYCbCrA::ColorYCbCrA(): y(0), cb(0), cr(0) {}
ColorYCbCrA::ColorYCbCrA(double y, double cb, double cr): y(y), cb(cb), cr(cr) {}
ColorYCbCrA::ColorYCbCrA(double y, double cb, double cr, double a): y(y), cb(cb), cr(cr), a(a) {}
ColorYCbCrA::ColorYCbCrA(const ColorRGBA& clr) {
    y = Kr*clr.r + (1-Kr-Kb)*clr.g + Kb*clr.b;
    cb = (clr.b - y)/(2*(1 - Kb));
    cr = (clr.r - y)/(2*(1 - Kr));
}

ColorYCbCrA ColorYCbCrA::operator*(double k) {
    return ColorYCbCrA(y*k, cb*k, cr*k, a);
}
ColorYCbCrA ColorYCbCrA::operator/(double k) {
    return ColorYCbCrA(y/k, cb/k, cr/k, a);
}

ColorYCbCrA ColorYCbCrA::operator+(const ColorYCbCrA& other) {
    return ColorYCbCrA(this->y+other.y, this->cb+other.cb, this->cr+other.cr, (this->a+other.a)/2.0);
}
ColorYCbCrA ColorYCbCrA::operator-(const ColorYCbCrA& other) {
    return ColorYCbCrA(this->y-other.y, this->cb-other.cb, this->cr-other.cr, (this->a+other.a)/2.0);
}

ColorYCbCrA& ColorYCbCrA::operator*=(double k) {
    this->y *= k;
    this->cb *= k;
    this->cr *= k;
    return *this;
}

ColorYCbCrA& ColorYCbCrA::operator+=(const ColorYCbCrA& other) {
    this->y += other.y;
    this->cb += other.cb;
    this->cr += other.cr;
    return *this;
}
ColorYCbCrA& ColorYCbCrA::operator-=(const ColorYCbCrA& other) {
    this->y -= other.y;
    this->cb -= other.cb;
    this->cr -= other.cr;
    return *this;
}

ColorYCbCrA::operator ColorRGBA() const {
    return ColorRGBA(*this);
}

ColorYCbCrA operator*(double k, const ColorYCbCrA& pix) {
    return ColorYCbCrA(pix.y*k, pix.cb*k, pix.cr*k, pix.a);
}
std::ostream& operator<<(std::ostream& os, const ColorYCbCrA& p) {
    os << "{Y: " << p.y << ", Cb: " << p.cb << ", Cr: " << p.cr << ", A: " << p.a << "}";
    return os;
}


#endif // STB_IMAGE_WRAPPER_IMPLEMENTATION
