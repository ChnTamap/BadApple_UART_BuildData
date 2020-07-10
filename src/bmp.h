/*************************

*bmp.h�ļ�

*************************/

#ifndef __BMP_H__
#define __BMP_H__

#include <unistd.h>
#include <stdio.h>

//�ļ�ͷ�ṹ��
typedef struct
{
	//�ļ�����
	unsigned char bfType[2];
	//λͼ��С
	unsigned long bfSize;
	unsigned short bfReserved1; //λ0
	unsigned short bfReserved2; //λ0
	//������ƫ����
	unsigned long bfOffBits;
} __attribute__((packed)) BitMapFileHeader;
//ʹ���������Ż������СΪ14�ֽ�

//��Ϣͷ�ṹ��
typedef struct
{
	// BitMapFileHeader �ֽ���
	unsigned long biSize;
	//λͼ���
	long biWidth;
	//λͼ�߶ȣ���λ���򣬷�֮Ϊ��ͼ
	long biHeight;
	//ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1
	unsigned short biPlanes;
	//˵��������/���أ�Ϊ1��4��8��16��24����32��
	unsigned short biBitCount;
	//ͼ������ѹ��������û��ѹ�������ͣ�BI_RGB
	unsigned long biCompression;
	//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ
	unsigned long biSizeImage;
	//˵��ˮƽ�ֱ���
	long biXPelsPerMeter;
	//˵����ֱ�ֱ���
	long biYPelsPerMeter;
	//˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
	unsigned long biClrUsed;
	//��ͼ����ʾ����ҪӰ�����������0����Ҫ��
	unsigned long biClrImportant;
} __attribute__((packed)) BitMapInfoHeader;

//���ص�ṹ��
typedef struct
{
	//����ɫ����ɫ����
	unsigned char Blue;
	//����ɫ����ɫ����
	unsigned char Green;
	//����ɫ�ĺ�ɫ����
	unsigned char Red;
	//����ֵ�����ȣ�
	unsigned char Reserved;
} __attribute__((packed)) RgbQuad;

//File
int readFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img);
int writeFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img);
void bmp_setSize(BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead,
				 long biWidth, long biHeight);

#endif //__BMP_H__