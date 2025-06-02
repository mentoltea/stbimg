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
            for (int y=0; y<img.width/2; y++) {
                PixelRGB *pxl;
                pxl = (PixelRGB*) img.at(x,y);
                pxl->r = 125;
                pxl->g = 0;
                pxl->b = 0;

                pxl = (PixelRGB*) img2.at(x,y);
                pxl->r = 125;
                pxl->g = 125;
                pxl->b = 125;
            }
        }

        draw_line<PixelRGB>(img, 0, 0, img.width/2, img.height, PixelRGB{255, 0, 0}, 10);
        add_line<PixelRGB>(img, 0, img.height/2, img.width, img.height/2, PixelRGB{0, 0, 255}, 10, 0.9);

        std::cout << "Image edited" << std::endl;

        img.save_png("saved.png");
        img2.save_png("saved2.png");
        std::cout << "Image saved" << std::endl;
    }
    std::cout << "Image deinitialized" << std::endl;

    return 0;
}