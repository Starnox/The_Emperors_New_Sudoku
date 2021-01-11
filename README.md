# Tema 3 PC. The Emperor's New Sudoku

### Elev: Mihailescu Eduard-Florin
### Grupa: 312CB

#### Filosofie Generala
Am ales sa parsez fisierile JSON cu libraria cJSON (https://github.com/DaveGamble/cJSON) deoarece este "Ultralightweight" si are o documentatie
foarte clara din care am inteles exact cum trebuie sa o folosesc.

#### Functia main
Functia main doar parseaza argumentele de intrare ale programului
si selecteaza in functie de acestea functia dorita

#### Functii de citire si parsare JSON
*   `ReadJSON(const char *argument)` - aceasta functie deschide fisierul de input, calculeaza
    lungimea fisierului, aloca memorie pentru un string in care se vor incarca datele din fisier
    si returneaza un pointer catre acest string
*   `ParseJSON(const char * const inputString, Pixel **pixelMatrix)` - se parseaza tot JSON-ul
    incarcat in stringul inputString folosind functia `cJSON_Parse` din libraria cJSON,
    dupa care se parseaza pe rand info headerul, file headerul si matricea de pixeli pe care am retinut-o
    sub forma unui pointer de tip `Pixel` unde `Pixel` este o structura cu trei atribute(`r,g,b`). Cele doua
    headere sunt compactate intr-o structura de tip `bitmap` care este returnata de functie.
*   `ParseFileHeader(cJSON *sudoku)` - folosind functia din librarie (`cJSON_GetObjectItemCaseSensitive()`)
    se obtin pe rand campurile din JSON si sunt salvate intr-o structura de tip `bmp_fileheader`
*   `ParseInfoHeader(cJSON *sudoku)` - asemanatoare cu functia de sus, este creata o variabila de tip
    `bmp_infoheader` si returnata
*   `ParseBitmap(cJSON *sudoku, int width, int height)` 
