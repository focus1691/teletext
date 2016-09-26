#ifndef _TELETEXT_H_INCLUDED
#define _TELETEXT_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define BASENUMBER 2
#define BASEASCII 128
#define SPACE 32
#define MAXIMUM 1000
#define MAXNAME 128

/* Hex control codes */
#define RA 0x81
#define GA 0x82
#define YA 0x83
#define BA 0x84
#define MA 0x85
#define CA 0x86
#define WA 0x87
#define SH 0x8C
#define DH 0x8D
#define RG 0x91
#define GG 0x92
#define YG 0x93
#define BG 0x94
#define MG 0x95
#define CG 0x96
#define WG 0x97
#define CONT_G 0x99
#define SEP_G 0x9A
#define BL_BACK 0x9C
#define NEW_BACK 0x9D
#define HOLD_G 0x9E
#define REL_G 0x9F
#define SINGLE_H 0x8C
#define DOUBLE_H 0x8D

enum COLOURS { white = 16, red = 17, green = 18, yellow = 19, darkblue = 20, purple = 21, lightblue = 22 };

enum MODE { SINGLE = 140, DOUBLE = 141, HOLD = 158,
            RELEASE = 159, CONTIGUOUS = 153, SEPARATE = 154,
            ALPHA = 1, CONTI = 2, SEPARA = 3 };

enum CHARACTER { A0 = 160, A1 = 161, BF = 191, C0 = 192, DF = 223,
                 DA = 218, E0 = 224, FA = 250, FF = 255, x81 = 129,
                 x87 = 135, x91 = 145, x97 = 151, x9C = 156, x9D = 157 };

enum buttons { ALNUM = 15, GRAPH = 16, SEPA = 4, CONT = 9, SING_DOUB_SAVE = 18,
               HOLD_RELEASE_G = 22, BACKGROUND = 20 }; /* The row number for these buttons in keyboard mode */

enum sizes { TELETEXTWIDTH = 40, TELETEXTHEIGHT = 25, TEXTWIDTH = 16,
             TEXTHEIGHT = 18, CONTIGUOUSWIDTH = 8, CONTIGUOUSHEIGHT = 6,
             SEPARATEWIDTH = 6, SEPARATEHEIGHT = 4, SPLITSCREEN = 1280,
             WINDOWHEIGHT = 450, SINGLESIZE = 16, DOUBLESIZE = 16,
             HALFSCREEN = 640, BLOCKWIDTH = 2, BLOCKHEIGHT = 3 }; /* Various heights and widths used in the program */

enum boundaries { LEFTBOUNDARY = 0, SEPA_RIGHTBOUNDARY = 10, CONTIG_RIGHTBOUNDARY = 12,
                  SINGLEH_RIGHTBOUNDARY = 9, DOUBLEH_LEFTBOUNDARY = 12, DOUBLEH_RIGHTBOUNDARY = 21,
                  SAVE_LEFTBOUNDARY = 24, SAVE_RIGHTBOUNDARY = 31, ALPHA_LEFTBOUNDARY = 16,
                  ALPHA_RIGHTBOUNDARY = 22, GRAPH_LEFTBOUNDARY = 16, GRAPH_RIGHTBOUNDARY = 22,
                  HOLD_RIGHTBOUNDARY = 9, RELEASE_LEFTBOUNDARY = 12, RELEASE_RIGHTBOUNDARY = 22,
                  NEWBACK_RIGHTBOUNDARY = 9, BLACK_LEFTBOUNDARY = 12, BLACK_RIGHTBOUNDARY = 21 }; /* Boundaries for the keyboard buttons */

static const char MAINFONT[] = "teletextFont.ttf";
static const char TOPHALF[] = "tophalf.ttf";
static const char BOTTOMHALF[] = "bottomhalf.ttf";

TTF_Font* pMAINFONT;
TTF_Font* pTOPHALF;
TTF_Font* pBOTTOMHALF;

static const SDL_Color RED = {255,0,0,1}, GREEN = {0,255,0,0}, YELLOW = {255,255,0,1}, BLUE = {0,0,255,1},
                MAGENTA = {255,0,255,1}, CYAN = {0,255,255,1}, WHITE = {255,255,255,1}, BLACK = {0,0,0,0},
				CURSOR_COLOR = {127, 127, 127, 1};

typedef struct display display;
struct display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Surface* surface;
    SDL_Texture* texture;
};

typedef struct controlCodes controlCodes;
struct controlCodes {
    int hold,
        mode,
        table, /* 1 = alphanumeric, 2 = contiguous, 3 = separated */
        mostRecentGraphic,
        texW,
        texH,
        textheight,
        textsize,
        checkdouble,
        checksingle,
		positionX,
		positionY;
    SDL_Color bgcolor,
              lcolor,
              gpcolor;
    int checkMapDouble[TELETEXTHEIGHT][TELETEXTWIDTH];
    int hexcode[TELETEXTHEIGHT][TELETEXTWIDTH];
    int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH];
    SDL_Rect rectangle_unitContiguous;
    SDL_Rect rectangle_unitSeparate;
};

void drawGraphicBlock(display* dis, controlCodes* CC);
void DrawLetter(display* dis, controlCodes* CC, char str[1], int i, int j, SDL_Color color);
void DrawRectangle(display* dis, controlCodes* CC, int i, int j, SDL_Color color);
int specialLetter(int code);
int isLetterAlphanum(int code);
int isLetter(int code);
SDL_Color getColor(int incode);
int isChangeColor(int code);
int isGraphicBlock(int code);
void getGraphicBlock(int gpCode, int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]);
void fillInCell(int cellposition, int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]);
void initgraphicBlock(int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]);
void instructions();
void createDefaultPage(controlCodes* CC);
void readfile(char *file_name, controlCodes* CC);
void initTeletext(controlCodes* CC);
void graphicTable(display* dis, controlCodes* CC, int i, int j);
void alphanumericTable(display* dis, controlCodes* CC, int i, int j);
void checkContiguousGraphic(display* dis, controlCodes* CC, int i, int j);
void checkChangeColor(display* dis, controlCodes* CC, int i, int j);
void checkHoldMode(display* dis, controlCodes* CC, int i, int j);
void checkHeight(display* dis, controlCodes* CC, int i, int j);
void setDefault(controlCodes* CC);
void createDisplay(display* dis);
void graphicProcess(display* dis, controlCodes* CC, int i, int j);
void letterProcess(display* dis, controlCodes* CC, int i, int j);
void checkAlphaSelection(int* column, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkBlockSelection(int* column, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkSingDoubSaveSelection(int* column, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkBackgroundSelection(int* col, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkHoldReleaseSelection(int* col, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkContiguousSelection(int* col, controlCodes* CC, int* pCursorX, int* pCursorY);
void checkSeparatedSelection(int* col, controlCodes* CC, int* pCursorX, int* pCursorY);
void handleEvent(display* dis, controlCodes* CC);
void checkCursor(int* x, int* y);
void initAndStart(display* dis, controlCodes* CC);
void printCodeName(int code);
void fillKeyboard(controlCodes* keyboard);
void addRealeaseGraphicsKeys(int* ptr, int* currentPos);
void addBackgroundKeys(int* ptr, int* currentPos);
void addBlockColourKeys(int* ptr, int* currentPos);
void addHeightSelectionKeys(int* ptr, int* currentPos);
void addAlnumKeys(int* ptr, int* currentPos);
void addSepModeHeader(int* ptr, int* currentPos);
void addSepModeGraphicsKeys(int* ptr, int* currentPos);
void addAlnumColourKeys(int* ptr, int* currentPos);
void addAlnumHeaderKey(int* ptr, int* currentPos);
void addContigHeaderKey(int* ptr, int* currentPos);
void addContigModeKeys(int* ptr, int* currentPos);
int fillLine(int* ptr, int currentPos, int val);
int fillWithRange(int* ptr, int currentPos, int start, int end, int linestart[2], int separator);

#endif
