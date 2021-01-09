#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "bmp_header.h"
#include "cJSON.h"

#pragma pack(1)

#define NUM_OF_PIXELS_IN_CELL 49
#define CELLS_NUMBER 9

#define MASK1 "0000000000001000000100000010000001000000100000000"
#define MASK2 "0000000011111001000000111110000001001111100000000"
#define MASK3 "0000000011111000000100111110000001001111100000000"
#define MASK4 "0000000000001000000100111110010001001000100000000"
#define MASK5 "0000000011111000000100111110010000001111100000000"
#define MASK6 "0000000011111001000100111110010000001111100000000"
#define MASK7 "0000000000001000000100000010000001001111100000000"
#define MASK8 "0000000011111001000100111110010001001111100000000"
#define MASK9 "0000000011111000000100111110010001001111100000000"

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
void CreateBMP(bitmap *myBitmap, unsigned char *img, int taskNumber,
                char *boardNumber);
int CheckWhitePixel(Pixel *pixel);
int CheckPinkPixel(Pixel *pixel);
unsigned char *TransformPixelMatrix(Pixel *pixelMatrix, bitmap *myBitmap);
void MirrorNumbers(bitmap *myBitmap, Pixel **pixelMatrix);
void SwapPixels(Pixel * pixel1, Pixel *pixel2);
void SwapNumbers(uint8_t *a, uint8_t *b);
void Solve123(char *inputFile);
void Solve4();
void SolveBonus();
char * GetLastNumberFromString(char *inputString);
char *** ConstructCellsMask(Pixel *pixelMatrix, bitmap *myBitmap);
void FreePlayBoard(char ***gameBoard);
void FreeGameBoardNumbers(int **gameBoard);
int CellToNumber(char *input);
int ** TransformGameboard(char ***gameBoard);
int CheckGameBoard(int ** gameBoardNumbers);
void WriteResultJSON(int win, int taskNumber, char *boardNumber);
