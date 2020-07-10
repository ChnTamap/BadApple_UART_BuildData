#include "main.h"

#ifdef _TEST_CONV_

//测试：卷积
#define getEle(map, x, y, w) \
	((map) + (y) * (w) + (x))

#define ValueOfConv 0x40

//卷积函数
void convolution(RgbQuad *mapIn, RgbQuad *mapOut, long width, long height, char *kernel, unsigned int kernelSize)
{
	unsigned int x, y;
	char k;
	unsigned int kx, ky;
	long outHeight = height - (kernelSize - 1);
	long outWidth = width - (kernelSize - 1);
	int sr, sg, sb;
	RgbQuad *pix;
	//Begin
	for (y = 0; y < outHeight; y++)
	{
		for (x = 0; x < outWidth; x++)
		{
			//Pix
			sr = sg = sb = 0;
			for (ky = 0; ky < kernelSize; ky++)
			{
				for (kx = 0; kx < kernelSize; kx++)
				{
					//Kernel pix
					pix = (mapIn + (y + ky) * width + x + kx);
					k = *(kernel + ky * kernelSize + kx);
					sr += k * pix->Red;
					sg += k * pix->Green;
					sb += k * pix->Blue;
				}
			}
			//Set to mapOut
			pix = (mapOut + (y + kernelSize / 2) * width + x + kernelSize / 2);
			pix->Red = (sr > ValueOfConv) ? 0xFF : ((sr < -ValueOfConv) ? 0 : 0x80);
			pix->Green = (sg > ValueOfConv) ? 0xFF : ((sg < -ValueOfConv) ? 0 : 0x80);
			pix->Blue = (sb > ValueOfConv) ? 0xFF : ((sb < -ValueOfConv) ? 0 : 0x80);
		}
	}
}

//卷积核
#define KERNEL_SIZE 5
char blob_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
	{-1, -1, -1, -1, -1},
	{-1, +1, +1, +1, -1},
	{-1, +1, +8, +1, -1},
	{-1, +1, +1, +1, -1},
	{-1, -1, -1, -1, -1} //斑点
};
char line_kernel[3][3] = {
	{-1,-1,-1},
	{-1,+0,+1},
	{+1,+1,+1}
};
void test_conv()
{
	// convolution(imageI, imageO, infoHead.biWidth, infoHead.biHeight, (char *)blob_kernel, KERNEL_SIZE);
	convolution(imageI, imageO, infoHead.biWidth, infoHead.biHeight, (char *)line_kernel, 3);
}

#endif // _TEST_CONV_
