#include "main.h"
/* 
²âÊÔ£ºÊä³öÈý·ùHSL->»Ò¶ÈÍ¼Ïñ
 */
#ifdef _TEST_HSL_

RgbQuad imageH[IMG_SIZE];
RgbQuad imageS[IMG_SIZE];
RgbQuad imageL[IMG_SIZE];
void buildHSL()
{
	unsigned int s, d;
	RgbQuad *rgbLineC;
	RgbQuad *rgbLineH;
	RgbQuad *rgbLineS;
	RgbQuad *rgbLineL;
	unsigned short maxColor = 0;
	unsigned char *max;
	unsigned char *mid;
	unsigned char *min;
	for (y = 0; y < infoHead.biHeight; y++)
	{
		rgbLineC = imageC + y * infoHead.biWidth;
		rgbLineH = imageH + y * infoHead.biWidth;
		rgbLineS = imageS + y * infoHead.biWidth;
		rgbLineL = imageL + y * infoHead.biWidth;
		x = 0;
		while (x < infoHead.biWidth)
		{
			rgbLineL->Reserved = 0xFF;
			if (rgbLineC->Red > rgbLineC->Blue)
			{
				max = &(rgbLineC->Red);
				min = &(rgbLineC->Blue);
				maxColor = 0;
			}
			else
			{
				max = &(rgbLineC->Blue);
				min = &(rgbLineC->Red);
				maxColor = 0x400;
			}
			if (rgbLineC->Green > *max)
			{
				//G is Max
				maxColor = 0x200;
				mid = max;
				max = &(rgbLineC->Green);
			}
			else if (rgbLineC->Green < *min)
			{
				//G is Min
				mid = min;
				min = &(rgbLineC->Green);
			}
			else
			{
				//H.G is Mid
				mid = &(rgbLineC->Green);
			}
			d = *max - *min;
			s = (unsigned short)*max + *min;
			//Light
			rgbLineL->Red = rgbLineL->Blue = rgbLineL->Green =
				s / 2;
			//Hue
			rgbLineH->Red = rgbLineH->Blue = rgbLineH->Green =
				d ? (maxColor * 0x100 / 3 + (*mid - *min) * 0x100 / 3 / d) : 0;
			//Saturation
			if (s > 0xFF)
			{
				s = 0x200 - s;
			}
			rgbLineS->Red = rgbLineS->Blue = rgbLineS->Green =
				s ? (d * 0x100 / s) : 0xFF;
			//Hue'
			// if (d)
			// {
			// 	*mid = (unsigned short)(*mid - *min) * 0xFF / d;
			// 	*max = 0xFF;
			// 	*min = 0;
			// }
			// else
			// {
			// 	*max = *mid = *min = 0x80;
			// }
			// rgbLineC->Red = rgbLineC->Blue = rgbLineC->Green =
			// 	((unsigned short)rgbLineC->Red * rgbLineC->Red +
			// 	 (unsigned short)rgbLineC->Green * rgbLineC->Green +
			// 	 (unsigned short)rgbLineC->Blue * rgbLineC->Blue) /
			// 	0x300;
			// *max = (unsigned short)*max * (*max) / 0x100;
			// *mid = (unsigned short)*mid * (*mid) / 0x100;
			// *min = (unsigned short)*min * (*min) / 0x100;
			// ((unsigned short)rgbLineC->Red + rgbLineC->Green + rgbLineC->Blue) / 0x3;

			rgbLineC->Reserved = 0xFF;
			rgbLineH->Reserved = 0xFF;
			rgbLineS->Reserved = 0xFF;
			rgbLineL->Reserved = 0xFF;

			rgbLineC++;
			rgbLineH++;
			rgbLineS++;
			rgbLineL++;
			x++;
		}
	}
}
#endif // _TEST_HSL_
