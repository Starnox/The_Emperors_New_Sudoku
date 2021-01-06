#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "bmp_header.h"
#include "cJSON.h"

#pragma pack(1)

typedef struct
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
}Pixel;

typedef struct 
{
    bmp_fileheader fileheader;
    bmp_infoheader infoheader;
} bitmap;

#pragma pack()

char * ReadJSON(const char *argument);
bitmap * ParseJSON(const char * const inputString, Pixel **pixelMatrix);
bmp_fileheader * ParseFileHeader(cJSON *sudoku);
bmp_infoheader * ParseInfoHeader(cJSON *sudoku);
Pixel * ParseBitmap(cJSON *sudoku, int width, int height);
void CreateBMPFromJPEG(bitmap *myBitmap, Pixel *pixelMatrix);

