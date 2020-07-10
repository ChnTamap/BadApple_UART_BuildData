#include "main.h"

RgbQuad imageI[IMG_SIZE] = {};
RgbQuad imageO[IMG_SIZE] = {};
BitMapFileHeader fileHead = {};
BitMapInfoHeader infoHead = {};

int main(int argc, char const *argv[])
{
	char fileName[64] = "";
	unsigned char *datas;
	// char nameLink[] = "out.bmp";
	// char *pStr0;
	// char *pStr1;

	test_transPhoto();

	system("PAUSE");


	// test_match();
	// while (1)
	// {
	// 	printf("Input fileName:\n");
	// 	if (scanf("%s", fileName) != 1)
	// 		return 0;
	// 	//Read File
	// 	readFile(fileName, &fileHead, &infoHead, imageI);
	// 	pStr0 = fileName;

	// 	test_conv();

	// 	while (*(++pStr0))
	// 		;
	// 	pStr1 = nameLink;
	// 	while ((*(pStr0++) = *(pStr1++)) > 0)
	// 		;
	// 	writeFile(fileName, &fileHead, &infoHead, imageO);
	// }
	return 0;
}
