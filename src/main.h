#ifndef _MAIN_H_
#define _MAIN_H_

//Std
#include "stdio.h"
#include "stdlib.h"

//BMP
#include "bmp.h"
#include "graphics.h"

//Main
#define IMG_SIZE 3200*960		  //ͼ��ߴ�
extern RgbQuad imageI[IMG_SIZE];  //��ȡ����ͼ��
extern RgbQuad imageO[IMG_SIZE];  //�����ͼ��
extern BitMapFileHeader fileHead; //�ļ�ͷ
extern BitMapInfoHeader infoHead; //ͼ��ͷ

//Test
//#define _TEST_CHECK_
#ifdef _TEST_CHECK_
void test_check();
#endif // _TEST_CHECK_

//#define _TEST_HSL_
#ifdef _TEST_HSL_
void test_HSL();
#endif // _TEST_HSL_

// #define _TEST_CONV_
#ifdef _TEST_CONV_
void test_conv();
#endif // _TEST_CONV_

// #define _TEST_MATCH_
#ifdef _TEST_MATCH_
void test_match();
#endif // _TEST_MATCH_

#define _TEST_TRANSPHOTO_
#ifdef _TEST_TRANSPHOTO_
//�ļ�ͷ���ݽṹ
typedef struct
{
	unsigned short Length;
	unsigned char StartColor;
	unsigned char x0;
	unsigned char x1;
	unsigned char x2;
	unsigned char x3;
	unsigned char x4;
} TurnColor_TypeDef;
//�ļ�ͷBitMap
#define HeadBitMap(bmap) ((TurnColor_TypeDef *)bmap)
//Function
void test_TransPhoto(char *name);
unsigned char *test_TransPhoto2(char *name);
void readTurnPhoto(unsigned char *datas);
void test_transPhoto();
#endif // _TEST_TRANSPHOTO_

#endif // !_MAIN_H_