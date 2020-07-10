#include "main.h"
#include "time.h"

#ifdef _TEST_MATCH_

RgbQuad imageL[IMG_SIZE];   //图像
BitMapFileHeader fileHeadL; //文件头
BitMapInfoHeader infoHeadL; //图像头
RgbQuad imageR[IMG_SIZE];
BitMapFileHeader fileHeadR;
BitMapInfoHeader infoHeadR;
RgbQuad imageS[IMG_SIZE * 2];
BitMapFileHeader fileHeadS;
BitMapInfoHeader infoHeadS;

unsigned int diffOfPix(RgbQuad *pixPos, RgbQuad *pixNeg)
{
	int d;
	unsigned int Sd = 0;
	d = (int)(pixPos->Red) - pixNeg->Red;
	if (d < 0)
		d = -d;
	Sd += d;
	d = (int)(pixPos->Green) - pixNeg->Green;
	if (d < 0)
		d = -d;
	Sd += d;
	d = (int)(pixPos->Blue) - pixNeg->Blue;
	if (d < 0)
		d = -d;
	Sd += d;

	return Sd;
}

#define random(n) (rand() % (n))
#define MATCH_TIMES 100

void matching(RgbQuad *imgL, RgbQuad *imgR, RgbQuad *imgOut, unsigned long imgWidth, unsigned long imgHeight,
			  unsigned int kernelWidth, unsigned int kernelHeight)
{
	unsigned int kernelX;
	unsigned int kernelY;
	unsigned int areaX; //扫描区域X坐标
	unsigned int areaEndX;
	unsigned int areaY;
	unsigned int areaEndY;
	unsigned int sx, sy, ix, iy, i;
	unsigned int d; //区域差值
	unsigned int dMin;
	unsigned int xMin;
	unsigned int yMin;
	RgbQuad color;
	//拼合图像
	srand(time(NULL));
	for (sy = 0; sy < imgHeight; sy++)
	{
		for (sx = 0; sx < imgWidth; sx++)
		{
			//Test2不拼右图
			//右图
			// *(imgOut + sy * imgWidth * 2 + sx) =
			// 	*(imgR + sy * imgWidth + sx);
			//左图
			*(imgOut + sy * imgWidth * 2 + sx + imgWidth) =
				*(imgL + sy * imgWidth + sx);
		}
	}

	//Test1:随机对左图取区域进行匹配
	//匹配成功后将两个区域在左右图中画出并输出图像
	//Test2:遍历左图区域匹配右图
	//匹配后将左图区域像素映射到右图匹配区域

	//Test1
	// for (i = 0; i < MATCH_TIMES; i++)
	//Test2
	for (kernelY = 0; kernelY < imgHeight - kernelHeight; kernelY += kernelHeight)
		for (kernelX = 0; kernelX < imgWidth - kernelWidth; kernelX += kernelWidth)
		{
			//1取出左图kernel区域：得到kernel左上角像素的坐标
			//Tes1
			// kernelX = random(imgWidth - kernelWidth);
			// kernelY = random(imgHeight - kernelHeight);
			//Test2

			/* 2估计出右图可能出现匹配成功的区域 :
		中心坐标与kernel对齐
		y方向高度在kernel附近较小区域（相机高度相同） 
		x方向宽度在kernel附近较广区域（视差）
	*/
			areaX = 0;			 //先试试从左到右全部扫描
			areaEndX = imgWidth; //同上
			if (kernelY > kernelHeight / 2)
				areaY = kernelY - kernelHeight / 2; //Kernel的2倍
			else
				areaY = 0;
			areaEndY = areaY + kernelHeight * 2; //同上
			if (areaEndY > imgHeight)
				areaEndY = imgHeight;

			//3卷积，匹配
			dMin = (unsigned int)0 - 1;
			xMin = areaX;
			yMin = areaY;
			for (sy = areaY; sy < areaEndY - kernelHeight + 1; sy++)
			{
				for (sx = areaX; sx < areaEndX - kernelWidth + 1; sx++)
				{
					//右图对应区域左上角像素确定
					d = 0; //初始化差值d=0
					for (iy = 0; iy < kernelHeight; iy++)
					{
						for (ix = 0; ix < kernelWidth; ix++)
						{
							//Idea：两图三个通道差的绝对值之和
							d += diffOfPix(imageR + (sy + iy) * imgWidth + sx + ix,
										   imageL + (kernelY + iy) * imgWidth + kernelX + ix);
						}
					}
					if (d < dMin)
					{
						dMin = d;
						xMin = sx;
						yMin = sy;
#ifdef DEBUG
						printf("Find new dMin:%d at (%d,%d)\n", d, sx, sy);
#endif // DEBUG
					}
				}
			}

#ifdef DEBUG
			printf("kernelX:%d,kernelY:%d\n", kernelX, kernelY);
			printf("xMin:%d,yMin:%d,dMin:%d\n", xMin, yMin, dMin);
#endif // DEBUG

			//4表达，标记匹配点
			//Test2映射
			for (sy = 0; sy < kernelHeight; sy++)
			{
				for (sx = 0; sx < kernelWidth; sx++)
				{
					*(imgOut + (yMin + sy) * imgWidth * 2 + (xMin + sx)) =
						*(imgL + (kernelY + sy) * imgWidth + (kernelX + sx));
				}
			}
			if (xMin > kernelX)
				xMin -= kernelX;
			else
				xMin = kernelX - xMin;
			//Test1画线
			if (xMin > 60)
			{
				color.Reserved = 0xFF;
				color.Red = random(0xFF);
				color.Blue = random(0xFF);
				color.Green = random(0xFF);
				graphics_drawLine(imgOut, imgWidth * 2, imgHeight, imgWidth + kernelX, kernelY, xMin, yMin, color);
			}
			//
			/* for (sy = 0; sy < kernelHeight; sy++)
	{
		*(imgOut + kernelX + ((kernelY + sy) * 2 + 1) * imgWidth) = color;
		*(imgOut + kernelX + kernelWidth + ((kernelY + sy) * 2 + 1) * imgWidth) = color;
		*(imgOut + xMin + (yMin + sy) * imgWidth * 2) = color;
		*(imgOut + xMin + kernelWidth + (yMin + sy) * imgWidth * 2) = color;
	}
	for (sx = 0; sx < kernelWidth; sx++)
	{
		*(imgOut + (sx + kernelX) + (kernelY * 2 + 1) * imgWidth) = color;
		*(imgOut + (sx + kernelX) + ((kernelHeight + kernelY) * 2 + 1) * imgWidth) = color;
		*(imgOut + (sx + xMin) + yMin * imgWidth * 2) = color;
		*(imgOut + (sx + xMin) + (kernelHeight + yMin) * imgWidth * 2) = color;
	} */
		}
}

void test_match()
{
	readFile("left1.bmp", &fileHeadL, &infoHeadL, imageL);
	readFile("right1.bmp", &fileHeadR, &infoHeadR, imageR);

	fileHeadS = fileHeadL;
	infoHeadS = infoHeadL;
	bmp_setSize(&fileHeadS, &infoHeadS,
				((infoHeadL.biWidth < infoHeadR.biWidth) ? infoHeadL.biWidth : infoHeadR.biWidth) * 2,
				((infoHeadL.biHeight < infoHeadR.biHeight) ? infoHeadL.biHeight : infoHeadR.biHeight));
	// readFile("visionMod1.bmp", &fileHeadS, &infoHeadS, imageS);
	matching(imageL, imageR, imageS, infoHeadS.biWidth / 2, infoHeadS.biHeight,
			 5, 5);
	writeFile("vision1.bmp", &fileHeadS, &infoHeadS, imageS);
}

#endif // _TEST_MATCH_