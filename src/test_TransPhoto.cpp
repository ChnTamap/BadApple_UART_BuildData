#include "main.h"
#include "time.h"

#ifdef _TEST_TRANSPHOTO_

#define _OUTPUT_TEST_

RgbQuad imageLCD[IMG_SIZE];		//图像
BitMapFileHeader fileHeadLCD;   //文件头
BitMapInfoHeader infoHeadLCD;   //图像头
unsigned char bitMap[IMG_SIZE]; //输出二进制
char StringMap[IMG_SIZE];		//输出数组

/* LeftToRight is a line*/
void test_TransPhoto(char *name)
{
	RgbQuad *pImg;
	unsigned char *pByte;
	unsigned int i;
	unsigned int j = 0;
	readFile(name, &fileHeadLCD, &infoHeadLCD, imageLCD);
	i = infoHeadLCD.biWidth * infoHeadLCD.biHeight;
	pImg = imageLCD + i - 1;
	pByte = bitMap;
	while (i)
	{
		i--;
		*pByte <<= 1;
		if (pImg->Red > 128)
		{
			(*pByte)++;
		}
		pImg--;
		j++;
		if (j == 8)
		{
			j = 0;
			printf("0x%02X,", *pByte);
			pByte++;
		}
		if (i % infoHeadLCD.biWidth == 0)
			printf("\n");
	}
	if (j != 0)
	{
		printf("%02X", *pByte);
		pByte++;
	}
	printf("\nSize of bytes:%lld\n", (pByte - bitMap));
}

//测试颜色黑白
#define TEST_COLOR(pimg) (pimg->Red > 128)
/* 命令定义 */
//位图模式
#define CMD_MODE_BIT 0x01
#define CMD_MODE_BIT_EXIT0 0x00
#define CMD_MODE_BIT_EXIT1 0xFF
//双字节段命令
#define CMD_MODE_LONG 0x02
/* 模式 */
//段模式
#define MODE_LEN 0
//位图模式
#define MODE_BIT 1

/* 
1.换行延续
	换行时自动转换为上一行首的点色，不论与上一行行末是否发生变色，计数器跨过此点继续计数。
	换行时行首与上一行行首发生变色，计数器在此处打点标记换色。
2.复杂场景
	段长度小于8时应进入位图模式以节省空间，在段模式0x01作为进入位图模式命令。
	段长度大于等于8时应退出位图模式以节省空间，在位图模式0xFF为1色（0x00为0色）退出位图模式。
3.过长段
	段长度大于0xFF时应以0x02 XX XX作为双字节段命令。
4.文件头
	文件头预留8字节存储信息，0 1 字节以双字节表示数据长度，2 字节表示起始色，其他预留。
 */
unsigned char *test_TransPhoto2(char *name)
{
	//像素指针
	RgbQuad *pImg;
	//索引计数
	unsigned int i;
	//计数器
	unsigned int count;
	//位图字节
	unsigned char bits;
	//模式
	unsigned char mode;
	//数据指针
	unsigned char *pData;
	//前一个颜色
	unsigned char LastColor;
	//前一行颜色
	unsigned char LastColorLine;
	//颜色测试结果缓存
	unsigned char TestBuf;

	//读取文件
	readFile(name, &fileHeadLCD, &infoHeadLCD, imageLCD);
	//设置计数总数
	i = infoHeadLCD.biWidth * infoHeadLCD.biHeight;
	//取得起始像素
	pImg = imageLCD + i - 1;
	//初始化模式
	mode = MODE_LEN;
	//设置文件头.起始色 预设前一个颜色
	LastColorLine = LastColor = HeadBitMap(bitMap)->StartColor = TEST_COLOR(pImg);
	//预设bits
	bits = LastColor ? 0x01 : 0x00;
	//设置数据指针
	pData = bitMap + sizeof(TurnColor_TypeDef);
	//清空计数器
	count = 0;

FIX:

	//开始循环
	while (i)
	{
		//下一个像素
		pImg--;
		i--;
		count++;
		//测试颜色
		TestBuf = TEST_COLOR(pImg);
		//换行判断
		if ((i % infoHeadLCD.biWidth) == 0)
		{
			LastColor = LastColorLine;
			LastColorLine = TestBuf;
#ifdef _OUTPUT_TEST_
			if (mode == MODE_LEN && count < 8)
				printf("<<");
			printf("\n");
#endif // _OUTPUT_TEST_
		}
		//更新bits
		bits <<= 1;
		if (TestBuf)
			bits++;

		/* 模式判断 */
		if (mode == MODE_LEN)
		//段模式
		{
			if (TestBuf != LastColor)
			{
				//记录数据 反转颜色
				LastColor = TestBuf;
				//判断复杂场景（过短段）
				if (count < 8)
				{
					*(pData++) = CMD_MODE_BIT;
					mode = MODE_BIT;
					//经演算，转入BIT模式应补偿1计数
					count++;
				}
				else
				{
					//判断过长段
					if (count > 0xFF)
					{
						*(pData++) = CMD_MODE_LONG;
						*((unsigned short *)pData) = count;
						pData += 2;
					}
					else
						*(pData++) = count;
					//经过演算，此处应清除为0
					count = 0;
				}
			}
		}
		if (mode == MODE_BIT)
		// 位图模式
		{
			//判断集齐8位
			if (count >= 8)
			{
				LastColor = TestBuf;
				//判断是否全0或全1，退出位图模式
				if (bits == 0x00)
				{
					if (i)
						mode = MODE_LEN;
					*(pData++) = CMD_MODE_BIT_EXIT0;
					//经演算，退出位图模式需补偿count-1
					count--;
				}
				else if (bits == 0xFF)
				{
					if (i)
						mode = MODE_LEN;
					*(pData++) = CMD_MODE_BIT_EXIT1;
					count--;
				}
				else
				//记录bits
				{
					*(pData++) = bits;
					count = 0;
				}
			}
		}
#ifdef _OUTPUT_TEST_
		//Out
		if (TestBuf)
			printf("%02X", (unsigned char)count);
		else
			printf("  ");
#endif // _OUTPUT_TEST_
	}
	//位图补偿
	if (mode == MODE_BIT)
	{
		//暂时不补偿
	}
	if (mode == MODE_LEN && count > 1)
	{
		//段补偿
		i++;
		pImg++;
		count--;
		LastColorLine = LastColor = !LastColor;
		goto FIX;
	}
	//Out
	if (pData - bitMap > 2000)
		printf(" :: Warrning :: %lld\n", pData - bitMap);
	//设置文件头.数据长度
	HeadBitMap(bitMap)->Length = pData - bitMap;
	return bitMap;
}

void readTurnPhoto(unsigned char *datas)
{
	TurnColor_TypeDef *pHead;
	//索引计数 i 图像 j 数据
	unsigned int i, j;
	//计数器
	unsigned int count;
	//当前颜色
	unsigned char colorNow;
	//上行颜色
	unsigned char colorLine;
	//模式
	unsigned char mode;

	//获取文件头
	pHead = HeadBitMap(datas);
	printf("Length\t:%d\n", pHead->Length);
	printf("SrtCor\t:%d\n", pHead->StartColor);
	//初始化
	i = 240 * 180;
	j = pHead->Length;
	mode = MODE_LEN;
	count = 0;
	//配置起始颜色
	colorLine = colorNow = pHead->StartColor;

	//获取数据
	datas += sizeof(TurnColor_TypeDef);

	/* 循环 */
	do
	{
		//模式
		if (mode == MODE_LEN)
		//段模式
		{
			if (*datas == CMD_MODE_BIT)
			//进入位图模式
			{
				count = 8;
				mode = MODE_BIT;
			}
			else
			{
				if (*datas == CMD_MODE_LONG)
				//双字长
				{
					datas++;
					count = *((unsigned short *)datas);
					datas++;
					j += 2;
				}
				else
				{
					count = *datas;
				}
				if (i % 240 == 0)
					colorLine = colorNow;

				// 输出
				while (count-- && i)
				{
					if (colorNow)
						printf("%02X", (unsigned char)count);
					else
						printf("  ");
					i--;
					if (i % 240 == 0)
					{
						printf("*\n");
						colorNow = colorLine;
					}
				}

				//切换颜色
				colorNow = !colorNow;
			}
		}
		else if (mode == MODE_BIT)
		//位图模式
		{
			if (*datas == CMD_MODE_BIT_EXIT0)
			//退出位图模式
			{
				mode = MODE_LEN;
				colorNow = 0;
				count = 0;
				if((i % 240) < 8)
					colorLine = 0;
			}
			else if (*datas == CMD_MODE_BIT_EXIT1)
			{
				mode = MODE_LEN;
				colorNow = 1;
				count = 0;
				if((i % 240) < 8)
					colorLine = 1;
			}
			else
			{
				colorNow = *datas;
				count = 8;

				while (count-- && i)
				{
					if (i % 240 == 0)
						colorLine = colorNow & 0x80;
					if (colorNow & 0x80)
						printf("%02X", (unsigned char)count);
					else
						printf("  ");
					colorNow <<= 1;
					i--;
					if (i % 240 == 0)
						printf("*\n");
				}
			}
		}
		datas++;
		j++;
	} while (i && j);
}

void test_transPhotoSTR()
{
	char fileName[64] = "";
	unsigned char *datas;
	int i, max, number;
	unsigned short lenMax;
	FILE *fptr;
	char str[9600];
	unsigned char *pData;
	unsigned char *pDataEnd;

	printf("file number:");
	scanf("%d", &max);

	sprintf(fileName, "badappleL.h");
	fptr = fopen(fileName, "w");
	if (fptr == NULL)
	{
		printf("Error!");
		exit(1);
	}

	number = 0;
	lenMax = 0;
	for (i = 1; i < max + 1; i++)
	{
		sprintf(fileName, "ph (%d).bmp", i);
		datas = test_TransPhoto2(fileName);
		pData = datas;
		pDataEnd = datas + HeadBitMap(datas)->Length;
		number += HeadBitMap(datas)->Length;

		fprintf(fptr, "const unsigned char ph%d[%d] = {\n", i, HeadBitMap(datas)->Length);
		while (pData < pDataEnd)
		{
			sprintf(str, "0x%02X,", *(pData++));
			fprintf(fptr, "%s", str);
			if (((pDataEnd - pData) % 24) == 0)
			{
				fprintf(fptr, "\n");
			}
		}

#ifdef _OUTPUT_TEST_
		readTurnPhoto(datas);
		system("PAUSE");
#endif // _OUTPUT_TEST_

		fprintf(fptr, "\n};\n");
	}

	fprintf(fptr, "const unsigned char *phs[%d] = {\n", max);
	for (i = 1; i < max + 1; i++)
	{
		fprintf(fptr, "ph%d,", i);
		if ((i % 10) == 0)
			fprintf(fptr, "\n");
	}
	fprintf(fptr, "\n};\n");

	fclose(fptr);

	printf("\nNumber=%d", number);
}

void test_transPhotoHEX()
{
	char fileName[64] = "";
	unsigned char *datas;
	unsigned int i, max, number;
	unsigned short lenMax;
	FILE *fptr;
	unsigned char *pData;
	unsigned char *pDataEnd;
	unsigned char headZero[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	printf("file number:");
	scanf("%d", &max);

	sprintf(fileName, "badapple.bin");
	fptr = fopen(fileName, "wb");
	if (fptr == NULL)
	{
		printf("Error!");
		exit(1);
	}

	number = 0;
	lenMax = 0;
	for (i = 1; i < max + 1; i++)
	{
		sprintf(fileName, "ph (%d).bmp", i);
		datas = test_TransPhoto2(fileName);
		pData = datas;
		pDataEnd = datas + HeadBitMap(datas)->Length;
		number += HeadBitMap(datas)->Length;
		if (HeadBitMap(datas)->Length > lenMax)
			lenMax = HeadBitMap(datas)->Length;

		fwrite(datas, sizeof(unsigned char), HeadBitMap(datas)->Length, fptr);

		printf("\rFinish:%d/%d\tAvgSize:%d\tMaxSize:%d", i, max, number / i, lenMax);

#ifdef _OUTPUT_TEST_
		readTurnPhoto(datas);
		system("PAUSE");
#endif // _OUTPUT_TEST_
	}

	fwrite(headZero, sizeof(unsigned char), sizeof(TurnColor_TypeDef), fptr);
	fclose(fptr);

	printf("\nNumber=%d", number);
}

void test_transPhoto()
{
	test_transPhotoHEX();
}

#endif //_TEST_TRANSPHOTO_
