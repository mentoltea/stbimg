# STBIMG
This project is an wrapper for simplified use of stb image in C++.
Currently it consists of [image](image.hpp) & [image_edit](image_edit.hpp) headers which are header-only libraries (like the stb itself).
As the names suggest, [image](image.hpp) is responsible for general structures, where as [image_edit](image_edit.hpp) - for editing images.

---
# Image
To include implementation, define `STB_IMAGE_WRAPPER_IMPLEMENTATION`.

## Image class
Straight up an abstraction over stb images with same properties (`width`,  `height`, `channels`). 

The data is contained as one chunk of unsorted `uint8_t`. The type of data is described by `channels`:
- 1 => Grayscale (1 byte/pixel)
- 3 => RGB (3 bytes/pixel)
- 4 => RGBA (4 bytes/pixel)
You can specify in which mode to open the image by parameter `desired_number_of_channels` in constructor from file. (value 0 means to open image in mode that it was saved in).

Memory management is organised with `MemoryOwner` enum:
- `NONE` (0) => Image does not own memory at all (may be used in future for ImageViews)
- `LOCAL` (1) => Memory is owned by Image class (currently, with default `new` and `delete`)
- `STB` (2) => Memory is owned by STB
This diffirentiation is needed for the case when you want (or have) to use different allocators for stb and your program.

## Pixels & Colors
Pixel structures are meant to correspond to size of suppored channel modes:
- `PixelGray` for grayscale (1 channel)
- `PixelRGB` for RGB (3 channels)
- `PixelRGBA` for RGBA (4 channels)
Because of this correspondance it is safe to use them on raw data pointer returned by `Image.at(x,y)`:
```cpp
PixelRGB pixel = *(PixelRGB*)Image.at(x,y);
```
(as long as you're sure that you are using appropriate pixel structure for the channel mode)

To abstract away from specific pixel types, `ColorRGBA` structure is introduces. It can cast to / be constructed from any pixel structure, making it's usage universal. 
Also, in difference of discussed pixel structures, `ColorRGBA` is normalized (all it's values is in range [0,1] instead of [0,255]) and is not afraid of overflowing/underflowing as it uses `double` to store the data, which makes it ideal for mathematical operations on colors.
When casting back to specified pixel structure, it automatically maps it's values to [0,255] considering probable over&under-flow.

Some more usefull (but not so essential) functions of image processing can be found in example [here](https://github.com/mentoltea/gausian-blur-cpp).

---
# Image Edit
To include implementation, define `STB_IMAGE_WRAPPER_EDIT_IMPLEMENTATION`.  
As this library depends on [image.hpp](image.hpp), you should consider undefining `STB_IMAGE_WRAPPER_IMPLEMENTATION` if you want to place implementations to different places. Otherwhise, it is recommended to include `Image` implementation together with `Image Edit` by including only `Image Edit`.   

***IMPORTANT NOTE***: in given below descriptions, `ColorT` <u>must be one of **pixel structures**, NOT ColorRGBA</u>, as it is written directly to memory.


## plot(Image&, float x, float y, ColorT)
Sets pixel of type `ColorT` at given coordinates.

## plot_add(Image&, float x, float y, ColorT, float k)
Adds given pixel to pixel at given coordinates and writes to that place by formula:  
`k*given + (1-k)*current`  
Parameter `k` can be thought of as 'Color strength factor' - when it is 1, the behaviour is identical to `plot` (hard drawing); when it is 0, the drawing does not affect the image at all.

## draw_line(Image&, float x0, float y0, float x1, float y1, ColorT, float thick)
Draws anti-aliased line with given thickness of given pixel color using modified version of *Xiaolin Wu Algorythm*.  
Is based on `plot`.

## add_line_edge<ColorT>(Image&, float x0, float y0, float x1, float y1, ColorT, float thick, float k = 0.5)
~~Draws~~ *Adds* anti-aliased line with given thickness of given pixel color using modified version of *Xiaolin Wu Algorythm*.  
Is based on `plot_add`. By default, `k` is equal to 0.5, which means the average of colors.  
This algorythm <u>draws lines with dark (black) edges!</u>

## add_line_noedge<ColorT>(Image&, float x0, float y0, float x1, float y1, ColorT, float thick, float k = 0.5)
Absolutly identical to `add_line_edge` with one difference:  
This algorythm <u>does not draw line edges!</u> It basically means that almost no anti-aliasing is taking place, as the line is drawn in solid color.  
However, this fact can be compensated by using color merge provided with `plot_add`.