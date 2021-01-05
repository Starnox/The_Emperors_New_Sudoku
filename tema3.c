#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "tema3.h"
#include "bmp_header.h"

void ParseFileHeader(cJSON *sudoku ,bmp_fileheader *myFileHeader);
void ParseInfoHeader(cJSON *sudoku ,bmp_infoheader *myInfoHeader);
void ParseBitmap(cJSON *sudoku ,Pixel *pixelMatrix);

int main(int argc, char *argv[] )
{
    char *inputString = NULL;
    if(argc != 2)
    {
        printf("Please provide one input file.");
    }
    
    // Read the JSON file and store it into a string
    inputString = ReadJSON(argv[1]);
    if(inputString == NULL)
    {
        printf("Couldn't read from file");
        return 1;
    }
    
    // TODO free inputString

    // TASK1
    Pixel *pixelMatrix = NULL;
    ParseJSON(inputString,pixelMatrix);


    free(inputString);
    return 0;
}

char * ReadJSON(const char *argument)
{
    long length;
    char *inputString = NULL;

    FILE *inputFile = fopen(argument, "r");
    if(inputFile != NULL)
    {
        fseek(inputFile, 0, SEEK_END);
        length = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);
        if(length >= 0)
        {
            inputString = malloc(sizeof(char) * length);
            if(inputString != NULL)
            {
                fread(inputString, 1, length, inputFile);
            }
            fclose(inputFile);
        }
    }
    return inputString;
}

void ParseJSON(const char * const inputString, Pixel *pixelMatrix)
{
    // TODO change NUM_OF_PIXELS with values from the file
    int status = 0;

    // declaring
    bmp_fileheader *myFileHeader = NULL;
    bmp_infoheader *myInfoheader = NULL;
    // TODO free those two

    // parse the JSON and verify
    cJSON *sudoku = cJSON_ParseWithLength(inputString, strlen(inputString));
    if(sudoku == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        return;
    }

    ParseFileHeader(sudoku,myFileHeader);
    ParseInfoHeader(sudoku,myInfoheader);
    ParseBitmap(sudoku, pixelMatrix);


    free(pixelMatrix);
    free(myFileHeader);
    free(myInfoheader);
    return;

}

void ParseFileHeader(cJSON *sudoku ,bmp_fileheader *myFileHeader)
{
    myFileHeader = (bmp_fileheader *) calloc(1,sizeof(bmp_fileheader));
    if(myFileHeader == NULL)
        return;
    // variables for file_headers
    const cJSON  *file_header, *signature = NULL, *offset = NULL, *reserved = NULL, *file_size = NULL;
    
    // parse the file_header object
    file_header = cJSON_GetObjectItemCaseSensitive(sudoku, "file_header");
    if(cJSON_IsObject(file_header))
    {
        offset = cJSON_GetObjectItemCaseSensitive(file_header,"offset");
        signature = cJSON_GetObjectItemCaseSensitive(file_header,"signature");
        file_size = cJSON_GetObjectItemCaseSensitive(file_header,"file_size");

        if(cJSON_IsNumber(offset) && (offset->valueint != NULL))
        {
            myFileHeader->imageDataOffset = offset->valueint;
            printf("%d\n",myFileHeader->imageDataOffset);
        }
        if(cJSON_IsString(signature) && (signature->valuestring != NULL))
        {
            printf("%s\n", signature->valuestring);
            myFileHeader->fileMarker1 = signature->valuestring[0];
            myFileHeader->fileMarker2 = signature->valuestring[1];
        }
        if(cJSON_IsNumber(file_size) && (file_size->valueint != NULL))
        {
            myFileHeader->bfSize = file_size->valueint;
            printf("%d\n",myFileHeader->bfSize);
        }
        myFileHeader->unused1 = 0;
        myFileHeader->unused2 = 0;
        
    }
}

void ParseInfoHeader(cJSON *sudoku ,bmp_infoheader *myInfoHeader)
{
    bmp_infoheader *myInfoheader = (bmp_infoheader *) calloc(1,sizeof(bmp_infoheader));

    if(myInfoheader == NULL)
        return ;

    const cJSON *info_header, *colors_used, *size, *colors_important, *image_size,
                *y_pixels_per_meter, *x_pixels_per_meter, *width, *planes, *bit_count,
                *compression, *height;
    // parse the info_header object
    info_header = cJSON_GetObjectItemCaseSensitive(sudoku, "info_header");
    if(cJSON_IsObject(info_header))
    {

    }
}

void ParseBitmap(cJSON *sudoku ,Pixel *pixelMatrix)
{
     // allocating memory for the pixelMatrix
    pixelMatrix = (Pixel *) malloc(NUM_OF_PIXELS * NUM_OF_PIXELS * sizeof(Pixel));
    // TODO free the pixelMatrix
    if(pixelMatrix == NULL)
        return;

    const cJSON *bitmap, *bit;
    bitmap = cJSON_GetObjectItemCaseSensitive(sudoku, "bitmap");
    int k = 0, i = 72, j = 0, curr_index;

    // iterating through every number, create the pixel and add it from the buttom up
    cJSON_ArrayForEach(bit,bitmap)
    {
        if(!cJSON_IsNumber(bit))
        {
            printf("Error");
            return;
        }
        else
        {
            curr_index = i * NUM_OF_PIXELS + j;
            if(k == 0)
                pixelMatrix[curr_index].b = bit->valueint;
            else if(k == 1)
                pixelMatrix[curr_index].g = bit->valueint;
            else
                pixelMatrix[curr_index].r = bit->valueint;
            k++;
            if(k == 3)
            {
                k=0;
                j++;
            }
            if(j == NUM_OF_PIXELS)
            {
                i--;
                j=0;
            }
        }
    }
    
    
    /*
    // printing the pixel array
    for(i = 0; i< NUM_OF_PIXELS; ++i)
    {
        for(j = 0; j < NUM_OF_PIXELS; ++j)
        {
            curr_index = i * NUM_OF_PIXELS + j;
            printf("%d %d %d\n", pixelMatrix[curr_index].r, pixelMatrix[curr_index].g, pixelMatrix[curr_index].b);
        }
    }
    */
}