#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "tema3.h"
#include "bmp_header.h"

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
    ParseJSON(inputString);


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

void ParseJSON(const char * const inputString)
{
    int status = 0;

    bmp_fileheader *myFileHeader = (bmp_fileheader *) calloc(1,sizeof(bmp_fileheader));
    bmp_infoheader *myInfoheader = (bmp_infoheader *) calloc(1,sizeof(bmp_infoheader));
    // TODO free those two

    if(myFileHeader == NULL || myInfoheader == NULL)
        return;
    
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

    const cJSON *signature = NULL, *offset = NULL, *reserved = NULL, *file_size = NULL;
    /*
    signature = cJSON_GetObjectItemCaseSensitive(sudoku, "signature");
    if(cJSON_IsString(signature) && (signature->valuestring != NULL))
    {
        myFileHeader->fileMarker1 = signature->valuestring[0];
        myFileHeader->fileMarker2 = signature->valuestring[1];
    }
    printf("%c\n%c\n", myFileHeader->fileMarker1, myFileHeader->fileMarker1);
    */
    offset = cJSON_GetObjectItemCaseSensitive(sudoku, "signature");
    printf("%d", offset->valueint);
    free(myFileHeader);
    free(myInfoheader);
    return;

}