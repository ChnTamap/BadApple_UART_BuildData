/*************************

*bmp.h文件

*************************/

#ifndef __BMP_H__
#define __BMP_H__

#include <unistd.h>
#include <stdio.h>

//文件头结构体
typedef struct
{
	//文件类型
	unsigned char bfType[2];
	//位图大小
	unsigned long bfSize;
	unsigned short bfReserved1; //位0
	unsigned short bfReserved2; //位0
	//到数据偏移量
	unsigned long bfOffBits;
} __attribute__((packed)) BitMapFileHeader;
//使编译器不优化，其大小为14字节

//信息头结构体
typedef struct
{
	// BitMapFileHeader 字节数
	unsigned long biSize;
	//位图宽度
	long biWidth;
	//位图高度，正位正向，反之为倒图
	long biHeight;
	//为目标设备说明位面数，其值将总是被设为1
	unsigned short biPlanes;
	//说明比特数/象素，为1、4、8、16、24、或32。
	unsigned short biBitCount;
	//图象数据压缩的类型没有压缩的类型：BI_RGB
	unsigned long biCompression;
	//说明图象的大小，以字节为单位
	unsigned long biSizeImage;
	//说明水平分辨率
	long biXPelsPerMeter;
	//说明垂直分辨率
	long biYPelsPerMeter;
	//说明位图实际使用的彩色表中的颜色索引数
	unsigned long biClrUsed;
	//对图象显示有重要影响的索引数，0都重要。
	unsigned long biClrImportant;
} __attribute__((packed)) BitMapInfoHeader;

//像素点结构体
typedef struct
{
	//该颜色的蓝色分量
	unsigned char Blue;
	//该颜色的绿色分量
	unsigned char Green;
	//该颜色的红色分量
	unsigned char Red;
	//保留值（亮度）
	unsigned char Reserved;
} __attribute__((packed)) RgbQuad;

//File
int readFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img);
int writeFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img);
void bmp_setSize(BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead,
				 long biWidth, long biHeight);

#endif //__BMP_H__