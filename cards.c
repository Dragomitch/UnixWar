/*
 * =====================================================================================
 *
 *       Filename:  cards.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-05-14 10:49:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL
 *
 * =====================================================================================
 */
#include "cards.h"

char* card_names[] = {
	"2 of Spades",
	"2 of Clubs",
	"2 of Diamonds",
	"2 of Hearts",
	"3 of Spades",
	"3 of Clubs",
	"3 of Diamonds",
	"3 of Hearts",
	"4 of Spades",
	"4 of Clubs",
	"4 of Diamonds",
	"4 of Hearts",
	"5 of Spades",
	"5 of Clubs",
	"5 of Diamonds",
	"5 of Hearts",
	"6 of Spades",
	"6 of Clubs",
	"6 of Diamonds",
	"6 of Hearts",
	"7 of Spades",
	"7 of Clubs",
	"7 of Diamonds",
	"7 of Hearts",
	"8 of Spades",
	"8 of Clubs",
	"8 of Diamonds",
	"8 of Hearts",
	"9 of Spades",
	"9 of Clubs",
	"9 of Diamonds",
	"9 of Hearts",
	"10 of Spades",
	"10 of Clubs",
	"10 of Diamonds",
	"10 of Hearts",
	"Jack of Spades",
	"Jack of Clubs",
	"Jack of Diamonds",
	"Jack of Hearts",
	"Queen of Spades",
	"Queen of Clubs",
	"Queen of Diamonds",
	"Queen of Hearts",
	"King of Spades",
	"King of Clubs",
	"King of Diamonds",
	"King of Hearts",
	"Ace of Spades",
	"Ace of Clubs",
	"Ace of Diamonds",
	"Ace of Hearts"
};

int card_points[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1 };

char* get_card_name(int card) {
	return card_names[card];
}

int get_card_points(int card) {
	return card_points[card / 4];
}
