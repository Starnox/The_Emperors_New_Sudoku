#define NUM_OF_PIXELS 73

typedef struct
{
    int r,g,b;
}Pixel;



char * ReadJSON(const char *argument);
void ParseJSON(const char * const inputString, Pixel *pixelMatrix);

