#define STB_IMAGE_WRAPPER_IMPLEMENTATION
#include "image.hpp"
#undef STB_IMAGE_WRAPPER_IMPLEMENTATION

#define STB_IMAGE_WRAPPER_EDIT_IMPLEMENTATION
#include "image_edit.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Input file is expected" << std::endl;
        return 1;
    }
    char* filename = argv[1];

    {
        Image img(filename, 3);
        std::cout << "Image loaded: " << img.width << " x " << img.height << " x " << img.channels << " channels" << std::endl;
        
        Image img2 = img;
        std::cout << "Image copied" << std::endl;

        for (int x=0; x<img.width/2; x++){
            for (int y=0; y<img.height/2; y++) {
                ColorRGBA clr( *(PixelRGB*)img.at(x, y) );
                double v = (clr.r + clr.g + clr.b)/3;
                clr.r = v; clr.g = v; clr.b = v;
                *(PixelRGB*)img.at(x, y) = (PixelRGB)clr;
            }
        }

        draw_line<PixelRGB>(img, 0, 0, img.width/2, img.height, PixelRGB{255, 0, 0}, 10);
        add_line_edge<PixelRGB>(img, 0, img.height/2, img.width, img.height/2, PixelRGB{0, 0, 255}, 10, 0.5);

        // ColorYCbCrA::Kr = 0.5;
        for (int x=0; x<img2.width; x++){
            for (int y=0; y<img2.height; y++) {
                ColorRGBA rgba = *(PixelRGB*)img2.at(x, y);
                ColorYCbCrA ycbcra = rgba;
                // ycbcra.y = 0;
                // ycbcra.cb *= 0.4;
                ycbcra.cb = 0;
                // ycbcra.cr = 0;
                ColorRGBA final_clr = ycbcra;
                *(PixelRGB*)img2.at(x, y) = (PixelRGB)final_clr;
            }
        }

        ColorRGBA rgba = *(PixelRGB*)img.at(50, 50);
        ColorYCbCrA ycbcra = rgba;
        ColorRGBA final_clr = ycbcra;
        std::cout << rgba << " -> " << ycbcra << " -> " << final_clr << std::endl;

        std::cout << "Image edited" << std::endl;

        img.save_png("edit.png");
        img2.save_png("low_cb.png");

        std::cout << "Image saved" << std::endl;


    }
    std::cout << "Image deinitialized" << std::endl;


    return 0;
}