#include "main.h"

/* 
测试：扫描+简单的漫水（腐蚀）检测满足条件的点 
*/
#ifdef _TEST_CHECK_

typedef struct
{
	unsigned short x;
	unsigned short y;
} __attribute__((packed)) Point;

#define COLOR_D 20
#define COLOR_R 28  //216
#define COLOR_G 69  //155
#define COLOR_B 101 //135
#define CHECK_STEP 1
RgbQuad SampMax = {COLOR_B + COLOR_D, COLOR_G + COLOR_D, COLOR_R + COLOR_D, 0xFF};
RgbQuad SampMin = {COLOR_B - COLOR_D, COLOR_G - COLOR_D, COLOR_R - COLOR_D, 0xFF};
#define CUT_NUM 0x40
#define MARK 0
#define NMARK 1
Point pointList[IMG_SIZE];	 //检查像素队列，队列里必然是检查过的像素
Point *pPoint = pointList - 1; //队列顶端。不是将要检查的像素，运行时检查的是其周围的像素
//检查给定的像素
void inline checkPoint(unsigned short x, unsigned short y)
{
	RgbQuad *pix = &(imageC[y * infoHead.biWidth + x]);
	unsigned char *max;
	unsigned char *mid;
	unsigned char *min;
	//Is it NMark.
	if (pix->Reserved)
	{
		//1:Mark
		pix->Reserved = MARK;
		//2:Check
		if (pix->Red > pix->Blue)
		{
			max = &(pix->Red);
			min = &(pix->Blue);
			return;
		}
		else
		{
			max = &(pix->Blue);
			min = &(pix->Red);
		}
		if (pix->Green > *max)
		{
			//G is Max
			mid = max;
			max = &(pix->Green);
			return;
		}
		else if (pix->Green < *min)
		{
			//G is Min
			mid = min;
			min = &(pix->Green);
			return;
		}
		else
		{
			//G is Mid
			mid = &(pix->Green);
			if (*mid > SampMax.Green)
				return;
			if (*mid < SampMin.Green)
				return;
		}
		// printf("Check Pass:(%d,%d)-(%x %x %x)", x, y, pix->Red, pix->Green, pix->Blue);
		//3:Push
		pix->Red += (0xE0 - pix->Red) >> 2; //Test
		pix->Green += (0xE0 - pix->Green) >> 2;
		pix->Blue += (0xE0 - pix->Blue) >> 2;
		pPoint++;
		pPoint->x = x;
		pPoint->y = y;
	}
}
void test_check()
{
	int x, y;
	unsigned short biWidth = infoHead.biWidth;
	unsigned short biHeight = infoHead.biHeight;
	//防止跨界
	// y = (infoHead.biHeight - 1) * infoHead.biWidth;
	// x = 0;
	// while (x < infoHead.biWidth)
	// {
	// 	imageC[x].Reserved = imageC[y + x].Reserved = MARK;
	// 	x++;
	// }
	// x = (infoHead.biWidth - 1) * infoHead.biHeight;
	// y = 0;
	// while (y < infoHead.biWidth)
	// {
	// 	imageC[y].Reserved = imageC[x + y].Reserved = MARK;
	// 	y++;
	// }

	//采样扫描
	int xPl, yPl; //增量
	//分割
	xPl = biWidth / CUT_NUM;
	yPl = biHeight / CUT_NUM;
	//扫描
	for (x = 1; x < biWidth; x += xPl)
	{
		for (y = 1; y < biHeight; y += yPl)
		{
			checkPoint(x, y);
		}
	}

	//提前偏置像素检查步进边界
	biWidth -= CHECK_STEP;
	biHeight -= CHECK_STEP;
	while (pPoint >= pointList)
	{
		//预先提取xy，checkPoint函数会改变pPoint的值
		x = pPoint->x;
		y = pPoint->y;
		//Pop当前pPoint
		pPoint--;
		//检查MARK队列顶端周围的像素
		if (y >= CHECK_STEP)
			checkPoint(x, y - CHECK_STEP);
		if (x < biWidth)
			checkPoint(x + CHECK_STEP, y);
		if (y < biHeight)
			checkPoint(x, y + CHECK_STEP);
		if (x >= CHECK_STEP)
			checkPoint(x - CHECK_STEP, y);
	}
}

#endif // _TEST_CHECK_
