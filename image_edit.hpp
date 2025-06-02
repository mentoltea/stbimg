#ifndef STB_IMAGE_WRAPPER_EDIT_INCLUDE
#define STB_IMAGE_WRAPPER_EDIT_INCLUDE

#include "image.hpp"
#include <math.h>

#endif // STB_IMAGE_WRAPPER_EDIT_INCLUDE

template<class ColorT>
inline void plot(Image &img, float x, float y, ColorT clr);

template<class ColorT>
inline void plot_add(Image &img, float x, float y, ColorT clr, float k);

template<class ColorT>
void draw_line(Image &img, 
    float x0, float y0,
    float x1, float y1,
    ColorT clr,
    float thickness
);

template<class ColorT>
void add_line(Image &img, 
    float x0, float y0,
    float x1, float y1,
    ColorT clr,
    float thickness,
    float k = 0.5
);

#ifdef STB_IMAGE_WRAPPER_EDIT_IMPLEMENTATION

#define swap(xold,xnew) {   \
    float temp;   \
    temp = xold; xold = xnew; xnew = temp;  \
}

#define ipart(x) ((float)((int)(x)))

#define round(x) (float)( ipart(x + 0.5) )

#define fpart(x) (x - ipart(x))

#define rfpart(x) (1 - fpart(x))


template<class ColorT>
inline void plot(Image &img, float x, float y, ColorT clr) {
    int ix = (int)round(x);
    if (ix<0) ix = 0;
    else if (ix >= img.width) ix = img.width-1;

    int iy = (int)round(y);
    if (iy<0) iy = 0;
    else if (iy >= img.height) iy = img.height-1;
    
    ColorT *current = (ColorT*)(img.at(ix, iy));
    *current = clr;
}

template<class ColorT>
inline void plot_add(Image &img, float x, float y, ColorT clr, float k) {
    int ix = (int)round(x);
    if (ix<0) ix = 0;
    else if (ix >= img.width) ix = img.width-1;

    int iy = (int)round(y);
    if (iy<0) iy = 0;
    else if (iy >= img.height) iy = img.height-1;
    
    ColorT *current = (ColorT*)(img.at(ix, iy));
    *current = (clr*k + (*current)*(1-k));
}

template<class ColorT>
void draw_line(Image &img, 
    float x0, float y0,
    float x1, float y1,
    ColorT clr,
    float thickness
) {
    bool steep = abs(y1-y0) > abs(x1 - x0); 

    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient;
    
    if (dx == 0)    gradient = 1;
    else            gradient = dy/dx;
    
    float w = thickness * sqrt(1 + (gradient * gradient));

    float xend = round(x0);
    float yend = y0 + (w-1)/2 + gradient*(xend - x0);
    float xgap = rfpart(x0 + 0.5);
    
    float xpxl1 = xend;
    float ypxl1 = (float) ( (int) (yend));
    
    if (steep) {
        plot<ColorT>(img, ypxl1, xpxl1, clr * rfpart(yend) * xgap);
        for (int i=1; i<w; i++) {
            plot<ColorT>(img, ypxl1+i, xpxl1, clr * 1);
        }
        plot<ColorT>(img, ypxl1+ipart(w), xpxl1, clr * fpart(yend) * xgap);
    }
    else {
        plot<ColorT>(img, xpxl1, ypxl1, clr * rfpart(yend) * xgap);
        for (int i=1; i<w; i++) {
            plot<ColorT>(img, xpxl1, ypxl1+i, clr * 1);
        }
        plot<ColorT>(img, xpxl1, ypxl1+ipart(w), clr * fpart(yend) * xgap);
    }
    
    float intery = yend + gradient;
    
    xend = round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5);
    
    float xpxl2 = xend; //this will be used in the main loop
    float ypxl2 = (float)((int)(yend));
    if (steep) {
        plot<ColorT>(img, ypxl2, xpxl2, clr * rfpart(yend) * xgap);
        for (int i=1; i<w; i++) {
            plot<ColorT>(img, ypxl2+i, xpxl2, clr * 1);
        }
        plot<ColorT>(img, ypxl2+ipart(w), xpxl2, clr * fpart(yend) * xgap);
    }
    else {
        plot<ColorT>(img, xpxl2, ypxl2, clr * rfpart(yend) * xgap);
        for (int i=1; i<w; i++) {
            plot<ColorT>(img, xpxl2, ypxl2+i, clr * 1);
        }
        plot<ColorT>(img, xpxl2, ypxl2+ipart(w), clr * fpart(yend) * xgap);
    }
    
    // main loop
    if (steep) {
        for (int x = xpxl1 + 1;  x <= xpxl2 - 1; x++) {
            plot<ColorT>(img, ipart(intery), x, clr * rfpart(yend) * xgap);
            for (int i=1; i<w; i++) {
                plot<ColorT>(img, ipart(intery)+i, x, clr * 1);
            }
            plot<ColorT>(img, ipart(intery)+ipart(w), x, clr * fpart(yend) * xgap);
            intery = intery + gradient;
        }
    }
    else {
        for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
            plot<ColorT>(img, x, ipart(intery), clr * rfpart(yend) * xgap);
            for (int i=1; i<w; i++) {
                plot<ColorT>(img, x, ipart(intery)+i, clr * 1);
            }
            plot<ColorT>(img, x, ipart(intery)+ipart(w), clr * fpart(yend) * xgap);

            intery = intery + gradient;
        }
    }
}


template<class ColorT>
void add_line(Image &img, 
    float x0, float y0,
    float x1, float y1,
    ColorT clr,
    float thickness,
    float k
) {
    bool steep = abs(y1-y0) > abs(x1 - x0); 

    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient;
    
    if (dx == 0)    gradient = 1;
    else            gradient = dy/dx;
    
    float w = thickness * sqrt(1 + (gradient * gradient));

    float xend = round(x0);
    float yend = y0 + (w-1)/2 + gradient*(xend - x0);
    float xgap = rfpart(x0 + 0.5);
    
    float xpxl1 = xend;
    float ypxl1 = (float) ( (int) (yend));
    
    if (steep) {
        plot_add<ColorT>(img, ypxl1, xpxl1, clr * rfpart(yend) * xgap, k);
        for (int i=1; i<w; i++) {
            plot_add<ColorT>(img, ypxl1+i, xpxl1, clr * 1, k);
        }
        plot_add<ColorT>(img, ypxl1+ipart(w), xpxl1, clr * fpart(yend) * xgap, k);
    }
    else {
        plot_add<ColorT>(img, xpxl1, ypxl1, clr * rfpart(yend) * xgap, k);
        for (int i=1; i<w; i++) {
            plot_add<ColorT>(img, xpxl1, ypxl1+i, clr * 1, k);
        }
        plot_add<ColorT>(img, xpxl1, ypxl1+ipart(w), clr * fpart(yend) * xgap, k);
    }
    
    float intery = yend + gradient;
    
    xend = round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5);
    
    float xpxl2 = xend; //this will be used in the main loop
    float ypxl2 = (float)((int)(yend));
    if (steep) {
        plot_add<ColorT>(img, ypxl2, xpxl2, clr * rfpart(yend) * xgap, k);
        for (int i=1; i<w; i++) {
            plot_add<ColorT>(img, ypxl2+i, xpxl2, clr * 1, k);
        }
        plot_add<ColorT>(img, ypxl2+ipart(w), xpxl2, clr * fpart(yend) * xgap, k);
    }
    else {
        plot_add<ColorT>(img, xpxl2, ypxl2, clr * rfpart(yend) * xgap, k);
        for (int i=1; i<w; i++) {
            plot_add<ColorT>(img, xpxl2, ypxl2+i, clr * 1, k);
        }
        plot_add<ColorT>(img, xpxl2, ypxl2+ipart(w), clr * fpart(yend) * xgap, k);
    }
    
    // main loop
    if (steep) {
        for (int x = xpxl1 + 1;  x <= xpxl2 - 1; x++) {
            plot_add<ColorT>(img, ipart(intery), x, clr * rfpart(yend) * xgap, k);
            for (int i=1; i<w; i++) {
                plot_add<ColorT>(img, ipart(intery)+i, x, clr * 1, k);
            }
            plot_add<ColorT>(img, ipart(intery)+ipart(w), x, clr * fpart(yend) * xgap, k);
            intery = intery + gradient;
        }
    }
    else {
        for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
            plot_add<ColorT>(img, x, ipart(intery), clr * rfpart(yend) * xgap, k);
            for (int i=1; i<w; i++) {
                plot_add<ColorT>(img, x, ipart(intery)+i, clr * 1, k);
            }
            plot_add<ColorT>(img, x, ipart(intery)+ipart(w), clr * fpart(yend) * xgap, k);

            intery = intery + gradient;
        }
    }
}

#endif // STB_IMAGE_WRAPPER_EDIT_IMPLEMENTATION