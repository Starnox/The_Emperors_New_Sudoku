#include "tema3.h"

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
    bitmap *myBitmap = NULL;
    myBitmap = ParseJSON(inputString, &pixelMatrix);

    unsigned char *img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap,img);
    MirrorNumbers(myBitmap, &pixelMatrix);
    img = TransformPixelMatrix(pixelMatrix, myBitmap);
    CreateBMP(myBitmap,img);

    free(myBitmap);
    free(pixelMatrix);
    free(img);
    //free(myFileHeader);
    //free(myInfoheader);
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

// parse JSON and return a bitmap pointer
bitmap * ParseJSON(const char * const inputString, Pixel **pixelMatrix)
{
    // declaring
    bitmap *myBitmap = (bitmap *) calloc(1,sizeof(bitmap));
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
        return NULL;
    }

    myFileHeader = ParseFileHeader(sudoku);
    myInfoheader = ParseInfoHeader(sudoku);
    *pixelMatrix = ParseBitmap(sudoku, myInfoheader->width, myInfoheader->height);
    myBitmap->fileheader = *myFileHeader;
    myBitmap->infoheader = *myInfoheader;
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
    int k = 0, i = height-1, j = 0, curr_index;

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
            curr_index = i * height + j;
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
            curr_index = i * height + j;
            printf("%d %d %d\n", pixelMatrix[curr_index].r, pixelMatrix[curr_index].g, pixelMatrix[curr_index].b);
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
            int curr_index = i * height  + j,
                r = pixelMatrix[curr_index].r,
                g = pixelMatrix[curr_index].g,
                b = pixelMatrix[curr_index].b;

            img[i * width * pixelSize + j * pixelSize + 2] = (unsigned char) (r);
            img[i * width * pixelSize + j * pixelSize + 1] = (unsigned char) (g);
            img[i * width * pixelSize + j * pixelSize + 0] = (unsigned char) (b);
        }
    }

    return img;
}

void CreateBMP(bitmap *myBitmap, unsigned char * img)
{
    int width = myBitmap->infoheader.width, height = myBitmap->infoheader.height,
                pixelSize = sizeof(Pixel), i;
    unsigned char pad = '0';
    FILE *fp = fopen("test.bmp", "wb");
    fwrite(myBitmap, 1, sizeof(bitmap), fp);
    
    for(i=0; i < height; ++i)
    {
        // write each line of pixels
        fwrite(img + (i * pixelSize * width), pixelSize, width, fp);
        // add a padding of one byte to respect the format
        fwrite(&pad, sizeof(char), 1, fp);
    }
    
    
    /*
    int bytesPerPixel = sizeof(Pixel);
    int paddedRowSize = (int)(4 * ceil((float)width/4.0f))*bytesPerPixel;
    int unpaddedRowSize = width * bytesPerPixel;
    for(int i = 0; i < height; ++i)
    {
        int pixelOffset = ((height - i) - 1)*unpaddedRowSize;
        fwrite(&pixelMatrix[pixelOffset], 1, paddedRowSize, fp);
    }
    */

    /*
    int i, curr_index,aux;
    for(i = 0; i< height; ++i)
    {
        aux = fwrite(pixelMatrix + (i * 3 * width), sizeof(Pixel), width, fp);
        //fwrite(bmppad, 1, 1, fp);
        aux = fwrite(bmppad, 1, (4-(width * 3)%4)%4, fp); // Padding for each row
    }*/
    
    fclose(fp);
}

int CheckWhitePixel(Pixel *pixel)
{
    if(pixel->b == 255 && pixel->g == 255 && pixel->r == 255)
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
                pixelSize = sizeof(Pixel), i, j, curr_index, k = -1;
    Pixel *currPixel, *toSwap;
    for(i = 0; i < height; ++i)
    {
        for(j =  0; j < width; ++j)
        {
            curr_index = i * height  + j;
            // verify if it is a white pixel (color around the number)
            // and then mirror the pixels in relation with the vertical axis
            currPixel = (*pixelMatrix) + curr_index;

            if(CheckWhitePixel(currPixel) && k == -1)
            {
                k = 3;
                toSwap = (*pixelMatrix) + curr_index + 2 * k;
                SwapPixels(currPixel, toSwap);
                k--;
            } 
            else if(k > 0)
            {
                toSwap = (*pixelMatrix) + curr_index + 2 * k;
                SwapPixels(currPixel, toSwap);
                k--;
            }
            else if(k == 0)
            {
                j+=4;
                k=-1;
            }
            /*
            if(CheckWhitePixel(currPixel) == 1 && k = -1)
            {
                k = 3;
                swapPixels(&((*pixelMatrix)[curr_index]), &((*pixelMatrix)[curr_index+2*k]));
                k-- 
            }*/
        }
    }
}