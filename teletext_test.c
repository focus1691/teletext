#include <stdio.h>
#include <string.h>
#include "teletext.h"
#include "CuTest.h"
#define L 100

int colorEquals(SDL_Color a, SDL_Color b) {
	if(	a.r == b.r &&
		a.g == b.g &&
		a.b == b.b &&
		a.a == b.a) {
		return 1;
	}
	return 0;
}

void testSpecialLetter(CuTest* tc) {
	CuAssertIntEquals(tc, specialLetter(219), 27);
	CuAssertIntEquals(tc, specialLetter(220), 189);
	CuAssertIntEquals(tc, specialLetter(221), 26);
	CuAssertIntEquals(tc, specialLetter(222), 24);
	CuAssertIntEquals(tc, specialLetter(223), 163);
	CuAssertIntEquals(tc, specialLetter(224), 129);
	CuAssertIntEquals(tc, specialLetter(251), 188);
	CuAssertIntEquals(tc, specialLetter(252), 131);
	CuAssertIntEquals(tc, specialLetter(253), 130);
	CuAssertIntEquals(tc, specialLetter(254), 247);
	CuAssertIntEquals(tc, specialLetter(255), 128);
	CuAssertIntEquals(tc, specialLetter(0), 32);
	CuAssertIntEquals(tc, specialLetter(-34534), 32);
	CuAssertIntEquals(tc, specialLetter(1134), 32);
}

void alphanumTestIsLetter(CuTest* tc) {
	/* Check for positive outcome */
	CuAssertTrue(tc, isLetterAlphanum(0xA1));
	CuAssertTrue(tc, isLetterAlphanum(0xDA));
	CuAssertTrue(tc, isLetterAlphanum(0xE1));
	CuAssertTrue(tc, isLetterAlphanum(0xFA));
	CuAssertTrue(tc, isLetterAlphanum(0xA5));
	CuAssertTrue(tc, isLetterAlphanum(0xF1));
	/* Check for negative outcome */
	CuAssertTrue(tc, !isLetterAlphanum(0x00));
	CuAssertTrue(tc, !isLetterAlphanum(0xA0));
	CuAssertTrue(tc, !isLetterAlphanum(0xDB));
	CuAssertTrue(tc, !isLetterAlphanum(0xE0));
	CuAssertTrue(tc, !isLetterAlphanum(0xFB));
	CuAssertTrue(tc, !isLetterAlphanum(0xFFFFFFF));
}

void testIsLetter(CuTest* tc) {
	/* Check for positive outcome */
	CuAssertTrue(tc, isLetter(100));
	CuAssertTrue(tc, isLetter(1));
	CuAssertTrue(tc, isLetter(0xC0));
	CuAssertTrue(tc, isLetter(0xDF));
	CuAssertTrue(tc, isLetter(0xC5));
	/* Check for negative outcome */
	CuAssertTrue(tc, !isLetter(129));
	CuAssertTrue(tc, !isLetter(0xBF));
	CuAssertTrue(tc, !isLetter(0xE0));
	CuAssertTrue(tc, !isLetter(0xFFFF));
}

void testGetColour(CuTest* tc) {
	/* Alphanumeric codes */
	CuAssertTrue(tc, colorEquals(getColor(RA), RED));
	CuAssertTrue(tc, colorEquals(getColor(GA), GREEN));
	CuAssertTrue(tc, colorEquals(getColor(YA), YELLOW));
	CuAssertTrue(tc, colorEquals(getColor(BA), BLUE));
	CuAssertTrue(tc, colorEquals(getColor(MA), MAGENTA));
	CuAssertTrue(tc, colorEquals(getColor(CA), CYAN));
	/* Graphics codes */
	CuAssertTrue(tc, colorEquals(getColor(RG), RED));
	CuAssertTrue(tc, colorEquals(getColor(GG), GREEN));
	CuAssertTrue(tc, colorEquals(getColor(YG), YELLOW));
	CuAssertTrue(tc, colorEquals(getColor(BG), BLUE));
	CuAssertTrue(tc, colorEquals(getColor(MG), MAGENTA));
	CuAssertTrue(tc, colorEquals(getColor(CG), CYAN));
}

void testIsChangeColour(CuTest* tc) {
	/* Check for positive outcome */
	CuAssertTrue(tc, isChangeColor(0x81));
	CuAssertTrue(tc, isChangeColor(0x84));
	CuAssertTrue(tc, isChangeColor(0x87));
	CuAssertTrue(tc, isChangeColor(0x91));
	CuAssertTrue(tc, isChangeColor(0x95));
	CuAssertTrue(tc, isChangeColor(0x97));
	CuAssertTrue(tc, isChangeColor(0x9C));
	CuAssertTrue(tc, isChangeColor(0x9D));
	/* Check for negative outcome */
	CuAssertTrue(tc, !isChangeColor(0x80));
	CuAssertTrue(tc, !isChangeColor(0x88));
	CuAssertTrue(tc, !isChangeColor(0x90));
	CuAssertTrue(tc, !isChangeColor(0x98));
	CuAssertTrue(tc, !isChangeColor(0x9B));
	CuAssertTrue(tc, !isChangeColor(0x9E));
	CuAssertTrue(tc, !isChangeColor(0x00));
	CuAssertTrue(tc, !isChangeColor(0xFF));
}

void testIsGraphicBlock(CuTest* tc) {
	/* Check for positive outcome */
	CuAssertTrue(tc, isGraphicBlock(0xA1));
	CuAssertTrue(tc, isGraphicBlock(0xB0));
	CuAssertTrue(tc, isGraphicBlock(0xBF));
	CuAssertTrue(tc, isGraphicBlock(0xE0));
	CuAssertTrue(tc, isGraphicBlock(0xF0));
	CuAssertTrue(tc, isGraphicBlock(0xFF));
	/* Check for negative outcome */
	CuAssertTrue(tc, !isGraphicBlock(0xA0));
	CuAssertTrue(tc, !isGraphicBlock(0xC0));
	CuAssertTrue(tc, !isGraphicBlock(0xDF));
	CuAssertTrue(tc, !isGraphicBlock(0x100));
	CuAssertTrue(tc, !isGraphicBlock(0x00));
	CuAssertTrue(tc, !isGraphicBlock(0xFFFFFF));
}

void testFillInCell(CuTest* tc) {
	int gb[BLOCKHEIGHT][BLOCKWIDTH];
	memset(gb, 0, sizeof(gb));
	fillInCell(0, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 0);
	CuAssertIntEquals(tc, gb[1][0], 0);
	CuAssertIntEquals(tc, gb[1][1], 0);
	CuAssertIntEquals(tc, gb[2][0], 0);
	CuAssertIntEquals(tc, gb[2][1], 0);
	fillInCell(1, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 1);
	CuAssertIntEquals(tc, gb[1][0], 0);
	CuAssertIntEquals(tc, gb[1][1], 0);
	CuAssertIntEquals(tc, gb[2][0], 0);
	CuAssertIntEquals(tc, gb[2][1], 0);
	fillInCell(2, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 1);
	CuAssertIntEquals(tc, gb[1][0], 1);
	CuAssertIntEquals(tc, gb[1][1], 0);
	CuAssertIntEquals(tc, gb[2][0], 0);
	CuAssertIntEquals(tc, gb[2][1], 0);
	fillInCell(3, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 1);
	CuAssertIntEquals(tc, gb[1][0], 1);
	CuAssertIntEquals(tc, gb[1][1], 1);
	CuAssertIntEquals(tc, gb[2][0], 0);
	CuAssertIntEquals(tc, gb[2][1], 0);
	fillInCell(4, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 1);
	CuAssertIntEquals(tc, gb[1][0], 1);
	CuAssertIntEquals(tc, gb[1][1], 1);
	CuAssertIntEquals(tc, gb[2][0], 1);
	CuAssertIntEquals(tc, gb[2][1], 0);
	fillInCell(5, gb);
	CuAssertIntEquals(tc, gb[0][0], 1);
	CuAssertIntEquals(tc, gb[0][1], 1);
	CuAssertIntEquals(tc, gb[1][0], 1);
	CuAssertIntEquals(tc, gb[1][1], 1);
	CuAssertIntEquals(tc, gb[2][0], 1);
	CuAssertIntEquals(tc, gb[2][1], 1);
}

void testFillLine(CuTest* tc) {
	int i;
	int test[L];
	char failmsg[L];
	int pos = 0;
	pos = fillLine(test, pos, 0xBEEF);
	CuAssertIntEquals(tc, pos, TELETEXTWIDTH);
	for(i = 0; i < TELETEXTWIDTH; i++) {
		if(test[i] != 0xBEEF) {
			sprintf(failmsg, "test[%d] != 0xBEEF\n", i);
			CuFail(tc, failmsg);
		}
	}
	pos += 4;
	pos = fillLine(test, pos, 0xDEAD);
	CuAssertIntEquals(tc, pos, 2 * TELETEXTWIDTH);
	for(i = TELETEXTWIDTH + 4; i < 2 * TELETEXTWIDTH; i++) {
		if(test[i] != 0xDEAD) {
			sprintf(failmsg, "test[%d] != 0xDEAD\n", i);
			CuFail(tc, failmsg);
		}
	}
}

void testFillWithRange(CuTest* tc) {
	int i;
	int testStart[2] = {0xDEAD, 0xBEEF};
	int test[L];
	char failmsg[L];
	int pos = 0;

	pos = fillWithRange(test, pos, 0xF0, 0xFF, testStart, 0);
	CuAssertIntEquals(tc, pos, 0xFF - 0xF0 + 3);
	CuAssertIntEquals(tc, test[0], 0xDEAD);
	CuAssertIntEquals(tc, test[1], 0xBEEF);
	for(i = 2; i < pos; i++) {
		if(test[i] != i + 0xF0 - 1) {
			sprintf(failmsg, "test[%d] != %d", i, i + 0xF0 - 1);
		}
	}
}

CuSuite* GetTeletextTestSuite(void) {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testSpecialLetter);
	SUITE_ADD_TEST(suite, alphanumTestIsLetter);
	SUITE_ADD_TEST(suite, testIsLetter);
	SUITE_ADD_TEST(suite, testGetColour);
	SUITE_ADD_TEST(suite, testIsChangeColour);
	SUITE_ADD_TEST(suite, testIsGraphicBlock);
	SUITE_ADD_TEST(suite, testFillInCell);
	SUITE_ADD_TEST(suite, testFillLine);
	SUITE_ADD_TEST(suite, testFillWithRange);

	return suite;
}

void RunTests(void) {
	CuString* output = CuStringNew();
	CuSuite* suite = CuSuiteNew();
	CuSuiteAddSuite(suite, GetTeletextTestSuite());
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}
