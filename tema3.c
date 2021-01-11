#include "tema3.h"

int main(int argc, char *argv[] )
{
    if(argc != 3)
        printf("Please provide correct input");

    // check what task to solve   
    if(strcmp("123", argv[2]) == 0)
        Solve123(argv[1]);
    else if(strcmp("4", argv[2]) == 0)
        Solve4(argv[1]);
    else
        SolveBonus(argv[1]);
    
    return 0;
}

void Solve123(char *inputFile)
{
    int taskNumber;

    // Read the JSON with the library
    char *inputString = NULL, *boardNumber = NULL ;
    inputString = ReadJSON(inputFile); // read the input string and store it in a variable
    if(inputString == NULL)
    {
        printf("Couldn't read from file");
        return;
    }

    Pixel *pixelMatrix = NULL; // matrix of pixels
    bitmap *myBitmap = NULL; // the bitmap
    myBitmap = ParseJSON(inputString, &pixelMatrix); // create the bitmap

    boardNumber = GetLastNumberFromString(inputFile); // get the board number
    
    // Task 1
    
    taskNumber = 1;

    // transform the pixelMatrix into a char vector in order to write the bmp
    unsigned char *img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap,img, taskNumber, boardNumber); // create and write the file
    free(img); // free the memory for the image

    // Task 2
    taskNumber = 2;
    MirrorNumbers(myBitmap, &pixelMatrix); // apply the mirror algorithm to the pixel matrix
    img = TransformPixelMatrix(pixelMatrix, myBitmap); // transform again
    CreateBMP(myBitmap,img ,taskNumber, boardNumber); // create the bmp file

    // Task 3

    // construct a string mask for every cell in the sudoku and store
    // them in a matrix of strings
    char ***gameBoard = ConstructCellsMasks(pixelMatrix, myBitmap); 

    // transform the matrix of strings into a matrix of integers
    int **gameBoardNumbers = TransformGameboard(gameBoard); 
    taskNumber = 3;

    int result = CheckGameBoard(gameBoardNumbers); // verify if the game is won
    WriteResultJSON(result, taskNumber, boardNumber); // write the result to a json file

    
    // Free memory
    FreePlayBoard(gameBoard);
    FreeGameBoardNumbers(gameBoardNumbers);
    free(myBitmap);
    free(pixelMatrix);
    free(img);
    free(boardNumber);
    free(inputString);
}

void Solve4(char *inputFile)
{
    int taskNumber = 4;
    char *boardNumber = GetLastNumberFromString(inputFile);

    Pixel *pixelMatrix = NULL;

    // read a bmp file and store the headers in the bitmap structre and
    // the pixels in the pixel matrix
    bitmap *myBitmap = ReadBMP(inputFile, &pixelMatrix); 

    // same procedure as in the first 3 tasks
    char ***gameBoard = ConstructCellsMasks(pixelMatrix, myBitmap);
    int **gameBoardNumbers = TransformGameboard(gameBoard);

    int **toChange = SolveSudoku(gameBoardNumbers); // solve the sudoku puzzle


    TransformGameBoardIntoPixels(toChange, &pixelMatrix, myBitmap); // put the new pixels in the board
    unsigned char *img = TransformPixelMatrix(pixelMatrix, myBitmap);

    CreateBMP(myBitmap, img, taskNumber, boardNumber); // create the bmp

    // Free memory
    FreePlayBoard(gameBoard);   
    FreeGameBoardNumbers(gameBoardNumbers);
    FreeGameBoardNumbers(toChange);
    free(boardNumber);
    free(myBitmap);
    free(pixelMatrix);
    free(img);    
}

void SolveBonus(char *inputFile)
{
    
    int taskNumber = 0;
    char *boardNumber = GetLastNumberFromString(inputFile);

    Pixel *pixelMatrix = NULL;

    // read a bmp file and store the headers in the bitmap structre and
    // the pixels in the pixel matrix
    bitmap *myBitmap = ReadBMP(inputFile, &pixelMatrix); 

    char ***gameBoard = ConstructCellsMasks(pixelMatrix, myBitmap);
    int **gameBoardNumbers = TransformGameboard(gameBoard);

    int **toChange = (int **) calloc(CELLS_NUMBER, sizeof(int *));

    for(int i = 0; i< CELLS_NUMBER; ++i)
        toChange[i] = (int *) calloc(CELLS_NUMBER, sizeof(int));

    unsigned char *img;
    if(SolveSudokuBonus(gameBoardNumbers,toChange))
        TransformGameBoardIntoPixels(toChange, &pixelMatrix, myBitmap);
    else
        CreateInvalidBoard(&pixelMatrix, myBitmap);

    img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap, img, taskNumber, boardNumber);

    FreePlayBoard(gameBoard);   
    FreeGameBoardNumbers(gameBoardNumbers);
    FreeGameBoardNumbers(toChange);
    free(myBitmap);
    free(pixelMatrix);
    free(img);
    free(boardNumber);
    
}

char * ReadJSON(const char *argument)
{
    long length;
    char *inputString = NULL;

    FILE *inputFile = fopen(argument, "r"); // open file
    if(inputFile != NULL)
    {
        // get the size of the file
        fseek(inputFile, 0, SEEK_END);
        length = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);
        if(length >= 0)
        {
            inputString = calloc((length+1),sizeof(char));
            if(inputString != NULL)
            {
                fread(inputString, 1, length, inputFile); // read the whole file
            }
            fclose(inputFile);
        }
    }
    return inputString;
}

// parse JSON and return a bitmap pointer
bitmap * ParseJSON(const char * const inputString, Pixel **pixelMatrix)
{
    // declaring
    bitmap *myBitmap = (bitmap *) calloc(1,sizeof(bitmap));
    bmp_fileheader *myFileHeader = NULL;
    bmp_infoheader *myInfoheader = NULL;

    // parse the JSON and verify
    cJSON *sudoku = cJSON_Parse(inputString);
    if(sudoku == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return NULL;
    }

    myFileHeader = ParseFileHeader(sudoku);
    myInfoheader = ParseInfoHeader(sudoku);

    // get the pixelMatrix
    *pixelMatrix = ParseBitmap(sudoku, myInfoheader->width, myInfoheader->height);

    myBitmap->fileheader = *myFileHeader;
    myBitmap->infoheader = *myInfoheader;

    // Free memory
    free(myFileHeader);
    free(myInfoheader);
    cJSON_Delete(sudoku); // free up allocated memory with the built in function
    return myBitmap;

}

bmp_fileheader * ParseFileHeader(cJSON *sudoku)
{
    bmp_fileheader *myFileHeader = (bmp_fileheader *) calloc(1,sizeof(bmp_fileheader));
    if(myFileHeader == NULL)
        return NULL;
    // variables for file_headers
    const cJSON  *file_header, *signature = NULL, *offset = NULL, *file_size = NULL;
    
    // parse the file_header object
    file_header = cJSON_GetObjectItemCaseSensitive(sudoku, "file_header");
    if(cJSON_IsObject(file_header))
    {
        offset = cJSON_GetObjectItemCaseSensitive(file_header,"offset");
        signature = cJSON_GetObjectItemCaseSensitive(file_header,"signature");
        file_size = cJSON_GetObjectItemCaseSensitive(file_header,"file_size");

        // verify and set
        if(cJSON_IsNumber(offset))
            myFileHeader->imageDataOffset = offset->valueint;

        if(cJSON_IsString(signature))
        {
            myFileHeader->fileMarker1 = signature->valuestring[0];
            myFileHeader->fileMarker2 = signature->valuestring[1];
        }
        if(cJSON_IsNumber(file_size))
        {
            myFileHeader->bfSize = file_size->valueint;
        }
        myFileHeader->unused1 = 0;
        myFileHeader->unused2 = 0;
        
    }
    return myFileHeader;
}

bmp_infoheader * ParseInfoHeader(cJSON *sudoku)
{
    bmp_infoheader *myInfoheader = (bmp_infoheader *) calloc(1,sizeof(bmp_infoheader));

    if(myInfoheader == NULL)
        return NULL;

    const cJSON *info_header, *size,*colors_used , *colors_important, *image_size,
                *y_pixels_per_meter, *x_pixels_per_meter, *width, *planes, *bit_count,
                *compression, *height;
    // parse the info_header object
    info_header = cJSON_GetObjectItemCaseSensitive(sudoku, "info_header");
    if(cJSON_IsObject(info_header))
    {
        size = cJSON_GetObjectItemCaseSensitive(info_header,"size");
        width = cJSON_GetObjectItemCaseSensitive(info_header,"width");
        height = cJSON_GetObjectItemCaseSensitive(info_header,"height");
        planes = cJSON_GetObjectItemCaseSensitive(info_header,"planes");
        bit_count = cJSON_GetObjectItemCaseSensitive(info_header,"bit_count");
        compression = cJSON_GetObjectItemCaseSensitive(info_header,"compression");
        image_size = cJSON_GetObjectItemCaseSensitive(info_header,"image_size");
        x_pixels_per_meter = cJSON_GetObjectItemCaseSensitive(info_header,"x_pixels_per_meter");
        y_pixels_per_meter = cJSON_GetObjectItemCaseSensitive(info_header,"y_pixels_per_meter");
        colors_used = cJSON_GetObjectItemCaseSensitive(info_header,"colors_used");
        colors_important = cJSON_GetObjectItemCaseSensitive(info_header,"colors_important");

        // verify and set
        if(cJSON_IsNumber(size))
            myInfoheader->biSize = size->valueint;

        if(cJSON_IsNumber(width))
            myInfoheader->width = width->valueint;

        if(cJSON_IsNumber(height))
            myInfoheader->height = height->valueint;

        if(cJSON_IsNumber(planes))
            myInfoheader->planes = planes->valueint;

        if(cJSON_IsNumber(bit_count))
            myInfoheader->bitPix = bit_count->valueint;

        if(cJSON_IsNumber(compression))
            myInfoheader->biCompression = compression->valueint;

        if(cJSON_IsNumber(image_size))
            myInfoheader->biSizeImage = image_size->valueint;

        if(cJSON_IsNumber(x_pixels_per_meter))
            myInfoheader->biXPelsPerMeter = x_pixels_per_meter->valueint;

        if(cJSON_IsNumber(y_pixels_per_meter))
            myInfoheader->biYPelsPerMeter = y_pixels_per_meter->valueint;

        if(cJSON_IsNumber(colors_used))
            myInfoheader->biClrUsed = colors_used->valueint;
 
        if(cJSON_IsNumber(colors_important))
            myInfoheader->biClrImportant = colors_important->valueint;

    }
    return myInfoheader;
}

Pixel * ParseBitmap(cJSON *sudoku, int width, int height)
{
     // allocating memory for the pixelMatrix
    Pixel *pixelMatrix = (Pixel *) malloc(width * height * sizeof(Pixel));

    if(pixelMatrix == NULL)
        return NULL;

    const cJSON *bitmap, *bit;
    bitmap = cJSON_GetObjectItemCaseSensitive(sudoku, "bitmap");
    int k = 0, i = height-1, j = 0, currIndex;

    // iterating through every number, create the pixel and add it from the buttom up
    cJSON_ArrayForEach(bit,bitmap)
    {
        if(!cJSON_IsNumber(bit))
        {
            printf("Error");
            return NULL;
        }
        else
        {
            currIndex = i * height + j;
            // the order in bmp file is BGR
            if(k == 0)
                pixelMatrix[currIndex].b = bit->valueint;
            else if(k == 1)
                pixelMatrix[currIndex].g = bit->valueint;
            else
                pixelMatrix[currIndex].r = bit->valueint;
            k++;
            if(k == 3)
            {
                k=0;
                j++;
            }
            if(j == width)
            {
                i--;
                j=0;
            }
        }
    }
    

    return pixelMatrix;
}

unsigned char *TransformPixelMatrix(Pixel *pixelMatrix, bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                pixelSize = sizeof(Pixel), i, j;
    unsigned char *img = malloc(pixelSize * width * height); // continuos space allocated
    
    // create a new matrix of unsigned char with all the pixels
    for(i=0; i< height; ++i)
    {
        for(j=0; j< width; ++j)
        {
            int currIndex = i * height  + j,
                r = pixelMatrix[currIndex].r,
                g = pixelMatrix[currIndex].g,
                b = pixelMatrix[currIndex].b;

            // put the bytes in the correct positions
            img[i * width * pixelSize + j * pixelSize + 2] = (unsigned char) (r);
            img[i * width * pixelSize + j * pixelSize + 1] = (unsigned char) (g);
            img[i * width * pixelSize + j * pixelSize + 0] = (unsigned char) (b);
        }
    }

    return img;
}

void CreateBMP(bitmap *myBitmap, unsigned char * img, int taskNumber, char *boardNumber)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                pixelSize = sizeof(Pixel), i;
    unsigned char pad = '\0';

    char *destinationFile = malloc(100 * sizeof(char));

    // construct the output file in the specified format
    if(taskNumber != 0)
        sprintf(destinationFile,"output_task%d_board%s.bmp", taskNumber, boardNumber);
    else
        sprintf(destinationFile, "output_bonus_board%s.bmp", boardNumber);

    // open the file in write bytes mode
    FILE *fp = fopen(destinationFile, "wb");

    fwrite(myBitmap, 1, sizeof(bitmap), fp); // write the two headers to the file
    
    // go to each line of pixels
    for(i=0; i < height; ++i)
    {
        // write each line of pixels
        fwrite(img + (i * pixelSize * width), pixelSize, width, fp);
        // add a padding of one byte to respect the format
        fwrite(&pad, sizeof(char), 1, fp);
    }
    fclose(fp);
    free(destinationFile);
}

int CheckWhitePixel(Pixel *pixel)
{
    if(pixel->b == 255 && pixel->g == 255 && pixel->r == 255)
        return 1;
    return 0;
}

int CheckPinkPixel(Pixel *pixel)
{
    if(pixel->b == 175 && pixel->g == 175 && pixel->r == 255)
        return 1;
    return 0;
}
int CheckGreyPixel(Pixel *pixel)
{
    if(pixel->b == 128 && pixel->g == 128 && pixel->r == 128)
        return 1;
    return 0;
}
int CheckBlackPixel(Pixel *pixel)
{
    if(pixel->b == 0 && pixel->g == 0 && pixel->r == 0)
        return 1;
    return 0;
}

void SwapNumbers(unsigned char *a, unsigned char *b)
{
    unsigned char aux = *a;
    *a = *b;
    *b = aux;
}

void SwapPixels(Pixel * pixel1, Pixel * pixel2)
{
    SwapNumbers(&pixel1->b, &pixel2->b);
    SwapNumbers(&pixel1->g, &pixel2->g);
    SwapNumbers(&pixel1->r, &pixel2->r);
}


void MirrorNumbers(bitmap *myBitmap, Pixel **pixelMatrix)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                i, j, currIndex, k = -1;
    Pixel *currPixel, *toSwap;
    // iterate through every pixel
    for(i = 0; i < height; ++i)
    {
        for(j =  0; j < width; ++j)
        {
            currIndex = i * height  + j;
            // verify if it is a white pixel (color around the number)
            // and then mirror the pixels in relation with the vertical axis
            currPixel = (*pixelMatrix) + currIndex;

            if(CheckWhitePixel(currPixel) && k == -1)
            {
                k = 3;
                toSwap = (*pixelMatrix) + currIndex + 2 * k;
                SwapPixels(currPixel, toSwap);
                k--;
            } 
            else if(k > 0)
            {
                toSwap = (*pixelMatrix) + currIndex + 2 * k;
                SwapPixels(currPixel, toSwap);
                k--;
            }
            else if(k == 0)
            {
                j+=4; // skip over the just swapped pixels and continue
                k=-1; // mark that we finished swapping 
            }
        }
    }
}

char * GetLastNumberFromString(char *inputString)
{
    int length = strlen(inputString), i, found = 0;
    char *result = malloc(length * sizeof(char));
    strcpy(result,"");
    result[0] = '\0';
    for(i = length -1; i>= 0; i--)
    {
        // verify if the character is a digit
        if(inputString[i] >= '0' && inputString[i] <= '9')
        {
            found = 1;
            // construct the number
            strncat(result, inputString + i, 1);
        }
        else if(found == 1)
            break;
    }

    // reverse the string
    int resLength = strlen(result);
    for(i = 0; i < resLength / 2; ++i)
    {
        char aux = result[i];
        result[i] = result[resLength - i -1];
        result[resLength - i - 1] = aux; 
    }
    return result;
}

/* Construct a string mask for every cell in the sudoku game board
    0 - for a white pixel
    1 - for a pink pixel
*/
char *** ConstructCellsMasks(Pixel *pixelMatrix, bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                i, j, currIndex;

    // this variable stores the masks for every cell
    // allocate the memory
    char ***gameBoard = (char ***) calloc(CELLS_NUMBER,sizeof(char **));
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        gameBoard[i] = (char **) calloc(CELLS_NUMBER, sizeof(char *));
        for(j =0; j < CELLS_NUMBER; ++j)
        {
            gameBoard[i][j] = (char *) calloc(NUM_OF_PIXELS_IN_CELL+1, sizeof(char));
        }
    }
    Pixel *currPixel;
    

    int len;
    int x, y; // x = 0->9 , y = 0->9 marking one of cells
    // iterate through every pixel and see in which cell it is situated
    for(i = 0; i < height; ++i)
    {
        if(i % 8 == 0) // if it is a delimiter line
            continue;
        y = i / (CELLS_NUMBER-1);
        for(j = 0; j < width-1; ++j)
        {
            x = j / (CELLS_NUMBER-1);
            currIndex = i * height  + j;
            currPixel = pixelMatrix + currIndex;
            len = strlen(gameBoard[y][x]);
            if(CheckWhitePixel(currPixel)) // if it is a white pixel
            {
                gameBoard[y][x][len] = '0';
                gameBoard[y][x][len+1] = '\0';
            }
            else if(CheckPinkPixel(currPixel))
            {
                gameBoard[y][x][len] = '1';
                gameBoard[y][x][len+1] = '\0';
            }
        }
    }
    return gameBoard;
}

// Free The memery allocated for the gameBoard
void FreePlayBoard(char ***gameBoard)
{
    int i,j;
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j = 0; j< CELLS_NUMBER; ++j)
            free(gameBoard[i][j]);
        free(gameBoard[i]);
    }
    free(gameBoard);
}

void FreeGameBoardNumbers(int **gameBoard)
{
    int i;
    for(i = 0; i< CELLS_NUMBER; ++i)
    {
        free(gameBoard[i]);
    }
    free(gameBoard);
}

// check every mask and return the corresponding number if found
// else return -1
int MaskToNumber(char *input)
{
    if(strcmp(input, MASKNOTHING) == 0)
        return 0;
    if(strcmp(input,MASK1) == 0)
        return 1;
    if(strcmp(input,MASK2) == 0)
        return 2;
    if(strcmp(input,MASK3) == 0)
        return 3;
    if(strcmp(input,MASK4) == 0)
        return 4;
    if(strcmp(input,MASK5) == 0)
        return 5;
    if(strcmp(input,MASK6) == 0)
        return 6;
    if(strcmp(input,MASK7) == 0)
        return 7;
    if(strcmp(input,MASK8) == 0)
        return 8;
    if(strcmp(input,MASK9) == 0)
        return 9;
    return -1;
}

char * NumberToMask(int number)
{
    switch (number)
    {
    case 1:
        return MASK1;
        break;
    case 2:
        return MASK2;
        break;
    case 3:
        return MASK3;
        break;
    case 4:
        return MASK4;
        break;
    case 5:
        return MASK5;
        break;
    case 6:
        return MASK6;
        break;
    case 7:
        return MASK7;
        break;
    case 8:
        return MASK8;
        break;
    case 9:
        return MASK9;
        break;
    
    default:
        return NULL;
        break;
    }
}

int ** TransformGameboard(char ***gameBoard)
{
    int i,j;
    int ** result = (int **) calloc(CELLS_NUMBER, sizeof(int *));
    for(i = 0; i < 9; ++i)
    {
        result[i] = (int *) calloc(CELLS_NUMBER, sizeof(int));
    }
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j =0; j< CELLS_NUMBER; ++j)
        {
            result[i][j] = MaskToNumber(gameBoard[i][j]);
        }
    }

    return result;
}

int CheckGameBoard(int ** gameBoardNumbers)
{
    // frequency vector
    int *fv = (int *) calloc(CELLS_NUMBER+1, sizeof(int));

    int i, j, x, y;

    // Check for the 3 condition
    // Check every line
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j =0; j< CELLS_NUMBER; ++j)
        {
            if(gameBoardNumbers[i][j] == -1) // if the cell doesn't have a number in it
            {
                free(fv);
                return 0;
            }
            if(fv[gameBoardNumbers[i][j]] != 0)
            {
                free(fv);
                return 0;
            }
            fv[gameBoardNumbers[i][j]]++;
        }
        // check if the line has every number
        for(j = 1; j <= CELLS_NUMBER; ++j)
        {
            if(fv[j] != 1)
            {
                free(fv);
                return 0; // the game is lost
            }
            else
                fv[j] = 0; // reset it;
        }
    }
    // Check every column
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j =0; j< CELLS_NUMBER; ++j)
        {
            if(gameBoardNumbers[i][j] == -1)
            {
                free(fv);
                return 0;
            }
            if(fv[gameBoardNumbers[j][i]] != 0)
            {
                free(fv);
                return 0;
            }
            fv[gameBoardNumbers[j][i]]++;
        }

        for(j = 1; j <= CELLS_NUMBER; ++j)
        {
            if(fv[j] != 1)
            {
                free(fv);
                return 0;
            }
            else
                fv[j] = 0; // reset it;
        }
    }

    // Check every square
    // position on the first cell of every square
    for(i = 0; i < CELLS_NUMBER; i+= 3)
    {
        for(j = 0; j < CELLS_NUMBER; j+= 3)
        {
            for(x = i; x < i + 3; ++x)
            {
                for(y = j; y < j + 3; ++y)
                {
                    if(fv[gameBoardNumbers[x][y]] != 0)
                    {
                        free(fv);
                        return 0;
                    }
                    fv[gameBoardNumbers[x][y]]++;
                }
            }
            for(x = 1; x <= CELLS_NUMBER; ++x)
            {
                if(fv[x] != 1)
                {
                    free(fv);
                    return 0;
                }
                else
                    fv[x] = 0; // reset it;
            }
            
        }
    }
    free(fv);
    return 1;
}

void WriteResultJSON(int win, int taskNumber, char *boardNumber)
{
    cJSON *input_file = NULL;
    cJSON *game_state = NULL;
    char *inputFile = (char *) calloc(30, sizeof(char)),
            *gameState = (char *) calloc(30, sizeof(char)),
            *outputFile = (char *) calloc(30, sizeof(char)),
            *string = NULL;

    // format the input and output files        
    sprintf(inputFile,"board%s.json",boardNumber);
    sprintf(outputFile,"output_task%d_board%s.json",taskNumber, boardNumber);

    if(win == 1)
        strcpy(gameState, "Win!");
    else
        strcpy(gameState, "Loss :(");
    

    // Create a cJSON object and add the tags to it
    cJSON *result = cJSON_CreateObject();
    if(result == NULL)
        return;

    input_file = cJSON_CreateString(inputFile);
    if(input_file == NULL)
        return;
    cJSON_AddItemToObject(result,"input_file", input_file);

    game_state = cJSON_CreateString(gameState);
    if(game_state == NULL)
        return;
    cJSON_AddItemToObject(result,"game_state", game_state);

    FILE *fp = fopen(outputFile,"wb"); // open the output file

    string = cJSON_Print(result); // create a string from the JSON just created
    fprintf(fp, "%s\n", string); // write the string to the file

    cJSON_Delete(result);
    free(string);
    free(inputFile);
    free(outputFile);
    free(gameState);
    fclose(fp);
}

bitmap * ReadBMP(char *inputFile, Pixel **pixelMatrix)
{
    bitmap *myBitmap = (bitmap *) calloc(1, sizeof(bitmap));
    bmp_fileheader myFileHeader;
    bmp_infoheader myInfoheader;

    FILE *fp = fopen(inputFile, "rb"); // file pointer
    if(fp == NULL)
    {
        free(myBitmap);
        return NULL;
    }

    // read the fileHeader
    fread(&myFileHeader, sizeof(bmp_fileheader), 1, fp);
    if(myFileHeader.fileMarker1 != 'B') // If it is not a bmp file
    {
        fclose(fp);
        return NULL;
    }
    // read the infoHeader
    fread(&myInfoheader, sizeof(bmp_infoheader), 1, fp);

    myBitmap->fileheader = myFileHeader;
    myBitmap->infoheader = myInfoheader;
    
    // allocate memory for the pixel matrix
    (*pixelMatrix) = (Pixel *) calloc(myInfoheader.width * myInfoheader.height + 1, sizeof(Pixel));

    int unpaddedRowSize = myInfoheader.width;

    fread((*pixelMatrix), sizeof(Pixel) , unpaddedRowSize, fp); // read the first line of pixels

    for(int i = 1; i< myInfoheader.height; ++i)
    {
        fseek(fp, 1 , SEEK_CUR); // offset the the file pointer by 1 byte 
        fread((*pixelMatrix) + i * myInfoheader.width, sizeof(Pixel) , unpaddedRowSize, fp); // read next line
    }
    fclose(fp);

    return myBitmap;
}

int ** SolveSudoku(int **gameBoardNumbers)
{
    int i, j, digit;
    int **fvLine = (int **) calloc(CELLS_NUMBER+1, sizeof(int *)),
        **fvCol = (int **) calloc(CELLS_NUMBER+1, sizeof(int *)),
        **toChange = (int **) calloc(CELLS_NUMBER, sizeof(int *));

    if(fvLine == NULL || fvCol == NULL)
        return NULL;

    for(i = 0; i < CELLS_NUMBER + 1; ++i)
    {
        fvLine[i] = (int *) calloc(CELLS_NUMBER + 1, sizeof(int));
        fvCol[i] = (int *) calloc(CELLS_NUMBER + 1, sizeof(int));
        if(i != CELLS_NUMBER)
            toChange[i] = (int *) calloc(CELLS_NUMBER, sizeof(int));
    }
    

    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j = 0; j < CELLS_NUMBER; ++j)
        {
            // complete the frequency vectors for every line and every column
            fvLine[i][gameBoardNumbers[i][j]]++;
            fvCol[j][gameBoardNumbers[i][j]]++;
        }       
    }

    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j = 0; j < CELLS_NUMBER; ++j)
        {
            // check for every digit
            for(digit = 1; digit <= CELLS_NUMBER; ++digit)
            {
                if(fvLine[i][digit] == 0 && fvCol[j][digit] == 0 && gameBoardNumbers[i][j] == 0)
                {
                    // we need to add a new number
                    gameBoardNumbers[i][j] = digit;
                    toChange[i][j] = digit;
                    fvLine[i][digit]++;
                    fvCol[j][digit]++;
                }
            }
        }
    }

    for(i = 0; i < CELLS_NUMBER + 1; ++i)
    {
        free(fvLine[i]);
        free(fvCol[i]);
    }
    free(fvLine);
    free(fvCol);

    return toChange;
}

void TransformGameBoardIntoPixels(int **toChange, Pixel **pixelMatrix,  bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                i, j, currIndex;

    // matrix of cells where the cell holds the mask for the number and the current pixel
    cell **cellsToChange = (cell **) calloc(CELLS_NUMBER, sizeof(cell *));
    if(cellsToChange == NULL)
        return;

    for(i = 0; i< CELLS_NUMBER; ++i)
    {
        // alloc memory
        cellsToChange[i] = (cell *) calloc(CELLS_NUMBER, sizeof(cell));
    }
    
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j = 0; j < CELLS_NUMBER; ++j)
        {
            if(toChange[i][j] != 0)
            {
                // initialise every cell that has to be added to the board
                cellsToChange[i][j].currIndex = 0; // set the currPixel to 0 -> the 
                cellsToChange[i][j].mask = NumberToMask(toChange[i][j]);
            }
        }
    }

    Pixel *currPixel;

    int x, y, currIndexHash;
    char *mask;
    // iterate through every pixel
    for(i = 0; i < height; ++i)
    {
        if(i % 8 == 0) // if it is a delimiter line
            continue;
        y = i / (CELLS_NUMBER-1);

        for(j = 0; j < width-1; ++j)
        {
            x = j / (CELLS_NUMBER-1);
            currIndex = i * height  + j;
            currPixel = (*pixelMatrix) + currIndex; // get the curr pixel

            if(CheckGreyPixel(currPixel) || CheckBlackPixel(currPixel))
                continue;

            if(toChange[y][x] != 0) // if we are located on a cell that we need to modify
            {
                mask = cellsToChange[y][x].mask;
                currIndexHash = cellsToChange[y][x].currIndex;

                if(mask[currIndexHash] == '1') // the pixel is colored
                {
                    // set the pixel to magenta
                    currPixel->b = 255;
                    currPixel->g = 0;
                    currPixel->r = 255;
                }
                // increment the index
                cellsToChange[y][x].currIndex++;
            }
        }
    }
    for(i = 0; i< CELLS_NUMBER; ++i)
    {
        free(cellsToChange[i]);
    }
    free(cellsToChange);
}

// Bonus Sudoku Solver

int IsPresentInCol(int ** gameBoardNumbers, int col, int num)
{
    for(int row = 0; row < CELLS_NUMBER; ++row)
        if(gameBoardNumbers[row][col] == num)
            return 1;
    return 0;
}

int IsPresentInRow(int ** gameBoardNumbers, int row, int num)
{
    for(int col = 0; col < CELLS_NUMBER; ++col)
        if(gameBoardNumbers[row][col] == num)
            return 1;
    return 0;
}

int IsPresentInBox(int **gameBoardNumbers, int boxStartRow, int boxStartCol, int num)
{
    for(int row = 0; row < 3; ++row)
        for(int col = 0; col < 3; ++col)
            if(gameBoardNumbers[row + boxStartRow][col + boxStartCol] == num)
                return 1;
    return 0;
}

int FindEmptyPlace(int **gameBoardNumbers, int *row, int *col)
{
    for(*row = 0; *row < CELLS_NUMBER; (*row)++)
        for(*col = 0; *col < CELLS_NUMBER; (*col)++)
            if(gameBoardNumbers[*row][*col] == 0)
                return 1;
    return 0;
}

int IsValidPlace(int **gameBoardNumbers, int row, int col, int num)
{
    return !IsPresentInRow(gameBoardNumbers, row, num) && !IsPresentInCol(gameBoardNumbers, col, num)
            && !IsPresentInBox(gameBoardNumbers ,row - row % 3, col - col%3, num);
}

int SolveSudokuBonus(int **gameBoardNumber, int **toChange)
{
    int row, col, num;
    if(!FindEmptyPlace(gameBoardNumber,&row, &col))
        return 1; // all cells are filled
    
    for(num = 1; num <= 9; num++)
    {
        if(IsValidPlace(gameBoardNumber, row, col, num))
        {
            gameBoardNumber[row][col] = num;
            toChange[row][col] = num;
            if(SolveSudokuBonus(gameBoardNumber, toChange))
                return 1;
            gameBoardNumber[row][col] = 0; // the conditions are not satisfied
            toChange[row][col] = 0;
        }
    }
    return 0;
}

void CreateInvalidBoard(Pixel **pixelMatrix, bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                i, j, currIndex;

    // matrix of cells where the cell holds the mask for the X and the current pixel
    cell **cellsToChange = (cell **) calloc(CELLS_NUMBER, sizeof(cell *));
    if(cellsToChange == NULL)
        return;

    for(i = 0; i< CELLS_NUMBER; ++i)
    {
        // alloc memory
        cellsToChange[i] = (cell *) calloc(CELLS_NUMBER, sizeof(cell));
    }

    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j = 0; j < CELLS_NUMBER; ++j)
        {
            // initialise every cell with X
            cellsToChange[i][j].currIndex = 0; // set the currPixel to 0 -> the 
            cellsToChange[i][j].mask = MASKX;
        }
    }
    Pixel *currPixel;

    int x, y, currIndexHash;
    char *mask = MASKX;
    // iterate through every pixel
    for(i = 0; i < height; ++i)
    {
        if(i % 8 == 0) // if it is a delimiter line
            continue;
        y = i / (CELLS_NUMBER-1);

        for(j = 0; j < width-1; ++j)
        {
            x = j / (CELLS_NUMBER-1);
            currIndex = i * height  + j;
            currPixel = (*pixelMatrix) + currIndex; // get the curr pixel

            if(CheckGreyPixel(currPixel) || CheckBlackPixel(currPixel))
                continue;

            currIndexHash = cellsToChange[y][x].currIndex;
            if(mask[currIndexHash] == '1') // the pixel is colored
            {
                // set the pixel to red
                currPixel->b = 0;
                currPixel->g = 0;
                currPixel->r = 255;
            }
            else
            {
                // set the pixel to white
                currPixel->b = 255;
                currPixel->g = 255;
                currPixel->r = 255;
            }
            
            // increment the index
            cellsToChange[y][x].currIndex++;
        }
    }
    for(i = 0; i< CELLS_NUMBER; ++i)
    {
        free(cellsToChange[i]);
    }
    free(cellsToChange);
}