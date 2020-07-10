#include "main.h"
#include "time.h"

#ifdef _TEST_MATCH_

RgbQuad imageL[IMG_SIZE];   //ͼ��
BitMapFileHeader fileHeadL; //�ļ�ͷ
BitMapInfoHeader infoHeadL; //ͼ��ͷ
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
	unsigned int areaX; //ɨ������X����
	unsigned int areaEndX;
	unsigned int areaY;
	unsigned int areaEndY;
	unsigned int sx, sy, ix, iy, i;
	unsigned int d; //�����ֵ
	unsigned int dMin;
	unsigned int xMin;
	unsigned int yMin;
	RgbQuad color;
	//ƴ��ͼ��
	srand(time(NULL));
	for (sy = 0; sy < imgHeight; sy++)
	{
		for (sx = 0; sx < imgWidth; sx++)
		{
			//Test2��ƴ��ͼ
			//��ͼ
			// *(imgOut + sy * imgWidth * 2 + sx) =
			// 	*(imgR + sy * imgWidth + sx);
			//��ͼ
			*(imgOut + sy * imgWidth * 2 + sx + imgWidth) =
				*(imgL + sy * imgWidth + sx);
		}
	}

	//Test1:�������ͼȡ�������ƥ��
	//ƥ��ɹ�����������������ͼ�л��������ͼ��
	//Test2:������ͼ����ƥ����ͼ
	//ƥ�����ͼ��������ӳ�䵽��ͼƥ������

	//Test1
	// for (i = 0; i < MATCH_TIMES; i++)
	//Test2
	for (kernelY = 0; kernelY < imgHeight - kernelHeight; kernelY += kernelHeight)
		for (kernelX = 0; kernelX < imgWidth - kernelWidth; kernelX += kernelWidth)
		{
			//1ȡ����ͼkernel���򣺵õ�kernel���Ͻ����ص�����
			//Tes1
			// kernelX = random(imgWidth - kernelWidth);
			// kernelY = random(imgHeight - kernelHeight);
			//Test2

			/* 2���Ƴ���ͼ���ܳ���ƥ��ɹ������� :
		����������kernel����
		y����߶���kernel������С��������߶���ͬ�� 
		x��������kernel�����Ϲ������Ӳ
	*/
			areaX = 0;			 //�����Դ�����ȫ��ɨ��
			areaEndX = imgWidth; //ͬ��
			if (kernelY > kernelHeight / 2)
				areaY = kernelY - kernelHeight / 2; //Kernel��2��
			else
				areaY = 0;
			areaEndY = areaY + kernelHeight * 2; //ͬ��
			if (areaEndY > imgHeight)
				areaEndY = imgHeight;

			//3�����ƥ��
			dMin = (unsigned int)0 - 1;
			xMin = areaX;
			yMin = areaY;
			for (sy = areaY; sy < areaEndY - kernelHeight + 1; sy++)
			{
				for (sx = areaX; sx < areaEndX - kernelWidth + 1; sx++)
				{
					//��ͼ��Ӧ�������Ͻ�����ȷ��
					d = 0; //��ʼ����ֵd=0
					for (iy = 0; iy < kernelHeight; iy++)
					{
						for (ix = 0; ix < kernelWidth; ix++)
						{
							//Idea����ͼ����ͨ����ľ���ֵ֮��
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

			//4�����ƥ���
			//Test2ӳ��
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
			//Test1����
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