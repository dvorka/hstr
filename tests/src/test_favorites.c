/*
 ============================================================================
 Name        : test_favorites.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include "../../src/include/hstr_favorites.h"

int main(int argc, char *argv[])
{
	FavoriteItems favoriteItems;

	favorites_init(&favoriteItems);
	favorites_get(&favoriteItems);
	favorites_remove(&favoriteItems, "c");
}
