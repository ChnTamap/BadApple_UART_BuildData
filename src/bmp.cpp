#include "bmp.h"

int readFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img)
{
	FILE *fid; //File
	//Open File
	fid = fopen(fileName, "rb");
	if (fid == NULL)
	{
		printf("Feild to open file: %s\n", fileName);
		return -1;
	}
	//Read Information
	if (fread(pFileHead, sizeof(BitMapFileHeader), 1, fid) != 1)
	{
		printf("Read BitMapFileHeader error\n");
		fclose(fid);
		return -1;
	}
	if (memcmp(pFileHead->bfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose(fid);
		return -1;
	}

	//Read Bitmap Information
	if (1 != fread((char *)pInfoHead, sizeof(BitMapInfoHeader), 1, fid))
	{
		printf("Read BitMapInfoHeader error!\n");
		fclose(fid);
		return -1;
	}
	// printf("Image size: %ld*%ld\n", pInfoHead->biWidth, pInfoHead->biHeight);

	//Jump to Data
	fseek(fid, pFileHead->bfOffBits, SEEK_SET);

	unsigned char len = pInfoHead->biBitCount / 8; //Byte number of image
	unsigned int rowSize =
		(((pInfoHead->biWidth * pInfoHead->biBitCount) + 31) >> 5) << 2;
	rowSize -= pInfoHead->biWidth * len;
	unsigned int x = pInfoHead->biWidth;

	while (!feof(fid))
	{
		img->Reserved = 0xFF;
		if (len != fread((char *)img++, 1, len, fid))
			break;
		x--;
		if (x == 0)
		{
			x = pInfoHead->biWidth;
			if (rowSize != fread((char *)img, 1, rowSize, fid))
				break;
		}
	}

	fclose(fid);

	return 0;
}

int writeFile(const char *fileName, BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead, RgbQuad *img)
{
	FILE *fid; //File
	//Open File
	fid = fopen(fileName, "wb");
	if (fid == NULL)
	{
		printf("Feild to open file: %s\n", fileName);
		return -1;
	}
	//Write Information
	if (1 != fwrite(pFileHead, sizeof(BitMapFileHeader), 1, fid))
	{
		printf("Write BitMapFileHeader error\n");
		fclose(fid);
		return -1;
	}

	//Write Bitmap Information
	if (1 != fwrite((char *)pInfoHead, sizeof(BitMapInfoHeader), 1, fid))
	{
		printf("Write BitMapInfoHeader error!\n");
		fclose(fid);
		return -1;
	}

	//Jump to Data
	// fseek(fid, pFileHead->bfOffBits, SEEK_SET);

	int len = pInfoHead->biBitCount / 8; //Byte number of image
	unsigned long imageLength = (pInfoHead->biWidth) * pInfoHead->biHeight;
	unsigned int rowSize =
		(((pInfoHead->biWidth * pInfoHead->biBitCount) + 31) >> 5) << 2;
	rowSize -= pInfoHead->biWidth * len;
	unsigned int x = pInfoHead->biWidth;

	while (imageLength--)
	{
		if (len != fwrite((char *)img++, 1, len, fid))
			break;
		x--;
		if (x == 0)
		{
			x = pInfoHead->biWidth;
			if (rowSize != fwrite((char *)img, 1, rowSize, fid))
				break;
		}
	}

	fclose(fid);

	return 0;
}

void bmp_setSize(BitMapFileHeader *pFileHead, BitMapInfoHeader *pInfoHead,
				 long biWidth, long biHeight)
{
	//Set size
	pInfoHead->biWidth = biWidth;
	pInfoHead->biHeight = biHeight;
	pInfoHead->biSizeImage =
		pInfoHead->biHeight *
		((((pInfoHead->biWidth * pInfoHead->biBitCount) + 31) >> 5) << 2);
	pFileHead->bfSize = pFileHead->bfOffBits + pInfoHead->biSizeImage;
}