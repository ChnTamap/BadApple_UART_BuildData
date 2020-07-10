#include "main.h"
#include "time.h"

#ifdef _TEST_TRANSPHOTO_

#define _OUTPUT_TEST_

RgbQuad imageLCD[IMG_SIZE];		//ͼ��
BitMapFileHeader fileHeadLCD;   //�ļ�ͷ
BitMapInfoHeader infoHeadLCD;   //ͼ��ͷ
unsigned char bitMap[IMG_SIZE]; //���������
char StringMap[IMG_SIZE];		//�������

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

//������ɫ�ڰ�
#define TEST_COLOR(pimg) (pimg->Red > 128)
/* ����� */
//λͼģʽ
#define CMD_MODE_BIT 0x01
#define CMD_MODE_BIT_EXIT0 0x00
#define CMD_MODE_BIT_EXIT1 0xFF
//˫�ֽڶ�����
#define CMD_MODE_LONG 0x02
/* ģʽ */
//��ģʽ
#define MODE_LEN 0
//λͼģʽ
#define MODE_BIT 1

/* 
1.��������
	����ʱ�Զ�ת��Ϊ��һ���׵ĵ�ɫ����������һ����ĩ�Ƿ�����ɫ������������˵����������
	����ʱ��������һ�����׷�����ɫ���������ڴ˴�����ǻ�ɫ��
2.���ӳ���
	�γ���С��8ʱӦ����λͼģʽ�Խ�ʡ�ռ䣬�ڶ�ģʽ0x01��Ϊ����λͼģʽ���
	�γ��ȴ��ڵ���8ʱӦ�˳�λͼģʽ�Խ�ʡ�ռ䣬��λͼģʽ0xFFΪ1ɫ��0x00Ϊ0ɫ���˳�λͼģʽ��
3.������
	�γ��ȴ���0xFFʱӦ��0x02 XX XX��Ϊ˫�ֽڶ����
4.�ļ�ͷ
	�ļ�ͷԤ��8�ֽڴ洢��Ϣ��0 1 �ֽ���˫�ֽڱ�ʾ���ݳ��ȣ�2 �ֽڱ�ʾ��ʼɫ������Ԥ����
 */
unsigned char *test_TransPhoto2(char *name)
{
	//����ָ��
	RgbQuad *pImg;
	//��������
	unsigned int i;
	//������
	unsigned int count;
	//λͼ�ֽ�
	unsigned char bits;
	//ģʽ
	unsigned char mode;
	//����ָ��
	unsigned char *pData;
	//ǰһ����ɫ
	unsigned char LastColor;
	//ǰһ����ɫ
	unsigned char LastColorLine;
	//��ɫ���Խ������
	unsigned char TestBuf;

	//��ȡ�ļ�
	readFile(name, &fileHeadLCD, &infoHeadLCD, imageLCD);
	//���ü�������
	i = infoHeadLCD.biWidth * infoHeadLCD.biHeight;
	//ȡ����ʼ����
	pImg = imageLCD + i - 1;
	//��ʼ��ģʽ
	mode = MODE_LEN;
	//�����ļ�ͷ.��ʼɫ Ԥ��ǰһ����ɫ
	LastColorLine = LastColor = HeadBitMap(bitMap)->StartColor = TEST_COLOR(pImg);
	//Ԥ��bits
	bits = LastColor ? 0x01 : 0x00;
	//��������ָ��
	pData = bitMap + sizeof(TurnColor_TypeDef);
	//��ռ�����
	count = 0;

FIX:

	//��ʼѭ��
	while (i)
	{
		//��һ������
		pImg--;
		i--;
		count++;
		//������ɫ
		TestBuf = TEST_COLOR(pImg);
		//�����ж�
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
		//����bits
		bits <<= 1;
		if (TestBuf)
			bits++;

		/* ģʽ�ж� */
		if (mode == MODE_LEN)
		//��ģʽ
		{
			if (TestBuf != LastColor)
			{
				//��¼���� ��ת��ɫ
				LastColor = TestBuf;
				//�жϸ��ӳ��������̶Σ�
				if (count < 8)
				{
					*(pData++) = CMD_MODE_BIT;
					mode = MODE_BIT;
					//�����㣬ת��BITģʽӦ����1����
					count++;
				}
				else
				{
					//�жϹ�����
					if (count > 0xFF)
					{
						*(pData++) = CMD_MODE_LONG;
						*((unsigned short *)pData) = count;
						pData += 2;
					}
					else
						*(pData++) = count;
					//�������㣬�˴�Ӧ���Ϊ0
					count = 0;
				}
			}
		}
		if (mode == MODE_BIT)
		// λͼģʽ
		{
			//�жϼ���8λ
			if (count >= 8)
			{
				LastColor = TestBuf;
				//�ж��Ƿ�ȫ0��ȫ1���˳�λͼģʽ
				if (bits == 0x00)
				{
					if (i)
						mode = MODE_LEN;
					*(pData++) = CMD_MODE_BIT_EXIT0;
					//�����㣬�˳�λͼģʽ�貹��count-1
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
				//��¼bits
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
	//λͼ����
	if (mode == MODE_BIT)
	{
		//��ʱ������
	}
	if (mode == MODE_LEN && count > 1)
	{
		//�β���
		i++;
		pImg++;
		count--;
		LastColorLine = LastColor = !LastColor;
		goto FIX;
	}
	//Out
	if (pData - bitMap > 2000)
		printf(" :: Warrning :: %lld\n", pData - bitMap);
	//�����ļ�ͷ.���ݳ���
	HeadBitMap(bitMap)->Length = pData - bitMap;
	return bitMap;
}

void readTurnPhoto(unsigned char *datas)
{
	TurnColor_TypeDef *pHead;
	//�������� i ͼ�� j ����
	unsigned int i, j;
	//������
	unsigned int count;
	//��ǰ��ɫ
	unsigned char colorNow;
	//������ɫ
	unsigned char colorLine;
	//ģʽ
	unsigned char mode;

	//��ȡ�ļ�ͷ
	pHead = HeadBitMap(datas);
	printf("Length\t:%d\n", pHead->Length);
	printf("SrtCor\t:%d\n", pHead->StartColor);
	//��ʼ��
	i = 240 * 180;
	j = pHead->Length;
	mode = MODE_LEN;
	count = 0;
	//������ʼ��ɫ
	colorLine = colorNow = pHead->StartColor;

	//��ȡ����
	datas += sizeof(TurnColor_TypeDef);

	/* ѭ�� */
	do
	{
		//ģʽ
		if (mode == MODE_LEN)
		//��ģʽ
		{
			if (*datas == CMD_MODE_BIT)
			//����λͼģʽ
			{
				count = 8;
				mode = MODE_BIT;
			}
			else
			{
				if (*datas == CMD_MODE_LONG)
				//˫�ֳ�
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

				// ���
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

				//�л���ɫ
				colorNow = !colorNow;
			}
		}
		else if (mode == MODE_BIT)
		//λͼģʽ
		{
			if (*datas == CMD_MODE_BIT_EXIT0)
			//�˳�λͼģʽ
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
