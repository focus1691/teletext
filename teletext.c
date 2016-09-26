#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "teletext.h"

void RunTests(void);

int main(int argc, char * argv[]) {
    display dis;
    controlCodes CC;

    RunTests();
    instructions();

	if(argc == 2) {
    	readfile(argv[1], &CC); /* Only read the file if there are 2 arguments */
	} else {
	    createDefaultPage(&CC); /* Otherwise create a default teletext page */
	}
	initAndStart(&dis, &CC);
    return 0;
}

void instructions() {
	printf("Controls:\n"
			"   h -------------- show / hide keyboard (works only in windowed mode)\n"
			"   f -------------- fullscreen / windowed mode\n"
			"   < -------------- move left\n"
			"   > -------------- move right\n"
			"   del ------------ delete a control code\n");
	printf("Press any button to continue.");
	getch();
}

void readfile(char *file_name, controlCodes* CC) {
    int ch, i = 0, j = 0, count = 0, amount = 0;
    FILE *fp;

    if(!(fp = fopen(file_name, "rb"))) {
        fprintf(stderr, "Error while opening the file.\n");
        SDL_Quit();
        exit(1);
    }

    while(((ch = fgetc(fp)) != EOF) && amount < MAXIMUM) {
        if (count == TELETEXTWIDTH) {
            count = 0;
            i++;
            j = 0;
        }
        count++;
        amount++;
        CC->hexcode[i][j] = ch;
        j++;
    }
    if(amount < MAXIMUM) { /*Check if characters are there*/
        fprintf(stderr, "Missing character codes\n");
        SDL_Quit();
        exit(1);
    }
    fclose(fp);
}

void createDefaultPage(controlCodes* CC) {

    int i = 0, j = 0;

    for (i = 0; i < TELETEXTHEIGHT; i++) {
        for (j = 0; j < TELETEXTWIDTH; j++) {
            CC->hexcode[i][j] = 0xA0;
        }
    }
}

void initgraphicBlock(int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]) {

    int i, j;
    for (i = 0; i < BLOCKHEIGHT; i++) {
        for (j = 0; j < BLOCKWIDTH; j++) {
            graphicBlock[i][j] = 0;
        }
    }
}

void fillInCell(int cellposition, int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]) {
    switch (cellposition) {
    case 0:
        graphicBlock[0][0] = 1;
        break;
    case 1:
        graphicBlock[0][1] = 1;
        break;
    case 2:
        graphicBlock[1][0] = 1;
        break;
    case 3:
        graphicBlock[1][1] = 1;
        break;
    case 4:
        graphicBlock[2][0] = 1;
        break;
    case 5:
        graphicBlock[2][1] = 1;
        break;
    }
}

void getGraphicBlock(int gpCode, int graphicBlock[BLOCKHEIGHT][BLOCKWIDTH]) {

    int n, i = BASENUMBER, cell = 0;

    initgraphicBlock(graphicBlock);

    if (gpCode < C0) {
        n = gpCode - A0;
    } else {
        (n = gpCode - A0 - 32);
    }
    if (n == 0) {
        return;
    }
    while (i <= n) {
        i = i * BASENUMBER;
        cell++;
    }
    while (n > 0) {
        while (pow(BASENUMBER, cell) > n) {
            cell--;
        }
        fillInCell(cell, graphicBlock);
        n = n - pow(BASENUMBER, cell);
        cell--;
    }
}

int isGraphicBlock(int code) {
    /* Graphic block in the contiguous and separated graphics tables are from 0xA1 to 0xBF and from 0xE0 to 0xFF */
    if ((A1 <= code && code <= BF) || (E0 <= code && code <= FF)) {
        return 1;
    }
    return 0;
}

int isChangeColor(int code) {
    /* The colour codes are from 0x81 to 0x87 and 0x91 to 0x97 and 0x9C and 0x9D */
    if ((x81 <= code && code <= x87) || (x91 <= code && code <= x97) || (x9C <= code && code <= x9D)) {
        return 1;
    }
    return 0;
}

SDL_Color getColor(int incode) {
    SDL_Color gcolor;
    int code = incode;
    /* Check if the code is in the range of colour code choices */
    if (x81 <= code && code <= x87) {
        code = code - BASEASCII;
    }
    else if (x91 <= code && code <= x9D) {
        code = code - BASEASCII - 16;
    }
    switch(code) {
    case 1:
        gcolor = RED;
        break;
    case 2:
        gcolor = GREEN;
        break;
    case 3:
        gcolor = YELLOW;
        break;
    case 4:
        gcolor = BLUE;
        break;
    case 5:
        gcolor = MAGENTA;
        break;
    case 6:
        gcolor = CYAN;
        break;
    case 7:
        gcolor = WHITE;
        break;
    case 12:
        gcolor = BLACK;
        break;
    case 13:
        gcolor = WHITE;
        break;
        default:;
    }
    return gcolor;
}

int isLetter(int code) {
/* If the code <128 (ASCII) or from 0xC0 to 0xDF in the contiguous graphics table, then it is a letter */
    if ((code < BASEASCII) || (C0 <= code && code <= DF)) {
        return 1;
    }
    return 0;
}

int isLetterAlphanum(int code) {
/* If the code is from 0xA1 to 0xDA or 0xE1 to 0xFA in the alphanumeric table, then it is a letter*/
    if ((A1 <= code && code <= DA) || (E0 < code && code <= FA)) {
        return 1;
    }
    return 0;
}

int specialLetter(int code) {
/* The special letters from the alphanumeric table are from 0xDB to 0xDF and 0xFB to 0xFF */
    switch (code) {
    case 219:
        return 27;
        break;
    case 220:
        return 189;
        break;
    case 221:
        return 26;
        break;
    case 222:
        return 24;
        break;
    case 223:
        return 163;
        break;
    case 224:
        return 129;
        break;
    case 251:
        return 188;
        break;
    case 252:
        return 131;
        break;
    case 253:
        return 130;
        break;
    case 254:
        return 247;
        break;
    case 255:
        return 128;
        break;
    default:
        return 32;
    }
}

void DrawRectangle(display* dis, controlCodes* CC, int i, int j, SDL_Color color) {
	Uint8 prevR, prevG, prevB, prevA;

    /* Declare the coordinates of the top-left edge of the rectangle */
    int Xcoordinates = j * TEXTWIDTH + CC->positionX;
    int Ycoordinates = i * TEXTHEIGHT + CC->positionY;
    SDL_Rect rectangle;
	rectangle.x = Xcoordinates;
	rectangle.y = Ycoordinates;
	rectangle.w = TEXTWIDTH;
	rectangle.h = TEXTHEIGHT;

	SDL_GetRenderDrawColor(dis->renderer, &prevR, &prevG, &prevB, &prevA);
    SDL_SetRenderDrawColor(dis->renderer, color.r, color.g, color.b, color.a);

    SDL_RenderFillRect(dis->renderer, &rectangle);
    SDL_SetRenderDrawColor(dis->renderer, prevR, prevG, prevB, prevA);
}

void DrawLetter(display* dis, controlCodes* CC, char str[1], int i, int j, SDL_Color color) {
	SDL_Rect dstrect;
    /* Declare the coordinates of the top-left edge of the rectangle */
    int Xcoordinates = j * TEXTWIDTH + CC->positionX;
    int Ycoordinates = i * TEXTHEIGHT + CC->positionY;
    dis->surface = TTF_RenderText_Solid(dis->font, str,color);

    if(dis->surface == NULL) {
        fprintf(stderr, "Failed to load surface! SDL_Surface Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    dis->texture = SDL_CreateTextureFromSurface(dis->renderer, dis->surface);
	SDL_FreeSurface(dis->surface);

    if(dis->texture == NULL) {
        fprintf(stderr, "Failed to load texture! SDL_texture Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    SDL_QueryTexture(dis->texture, NULL, NULL, &CC->texW, &CC->texH);
    dstrect.x = Xcoordinates;
	dstrect.y = Ycoordinates;
	dstrect.w = CC->texW;
	dstrect.h = CC->texH;
    SDL_RenderCopy(dis->renderer, dis->texture, NULL, &dstrect);
	SDL_DestroyTexture(dis->texture);
}

void createDisplay(display* dis) {
    SDL_Color bgcolor = BLACK;
    TTF_Init();
    dis->window = NULL;
    dis->renderer = NULL;
    dis->window = SDL_CreateWindow ("SDL screen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SPLITSCREEN, WINDOWHEIGHT, SDL_WINDOW_SHOWN);

    if (dis->window == NULL) {
        fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    dis->renderer = SDL_CreateRenderer(dis->window, -1, SDL_RENDERER_ACCELERATED);
    if (dis->renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    /* Set the render colour to black for the background */
    SDL_SetRenderDrawColor(dis->renderer, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
    SDL_RenderClear(dis->renderer);
}

void setDefault(controlCodes* CC) {
    CC->bgcolor = BLACK;
    CC->lcolor = WHITE;
    CC->table = ALPHA; /* alphanumeric table */
    CC->hold = 0; /* release hold graphic is off */
    CC->mode = CONTI; /* contiguous graphics mode */
    CC->checksingle = 1; /* single height size mode in on */
}

void checkHeight(display* dis, controlCodes* CC, int i, int j) {
    if (CC->hexcode[i][j] == SINGLE) {
        DrawRectangle(dis, CC, i, j, CC->bgcolor); /* Draw a space */
        CC->checksingle = 1; /*1 in single height mode, 0 in double height mode*/
    } else if (CC->hexcode[i][j] == DOUBLE) { /*Double height mode*/
        DrawRectangle(dis, CC, i, j, CC->bgcolor); /*Draw a space*/
        if (CC->hexcode[i-1][j] != DOUBLE) { /*if here is the first line of double height, mark it = 1*/
            CC->checkdouble = 1; /*double height current line is first line.*/
            CC->checksingle = 0; /*in double height mode, single height = 0*/
        } else if (CC->hexcode[i-1][j] == DOUBLE) { /*if the code on top of this code are double height, it means this is the second line of double height*/
            CC->checkdouble = 2; /*double height current line is second line.*/
            CC->checksingle = 0; /*turn off single height mode*/
            CC->hexcode[i][j] = SPACE; /*32 is ascii code of a space*/
        }
    }
}

void checkHoldMode(display* dis, controlCodes* CC, int i, int j) {
       /*hold mode 0x9E*/
    if (CC->hexcode[i][j] == HOLD) {
        CC->hold = 1;
        CC->hexcode[i][j] = CC->mostRecentGraphic; /* if code is in hold mode then it will become the most recent graphic block */
    }
    /*release hold mode 0x9F*/
    if (CC->hexcode[i][j] == RELEASE) {
        CC->hold = 0;
        DrawRectangle(dis, CC, i, j, CC->bgcolor);
    }
}

void checkChangeColor(display* dis, controlCodes* CC, int i, int j) {
    if(isChangeColor(CC->hexcode[i][j])) {
        DrawRectangle(dis, CC, i, j, CC->bgcolor);
        if (x81 <= CC->hexcode[i][j] && CC->hexcode[i][j] <= x87) { /*if color in 0x8 column*/
            CC->lcolor = getColor(CC->hexcode[i][j]);
            CC->table = ALPHA; /*we using table 1 for color in 0x8 column*/
        } else if (x91 <= CC->hexcode[i][j] && CC->hexcode[i][j] <= x97) { /*if color in 0x9 column*/
            CC->gpcolor = getColor(CC->hexcode[i][j]);
            CC->mode = CONTI; /*(2 is contiguous, 3 is separate)*/
            CC->table = CONTI; /*we using table 2 for color in 0x9 column*/
        } else if (CC->hexcode[i][j] == x9C) { /*set black background code*/
            CC->bgcolor = getColor(CC->hexcode[i][j]);
        } else if (CC->hexcode[i][j] == x9D) { /*new background code*/
            if (isChangeColor(CC->hexcode[i][j-1])) { /*check if the code before this code is color, it means we have to apply that color for new background*/
                CC->bgcolor = getColor(CC->hexcode[i][j-1]);
            } else { /*else we set white color as default*/
                CC->bgcolor = getColor(CC->hexcode[i][j]);
            }
        }
        if (CC->hold == 1) { /*if hold mode is on, all color code have turn to most recent graphic block*/
            CC->hexcode[i][j] = CC->mostRecentGraphic;
        }
    }
}

void checkContiguousGraphic(display* dis, controlCodes* CC, int i, int j) {
    /* control code in contiguous graphics mode */
    if (CC->hexcode[i][j] == CONTIGUOUS){
        CC->mode = CONTI;
        CC->table = CONTI;
        DrawRectangle(dis, CC, i, j, CC->bgcolor);
    }
    /* Control code in separate graphics mode */
    if (CC->hexcode[i][j] == SEPARATE) {
        CC->mode = SEPARA;
        CC->table = CONTI;
        DrawRectangle(dis, CC, i, j, CC->bgcolor);
    }
}

void alphanumericTable(display* dis, controlCodes* CC, int i, int j) {
    char str[1];
    /* if the code is a normal letter*/
    if (isLetterAlphanum(CC->hexcode[i][j])) {
        sprintf(str, "%c", (CC->hexcode[i][j] - BASEASCII)); /*-128 to turn to ASCII character*/
    }
    /*if the code is an ASCII character*/
    else if (CC->hexcode[i][j] < BASEASCII) {
        sprintf(str, "%c", CC->hexcode[i][j]);
    }
    /* if the code is a special character*/
    else {
        sprintf(str, "%c", specialLetter(CC->hexcode[i][j]));
    }

    DrawRectangle(dis, CC, i, j, CC->bgcolor);
    dis->font = pMAINFONT;
    CC->checkMapDouble[i][j] = 0;

    /*if it in double height mode and current position in first line of double height*/
    if (CC->checkdouble == 1 && CC->checksingle == 0 && CC->checkMapDouble[i-1][j] != 1) {
        /*get font for display a top half character*/
        dis->font = pTOPHALF;
        CC->checkMapDouble[i][j] = 1;
    }
    /*if it in double height mode and current in second line of double height*/
    else if ((CC->checkdouble == 2 || CC->checkMapDouble[i-1][j] == 1) && CC->checksingle == 0) {
        /*get font for display a bottom half character*/
        dis->font = pBOTTOMHALF;
    }

    if(dis->font == NULL) {
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    DrawLetter(dis, CC, str, i, j, CC->lcolor);
}

void graphicProcess(display* dis, controlCodes* CC, int i, int j) {
    int i2, j2;
    /*update most recent graphic block in case needed in hold mode*/
    CC->mostRecentGraphic = CC->hexcode[i][j];
    /*get graphic block 3x2 from code*/
    getGraphicBlock(CC->hexcode[i][j], CC->graphicBlock);
    DrawRectangle(dis, CC, i, j, CC->bgcolor);

    /*present the graphic block 3x2 on screen*/
    for (i2 = 0; i2 < BLOCKHEIGHT; i2++) {
        for (j2 = 0; j2 < BLOCKWIDTH; j2++) {
            /*if  graphic block in contiguous mode, get size of unit cell;*/
            if (CC->mode == 2) {
                CC->rectangle_unitContiguous.y = i * TEXTHEIGHT + i2 * CONTIGUOUSHEIGHT + CC->positionY;
                CC->rectangle_unitContiguous.x = j * TEXTWIDTH + j2 * CONTIGUOUSWIDTH + CC->positionX;
            }
            /*else graphic block in separate mode, get size of unit cell*/
            else {
                CC->rectangle_unitSeparate.y = i*TEXTHEIGHT + i2*CONTIGUOUSHEIGHT + 1 + CC->positionY;
                CC->rectangle_unitSeparate.x = j*TEXTWIDTH + j2*CONTIGUOUSWIDTH + 1 + CC->positionX;
            }
            /*if cell is empty, set cell color to background color*/
            if (CC->graphicBlock[i2][j2] == 0) {
                SDL_SetRenderDrawColor(dis->renderer, CC->bgcolor.r, CC->bgcolor.g, CC->bgcolor.b, CC->bgcolor.a);
            }
            /*if cell is not empty set cell color to graphic color*/
            else {
                SDL_SetRenderDrawColor(dis->renderer, CC->gpcolor.r, CC->gpcolor.g, CC->gpcolor.b, CC->gpcolor.a);
            }
            /*if in contiguous mode*/
            if (CC->mode == CONTI) {
                /* Render rect*/
                SDL_RenderFillRect(dis->renderer, &CC->rectangle_unitContiguous); /*draw cell*/
            }
             /*else it in separate mode*/
            else {
                SDL_RenderFillRect(dis->renderer, &CC->rectangle_unitSeparate); /*draw cell*/
            }
        }
    }
}

void letterProcess(display* dis, controlCodes* CC, int i, int j) {
    char str[1];
    /*if letter is ASCII code*/
    if (CC->hexcode[i][j] < BASEASCII) {
        /*get letter from code*/
        sprintf(str, "%c", CC->hexcode[i][j]);
    } else if (DA < CC->hexcode[i][j] && CC->hexcode[i][j] < E0) {
        sprintf(str, "%c", specialLetter(CC->hexcode[i][j]));
    }
    /*else letter is letter in table*/
    else {
        /*get letter from code*/
        sprintf(str, "%c", CC->hexcode[i][j] - BASEASCII);
    }
    DrawRectangle(dis, CC, i, j, CC->bgcolor);
    dis->font = pMAINFONT;
    if(dis->font == NULL) {
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    DrawLetter(dis, CC, str, i, j, CC->gpcolor); /*Draw letter*/
}

void graphicTable(display* dis, controlCodes* CC, int i, int j) {
    /*if code is graphic block*/
    if (isGraphicBlock(CC->hexcode[i][j])) {
        graphicProcess(dis, CC, i, j);
    }
    /*if code is letter*/
    else if(isLetter(CC->hexcode[i][j])) {
        letterProcess(dis, CC, i, j);
    }
}

void initTeletext(controlCodes* CC) {
	CC->positionY = 0;
	CC->positionX = 0;
	CC->texW = TEXTWIDTH;
    CC->texH = TEXTHEIGHT;
    CC->textsize = SINGLESIZE;
    CC->checkdouble = 0;
    CC->checksingle = 0;
    CC->rectangle_unitContiguous.w = CONTIGUOUSWIDTH;
    CC->rectangle_unitContiguous.h = CONTIGUOUSHEIGHT;
    CC->rectangle_unitSeparate.w = SEPARATEWIDTH;
    CC->rectangle_unitSeparate.h = SEPARATEHEIGHT;
}

void closeDisplay(display* dis) {
    TTF_CloseFont(dis->font);
    SDL_DestroyRenderer(dis->renderer);
    SDL_DestroyTexture(dis->texture);
    SDL_DestroyWindow(dis->window);
    TTF_Quit();
    SDL_Quit();
}

void saveToFile(controlCodes* CC) {
	int i;
	char filename[MAXNAME];
	FILE* file;
	int* ptr = (int*)CC->hexcode;
	printf("Enter filename: ");
	scanf("%s", filename);

    if(!(file = fopen(filename, "wb"))) {
        fprintf(stderr, "Error while trying to save file.\n");
        SDL_Quit();
        exit(1);
    }
	for(i = 0; i < MAXIMUM; i++) {
		fprintf(file, "%c", (char)ptr[i]);
	}
	fclose(file);
}

void handleKeyDown(SDL_Keycode code, controlCodes* CC, int* pCursorX, int* pCursorY, display* dis) {

    static int keyboardShown = 1;
	static int windowFullscreen = 0;

	switch(code) {
		case SDLK_UP:
			(*pCursorY)--;
			break;
		case SDLK_DOWN:
			(*pCursorY)++;
			break;
		case SDLK_LEFT:
			(*pCursorX)--;
			break;
		case SDLK_RIGHT:
			(*pCursorX)++;
			break;
        case SDLK_DELETE:
            (*pCursorX)--;
            CC->hexcode[(*pCursorY)][(*pCursorX)] = A0;
            break;
		case SDLK_h:
			if(!windowFullscreen) {
				if(keyboardShown) {
					SDL_SetWindowSize(dis->window, HALFSCREEN, WINDOWHEIGHT);
				} else {
					SDL_SetWindowSize(dis->window, SPLITSCREEN, WINDOWHEIGHT);
				}
				keyboardShown = !keyboardShown;
			}
			break;
		case SDLK_f:
			if(!keyboardShown) {
				if(windowFullscreen) {
					SDL_SetWindowFullscreen(dis->window, 0);
				} else {
					SDL_SetWindowFullscreen(dis->window, SDL_WINDOW_FULLSCREEN);
				}
				windowFullscreen = !windowFullscreen;
			}
			break;
	}
}

void handleMouseButtonDown(SDL_Event event, int* pCursorX, int* pCursorY, controlCodes* CC, controlCodes* keyboard) {
	int colClick, rowClick;
	rowClick = event.button.y / TEXTHEIGHT;
	colClick = (event.button.x - HALFSCREEN) / TEXTWIDTH;

	if(event.button.button == SDL_BUTTON_LEFT) {
		if(event.button.x > HALFSCREEN) {
			if(rowClick == SEPA) {
                checkSeparatedSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if (rowClick == CONT) {
			    checkContiguousSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(rowClick == ALNUM) {
                checkAlphaSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(rowClick == GRAPH) {
                checkBlockSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(rowClick == SING_DOUB_SAVE) {
                checkSingDoubSaveSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(rowClick == BACKGROUND) {
                checkBackgroundSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(rowClick == HOLD_RELEASE_G) {
                checkHoldReleaseSelection(&colClick, CC, pCursorX, pCursorY);
			}
			else if(colClick > 1) { /* Check character selection */
				CC->hexcode[(*pCursorY)][(*pCursorX)] = keyboard->hexcode[rowClick][colClick];
				(*pCursorX)++;
			}
		}
	}
}

void checkSeparatedSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= LEFTBOUNDARY && *colClick <= SEPA_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = SEP_G;
        (*pCursorX)++;
    }
}

void checkContiguousSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= LEFTBOUNDARY && *colClick <= CONTIG_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = CONT_G;
        (*pCursorX)++;
    }
}

void checkSingDoubSaveSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= LEFTBOUNDARY && *colClick <= SINGLEH_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = SINGLE_H;
        (*pCursorX)++;
    }
    else if(*colClick >= DOUBLEH_LEFTBOUNDARY && *colClick <= DOUBLEH_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = DOUBLE_H;
        (*pCursorX)++;
    }
    else if(*colClick >= SAVE_LEFTBOUNDARY && *colClick <= SAVE_RIGHTBOUNDARY) {
        saveToFile(CC);
    }
}

void checkAlphaSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= ALPHA_LEFTBOUNDARY && *colClick <= ALPHA_RIGHTBOUNDARY) {
        switch(*colClick) {
            case white:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = WA;
                break;
            case red:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = RA;
                break;
            case green:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = GA;
                break;
            case yellow:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = YA;
                break;
            case darkblue:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = BA;
                break;
            case purple:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = MA;
                break;
            case lightblue:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = CA;
                break;
        }
        (*pCursorX)++;
    }
}

void checkBlockSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= GRAPH_LEFTBOUNDARY && *colClick <= GRAPH_RIGHTBOUNDARY) {
        switch(*colClick) {
            case white:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = WG;
                break;
            case red:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = RG;
                break;
            case green:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = GG;
                break;
            case yellow:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = YG;
                break;
            case darkblue:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = BG;
                break;
            case purple:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = MG;
                break;
            case lightblue:
                CC->hexcode[(*pCursorY)][(*pCursorX)] = CG;
                break;
        }
        (*pCursorX)++;
    }
}

void checkHoldReleaseSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= LEFTBOUNDARY && *colClick <= HOLD_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = HOLD_G;
        (*pCursorX)++;
    }
    else if(*colClick >= RELEASE_LEFTBOUNDARY && *colClick <= RELEASE_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = REL_G;
        (*pCursorX)++;
    }
}

void checkBackgroundSelection(int* colClick, controlCodes* CC, int* pCursorX, int* pCursorY) {
    if(*colClick >= LEFTBOUNDARY && *colClick <= NEWBACK_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = NEW_BACK;
        (*pCursorX)++;
    }
    else if(*colClick >= BLACK_LEFTBOUNDARY && *colClick <= BLACK_RIGHTBOUNDARY) {
        CC->hexcode[(*pCursorY)][(*pCursorX)] = BL_BACK;
        (*pCursorX)++;
    }
}

void handleEvent(display* dis, controlCodes* CC) {

	int showCursor = 1;
	int cursorX = 0, cursorY = 0;
	SDL_Event event;
	bool quit = false;

	controlCodes keyboard;
	initTeletext(&keyboard);
	fillKeyboard(&keyboard);
	keyboard.positionX = HALFSCREEN;

	/* Wait until the user closes the window */
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					handleKeyDown(event.key.keysym.sym, CC, &cursorX, &cursorY, dis);
					checkCursor(&cursorX, &cursorY);
					break;
				case SDL_MOUSEBUTTONDOWN:
					handleMouseButtonDown(event, &cursorX, &cursorY, CC, &keyboard);
					break;
				case SDL_USEREVENT:
					if(event.user.code == 0) {
						showCursor = !showCursor;
					}
					break;
			}
		}
		SDL_RenderClear(dis->renderer);
		drawGraphicBlock(dis, CC);
		if(showCursor) {
			DrawRectangle(dis, CC, cursorY, cursorX, CURSOR_COLOR);
		}
		drawGraphicBlock(dis, &keyboard);
		SDL_SetRenderDrawColor(dis->renderer, 255, 255, 255, 1); /* White */
		SDL_RenderDrawLine(dis->renderer, HALFSCREEN, 0, HALFSCREEN, WINDOWHEIGHT);
		SDL_RenderPresent(dis->renderer);
	}
	closeDisplay(dis);
}

void checkCursor(int* x, int* y) {

    /* Adjust the cursor if it goes out of bounds */
    if(*x < 0) {
        *x = TELETEXTWIDTH - 1;
        *y-=1;
    }
    else if(*x > TELETEXTWIDTH - 1) {
			*x = 0;
			*y+=1;
    }
    if(*y < 0) {
        *y = TELETEXTHEIGHT - 1;
    }
    else if(*y > TELETEXTHEIGHT - 1) {
        *y = 0;
    }
}

void drawGraphicBlock(display* dis, controlCodes* CC) {

	int i, j;
	int savedHexCode;

    for (i = 0; i < TELETEXTHEIGHT; i++) {
        /* Default: Each newline has white text, single height, contiguous graphics, black background, release graphics. */
        setDefault(CC);
        for (j = 0; j < TELETEXTWIDTH; j++) {
			savedHexCode = CC->hexcode[i][j];
            /* Check if the control code is for Single height mode */
            checkHeight(dis, CC, i, j);
            /* Is the control code for Hold Mode? */
            checkHoldMode(dis, CC, i, j);
            /* Is the control code to change the colour? */
            checkChangeColor(dis, CC, i, j);
            /* Is the control code for Contiguous graphic or separate graphic mode? */
            checkContiguousGraphic(dis, CC, i, j);
            /* Is the code for a space? */
            if (CC->hexcode[i][j] == A0) {
                DrawRectangle(dis, CC, i, j, CC->bgcolor);
            }
            /* Is the control code for the alphanumeric table? */
            else if (CC->table == ALPHA) {
                alphanumericTable(dis, CC, i, j);
            }
            /* Is the control code for Contiguous graphics mode? */
            else if (CC->table == CONTI) {
                graphicTable(dis, CC, i, j);
            }
			CC->hexcode[i][j] = savedHexCode;
        }
    }
}

Uint32 cursorTimerCallback(Uint32 interval, void* param) {

	SDL_Event event;
	SDL_UserEvent userEvent;
    (void)param;
	userEvent.type = SDL_USEREVENT;
	userEvent.code = 0; /* Switch showCursor flag. */
	userEvent.data1 = NULL;
	userEvent.data2 = NULL;
	event.type = SDL_USEREVENT;
	event.user = userEvent;
	SDL_PushEvent(&event);
	return interval;
}

void initAndStart(display* dis, controlCodes* CC) {

	SDL_AddTimer(500, cursorTimerCallback, NULL); /* arbitrary value */
    createDisplay(dis);
    initTeletext(CC);
	pMAINFONT = TTF_OpenFont(MAINFONT, CC->textsize);
	pBOTTOMHALF = TTF_OpenFont(BOTTOMHALF, CC->textsize);
	pTOPHALF = TTF_OpenFont(TOPHALF, CC->textsize);

    if (pMAINFONT == NULL || pBOTTOMHALF == NULL || pTOPHALF == NULL) {
        fprintf(stderr, "Font could not be loaded SDL_Error: %s.\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
	handleEvent(dis, CC);
}

void fillKeyboard(controlCodes* keyboard) {

	int currentPos = 0;
	int* ptr = (int*)keyboard->hexcode;
	memset(ptr, 0xA0, sizeof(keyboard->hexcode));

	addAlnumHeaderKey(ptr, &currentPos);
	addAlnumKeys(ptr, &currentPos);
	addSepModeHeader(ptr, &currentPos);
	addSepModeGraphicsKeys(ptr, &currentPos);
	addContigHeaderKey(ptr, &currentPos);
	addContigModeKeys(ptr, &currentPos);
    addAlnumColourKeys(ptr, &currentPos);
	addBlockColourKeys(ptr, &currentPos);
	addHeightSelectionKeys(ptr, &currentPos);
	addBackgroundKeys(ptr, &currentPos);
    addRealeaseGraphicsKeys(ptr, &currentPos);
}

int fillLine(int* ptr, int currentPos, int val) {

	int i, filledInLine;

	filledInLine = currentPos % TELETEXTWIDTH;
	for(i = 0; i < TELETEXTWIDTH - filledInLine; i++) {
		ptr[currentPos+i] = val;
	}
	return currentPos + i;
}

int fillWithRange(int* ptr, int currentPos, int start, int end, int linestart[2], int separator) {

	int k;

	for(k = start; k <= end; k++) {
		if(currentPos % TELETEXTWIDTH == 0) {
			ptr[currentPos++] = linestart[0];
			ptr[currentPos++] = linestart[1];
		}
		ptr[currentPos++] = k;
		if(separator != 0) {
			ptr[currentPos++] = separator;
		}
	}
	return currentPos;
}

void addAlnumHeaderKey(int* ptr, int* currentPos) {

    const int alnum_mode[] = { GA, NEW_BACK, WA, 0xC1, 0xEC, 0xEE, 0xF5, 0xED, 0xA0, 0xED, 0xEF, 0xE4, 0xE5, BL_BACK };

	memcpy(ptr, alnum_mode, sizeof(alnum_mode));
	*currentPos += sizeof(alnum_mode) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addAlnumKeys(int* ptr, int* currentPos) {

    int alnum_mode_start[2] = {WA, 0xA0};

	*currentPos = fillWithRange(ptr, *currentPos, 0xA1, 0xFF, alnum_mode_start, 0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addSepModeHeader(int* ptr, int* currentPos) {

    const int sep_mode[] = {GA, NEW_BACK, WA, 0xD3, 0xE5, 0xF0, 0xA0, 0xED, 0xEF, 0xE4, 0xE5, BL_BACK};

	memcpy(ptr + *currentPos, sep_mode, sizeof(sep_mode));
	*currentPos += sizeof(sep_mode) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addSepModeGraphicsKeys(int* ptr, int* currentPos) {

    int sep_mode_start[2] = {WG, SEP_G};

	*currentPos = fillWithRange(ptr, *currentPos, 0xA1, 0xBF, sep_mode_start, 0xA0);
	*currentPos = fillWithRange(ptr, *currentPos, 0xE0, 0xFF, sep_mode_start, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addContigHeaderKey(int* ptr, int* currentPos) {

    const int cont_mode[] = {GA, NEW_BACK, WA, 0xC3, 0xEF, 0xEE, 0xF4, 0xA0, 0xED, 0xEF, 0xE4, 0xE5, BL_BACK};

	memcpy(ptr + *currentPos, cont_mode, sizeof(cont_mode));
	*currentPos += sizeof(cont_mode) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addContigModeKeys(int* ptr, int* currentPos) {

    int cont_mode_start[2] = {WG, CONT_G};

	*currentPos = fillWithRange(ptr, *currentPos, 0xA1, 0xBF, cont_mode_start, 0xA0);
	*currentPos = fillWithRange(ptr, *currentPos, 0xE0, 0xFF, cont_mode_start, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addAlnumColourKeys(int* ptr, int* currentPos) {

    const int alnum_color[] = {WA, 0xC1, 0xEC, 0xEE, 0xF5, 0xED, 0xA0, 0xC3, 0xEF, 0xEC, 0xEF, 0xF5, 0xF2, 0xBA,
                           0xA0, 0xFF, HOLD_G, RA, GA, YA, BA, MA, CA, REL_G};

	memcpy(ptr + *currentPos, alnum_color, sizeof(alnum_color));
	*currentPos += sizeof(alnum_color) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addBlockColourKeys(int* ptr, int* currentPos) {

    const int graph_color[] = { WA, 0xC7, 0xF2, 0xE1, 0xF0, 0xE8, 0xA0, 0xC3, 0xEF, 0xEC, 0xEF, 0xF5,
                           0xF2, 0xBA, WG, HOLD_G, 0xFF, RG, GG, YG, BG, MG, CG, REL_G };

	memcpy(ptr + *currentPos, graph_color, sizeof(graph_color));
	*currentPos += sizeof(graph_color) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addHeightSelectionKeys(int* ptr, int* currentPos) {

    const int single_double_save[] = { NEW_BACK, RA, 0xD3, 0xC9, 0xCE, 0xC7, 0xCC, 0xC5, 0xA0, 0xA0, BL_BACK, WA,
                            NEW_BACK, RA, 0xC4, 0xCF, 0xD5, 0xC2, 0xCC, 0xC5, 0xA0, 0xA0, BL_BACK, YA,
                            NEW_BACK, RA, 0xD3, 0xC1, 0xD6, 0xC5, 0xA0, 0xA0, BL_BACK };

	memcpy(ptr + *currentPos, single_double_save, sizeof(single_double_save));
	*currentPos += sizeof(single_double_save) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addBackgroundKeys(int* ptr, int* currentPos) {

    const int new_bg_black_bg[] = { NEW_BACK, BA, 0xCE, 0xC5, 0xD7, 0xA0, 0xC2, 0xC7, 0xA0, 0xA0, BL_BACK,
                                WA, NEW_BACK, BA, 0xC2, 0xCC, 0xC1, 0xC3, 0xCB, 0xA0, 0xC2, 0xC7,BL_BACK };

    memcpy(ptr + *currentPos, new_bg_black_bg, sizeof(new_bg_black_bg));
	*currentPos += sizeof(new_bg_black_bg) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}

void addRealeaseGraphicsKeys(int* ptr, int* currentPos) {

    const int hold_g_rel_g[] = { NEW_BACK, GA, 0xC8, 0xCF, 0xCC, 0xC4, 0xA0, 0xC7, 0xA0, 0xA0, BL_BACK, WA,
                             NEW_BACK, GA, 0xD2, 0xC5, 0xCC, 0xC5, 0xC1, 0xD3, 0xC5, 0xA0, 0xC7, BL_BACK };

    memcpy(ptr + *currentPos, hold_g_rel_g, sizeof(hold_g_rel_g));
	*currentPos += sizeof(hold_g_rel_g) / sizeof(int);
	*currentPos = fillLine(ptr, *currentPos, 0xA0);
}
