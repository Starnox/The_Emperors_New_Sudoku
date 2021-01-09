#include "tema3.h"

int main(int argc, char *argv[] )
{
    if(argc != 3)
        printf("Please provide correct input");
    
    if(strcmp("123", argv[2]) == 0)
        Solve123(argv[1]);
    else if(strcmp("4", argv[2]) == 0)
        Solve4();
    else
        SolveBonus();
    
    return 0;
}

void Solve123(char *inputFile)
{
    int taskNumber;

    // Read the JSON with the library
    char *inputString = NULL, *boardNumber = NULL ;
    inputString = ReadJSON(inputFile);
    if(inputString == NULL)
    {
        printf("Couldn't read from file");
        return;
    }

    Pixel *pixelMatrix = NULL;
    bitmap *myBitmap = NULL;
    myBitmap = ParseJSON(inputString, &pixelMatrix); // create the bitmap

    boardNumber = GetLastNumberFromString(inputFile); // get the board number
    
    // Task 1
    // transform the pixelMatrix into a char vector in order to write the bmp
    taskNumber = 1;
    unsigned char *img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap,img, taskNumber, boardNumber); // create and write the file
    free(img);

    // Task 2
    taskNumber = 2;
    MirrorNumbers(myBitmap, &pixelMatrix);
    img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap,img ,taskNumber, boardNumber);

    // Task 3
    char ***gameBoard = ConstructCellsMask(pixelMatrix, myBitmap);
    int **gameBoardNumbers = TransformGameboard(gameBoard);
    taskNumber=3;

    int result = CheckGameBoard(gameBoardNumbers);
    WriteResultJSON(result, taskNumber, boardNumber);
    // TODO Free those 2

    
    
    FreePlayBoard(gameBoard);
    FreeGameBoardNumbers(gameBoardNumbers);
    // Free memory
    free(myBitmap);
    free(pixelMatrix);
    free(img);
    free(boardNumber);
    free(inputString);
}

void Solve4()
{

}

void SolveBonus()
{

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
            inputString = calloc((length+1),sizeof(char));
            if(inputString != NULL)
            {
                fread(inputString, 1, length, inputFile);
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
    // TODO free those two

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
    *pixelMatrix = ParseBitmap(sudoku, myInfoheader->width, myInfoheader->height);
    myBitmap->fileheader = *myFileHeader;
    myBitmap->infoheader = *myInfoheader;

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
    // TODO free the pixelMatrix
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
    
    /*
    // printing the pixel array
    for(i = 0; i< height; ++i)
    {
        for(j = 0; j < width; ++j)
        {
            currIndex = i * height + j;
            printf("%d %d %d\n", pixelMatrix[currIndex].r, pixelMatrix[currIndex].g, pixelMatrix[currIndex].b);
        }
    }
    */
    return pixelMatrix;
}

unsigned char *TransformPixelMatrix(Pixel *pixelMatrix, bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                pixelSize = sizeof(Pixel), i, j;
    unsigned char *img = malloc(pixelSize * width * height);
    
    // create a new matrix of unsigned char with all the pixels
    for(i=0; i< height; ++i)
    {
        for(j=0; j< width; ++j)
        {
            int currIndex = i * height  + j,
                r = pixelMatrix[currIndex].r,
                g = pixelMatrix[currIndex].g,
                b = pixelMatrix[currIndex].b;

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
    sprintf(destinationFile,"output_task%d_board%s.bmp", taskNumber, boardNumber);

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

void SwapNumbers(uint8_t *a, uint8_t *b)
{
    int aux = *a;
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
char *** ConstructCellsMask(Pixel *pixelMatrix, bitmap *myBitmap)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                i, j, currIndex;

    // this variable stores the bitmasks for every cell
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
                //currIndexBoard = y * CELLS_NUMBER + x * NUM_OF_PIXELS_IN_CELL; 
                //strncat(gameBoard + currIndexBoard, )
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
int CellToNumber(char *input)
{
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
            result[i][j] = CellToNumber(gameBoard[i][j]);
        }
    }

    return result;
}

int CheckGameBoard(int ** gameBoardNumbers)
{
    // Check for the 3 condition
    int *fv = (int *) calloc(CELLS_NUMBER+1, sizeof(int));

    int i, j, x, y;
    // Check every line
    for(i = 0; i < CELLS_NUMBER; ++i)
    {
        for(j =0; j< CELLS_NUMBER; ++j)
        {
            if(gameBoardNumbers[i][j] == -1)
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
        // check if every the line has every number
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
    sprintf(inputFile,"board%s.json",boardNumber);
    sprintf(outputFile,"output_task%d_board%s.json",taskNumber, boardNumber);
    if(win == 1)
        strcpy(gameState, "Win!");
    else
        strcpy(gameState, "Loss :(");
    

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
    FILE *fp = fopen(outputFile,"wb");
    string = cJSON_Print(result);
    fprintf(fp, "%s\n", string);

    cJSON_Delete(result);
    free(string);
    free(inputFile);
    free(outputFile);
    free(gameState);
    fclose(fp);
}