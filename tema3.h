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
    uint8_t b;
    uint8_t g;
    uint8_t r;
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
void CreateBMP(bitmap *myBitmap, unsigned char *img);
int CheckWhitePixel(Pixel *pixel);
unsigned char *TransformPixelMatrix(Pixel *pixelMatrix, bitmap *myBitmap);
void MirrorNumbers(bitmap *myBitmap, Pixel **pixelMatrix);
void SwapPixels(Pixel * pixel1, Pixel *pixel2);
void SwapNumbers(uint8_t *a, uint8_t *b);