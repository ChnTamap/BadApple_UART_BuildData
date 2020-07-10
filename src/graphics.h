#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//Define color struct
#include "bmp.h"
typedef RgbQuad Color;

//Graphics
void graphics_drawLine(
	Color *image, unsigned int width, unsigned int height,
	unsigned int sx, unsigned int sy, unsigned int ex, unsigned int ey,
	Color lineColor);

#define GetPixAdr(img, width, x, y) (img + y * width + x)

#endif // _GRAPHICS_H_