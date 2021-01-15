# Tema 3 PC. The Emperor's New Sudoku

### Elev: Mihailescu Eduard-Florin
### Grupa: 312CB

#### Filosofie Generala
Am ales sa parsez fisierile JSON cu libraria cJSON (https://github.com/DaveGamble/cJSON) deoarece este "Ultralightweight" 
si are o documentatie foarte clara din care am inteles exact cum trebuie sa o folosesc.

#### Functia main
Functia `main` doar parseaza argumentele de intrare ale programului
si selecteaza in functie de acestea functia dorita

#### Functii de prelucrare JSON
*   `ReadJSON(const char *argument)` - aceasta functie deschide fisierul de input, calculeaza
lungimea fisierului, aloca memorie pentru un string in care se vor incarca datele din fisier
si returneaza un pointer catre acest string
*   `ParseJSON(const char * const inputString, Pixel **pixelMatrix)` - se parseaza tot JSON-ul
incarcat in stringul inputString folosind functia `cJSON_Parse` din libraria cJSON,
dupa care se parseaza pe rand info headerul, file headerul si matricea de pixeli pe care am retinut-o
sub forma unui pointer de tip `Pixel` unde `Pixel` este o structura cu trei atribute(`r,g,b`). Cele doua headere 
sunt compactate intr-o structura de tip `bitmap` care este returnata de functie.
*   `ParseFileHeader(cJSON *sudoku)` - folosind functia din librarie (`cJSON_GetObjectItemCaseSensitive()`)
se obtin pe rand campurile din JSON si sunt salvate intr-o structura de tip `bmp_fileheader`
*   `ParseInfoHeader(cJSON *sudoku)` - asemanatoare cu functia de sus, este creata o variabila de tip
`bmp_infoheader` si este returnata
*   `ParseBitmap(cJSON *sudoku, int width, int height)` - se aloca memorie intr-un spatiu continuu pentru 
matricea de pixeli dupa care se parcurge fiecare numar din JSON-ul primit, se construieste pixelul aferent si se stocheaza.
Functia returneaza un pointer catre zona de memorie unde este stocata matricea
*   `WriteResultJSON(int win, int taskNumber, char *boardNumber)` - creeaza un obiect de tip cJSON, caruia ii sunt
adaugate campurile cerute, formateaza fisierul de ouput si scrie in aceasta obiectul
cJSON cu functia `cJSON_Print()`

#### Functii de prelucrare BMP
*   `TransformPixelMatrix(Pixel *pixelMatrix, bitmap *myBitmap)` - aceasta functie transforma matricea
 de pixeli intr-un format convenabil pentru scrierea in fisierul bmp
*   `CreateBMP(bitmap *myBitmap, unsigned char * img, int taskNumber, char *boardNumber)` -
se formateaza fisierul de output in functie de taskul si board-ul curent, dupa care se scriu
in fisier cu functia `fwrite()` cele doua headere necesare functionarii, iar la final se itereaza
prin fiecare linie de pixeli din matrice si este adaugata in fisier.
*   `ReadBMP(char *inputFile, Pixel **pixelMatrix)` - functia aloca memorie pentru cele doua headere,
le citeste intr-o structa de tip `bitmap`, dupa care citeste fiecare linie de pixeli intr-o matrice
de tip `Pixel` deplasand de fiecare date cursorul bufferului cu o unitate pentru a sari peste bitul
de padding.

#### Functii utilitare
*   `CheckPinkPixel(Pixel *pixel)` - verifica daca pixelul este roz
*   `CheckGreyPixel(Pixel *pixel)` - verifica daca pixelul este gri
*   `CheckBlackPixel(Pixel *pixel)` - verifica daca pixelul este negru
*   `CheckWhitePixel(Pixel *pixel)` - verifica daca pixelul este alb
*   `SwapNumbers(unsigned char *a, unsigned char *b)` - interschimba valorile lui `a` si `b`
*   `SwapPixels(Pixel * pixel1, Pixel * pixel2)` - interschimba cei doi pixeli folosind functia `SwapNumbers()`
*   `MirrorNumbers(bitmap *myBitmap, Pixel **pixelMatrix)` - aplica un algoritm de oglindire pentru
fiecare celula din tabla de joc (oglindirea se face fata de verticala fiecarei celule)
*   `GetLastNumberFromString(char *inputString)` - extrage numarul de la finalul unui string ->
functia este apelata pentru a extrage numarul tablei de joc
*   `ConstructCellsMasks(Pixel *pixelMatrix, bitmap *myBitmap)` - Construieste o masca de tip string
formata din 0 si 1 (0 pentru pixel alb, 1 pentru pixel roz) pentru fiecare celula din tabla de joc
si le returneaza sub forma unui matrice de siruri de caractere
*   `TransformGameboard(char ***gameBoard)` - Transforma matricea de siruri de caractere intr-o matrice 
de numere folosind functia `MaskToNumber()`
*   `MaskToNumber(char *input)` - Transforma masca fiecarei celule intr-un numar. Pentru a compara
aceste masti am declarat constante pe care le-am aflat aplicand algoritmul de transformare de la `TransformGameboard()` 
pe o tabla de joc ce continea toate numerele si le-am extras de acolo.
*   `MaskToNumber(char *input)` - Functia inversa a lui `MaskToNumber()` -> Transforma un numar in masca aferenta
*   `CheckGameBoard(int ** gameBoardNumbers)` - Verifica daca tabla de sudoku este completa si rezolvata corect
*   `SolveSudoku(int **gameBoardNumbers)` - Rezolva tabla de Sudoku intr-o maniera elementara, functie
creata pentru taskul 4
*   `TransformGameBoardIntoPixels(int **toChange, Pixel **pixelMatrix,  bitmap *myBitmap)` - 
modifica pixelii fiecarei celule in care a fost adaugat un numar, urmarind masca aferenta 
(pentru a se realiza acest lucru s-a folosit structura `cell` care retine masca fiecarei celule si indexul curent). 
*   `FreePlayBoard(char ***gameBoard)` - dezaloca memoria pentru matricea de stringuri
*   `FreeGameBoardNumbers(int **gameBoard)` - dezaloca memoria pentur matricea de numere
##### Functii pentru bonus
*   `IsPresentInCol(int ** gameBoardNumbers, int col, int num)` - verifica daca numarul `num` se afla pe coloana `col` 
*   `IsPresentInRow(int ** gameBoardNumbers, int row, int num)` - verifica daca numarul `num` se afla pe linia `row`
*   `IsPresentInBox(int **gameBoardNumbers, int boxStarRow, int boxStartCol, int nu)` - verifica daca numarul `num`
se afla in patratul care incepe de la linia `boxStartRow`  si coloana `boxStartCol`
*   `FindEmptyPlace(int **gameBoardNumbers, int *row, int *col)` - cauta in tabla de joc o casuta goala
*   `IsValidPlace(int **gameBoardNumbers, int row, int col, int num)` - verifica daca casuta marcata de `row` si `col`
 este valida pentru a insera numarul `num`.
*   `SolveSudokuBonus(int **gameBoardNumber, int **toChange)` - rezolvare bazata pe backtracking -> se incearca
 toata modurile de a amplasa numerele pe tabla de joc (returneaza 1 daca jocul poate fi castigat, 0 in caz contrar)
*   `CreateInvalidBoard(Pixel **pixelMatrix, bitmap *myBitmap)` - inseareaza pixelii necesari pentru a creea
forma de X in toate celulele.

#### Functii de rezolvare
*   `Solve123(char *inputFile)` - parseaza fisierul JSON primit in parametrul `inputFile`,
creeaza matricea de pixeli si structura de bitmap, dupa care in functie de cele 3 taskuri,
creeaza imaginea bmp asa cum a fost primita cu functia `CreateBMP()`, apoi da mirror la
numere cu functia `MirrorNumber()` si creeaza din nou o imagine BMP, iar la final,
se transforma imaginea intr-o matrice de numere (`TransformGameboard()`), se verifica
daca tabla de joc e completata corect (`CheckGameBoard()`) si dupa se scrie rezultatul
intr-un fisir JSON (`WriteResultJSON()`);
*   `Solve4(char *inputFile)` - se citeste fisierul bmp cu functia `ReadBMP()`, se transforma
intr-o matrice de numere cu `TransformGameboard()`. se rezolva tabla cu `SolveSudoku()`, dupa
care se creeza o matrice de pixeli noi care sa reflecte tabla completata(`TransformGameBoardIntoPixels`)  si se creeaza
imagine BMP cu noua matrice de pixeli(`CreateBMP()`)
*   `SolveBonus(char *inputFile)` - se procedeaza la fel ca la `Solve4()`, insa de data aceasta
se apeleaza functia `SolveSudokuBonus()` care rezolva tabla de joc folosind metoda backtracking si in
caz de esec se apeleaza functia `CreateInvalidBoard()` care modifica matricea de pixeli astfel incat
sa contina doar X-uri in toate celulele.





