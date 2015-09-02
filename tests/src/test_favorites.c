/*
 test_*.c       HSTR test

 Copyright (C) 2014  Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "../../src/include/hstr_favorites.h"

int main(int argc, char *argv[])
{
    FavoriteItems favoriteItems;

    favorites_init(&favoriteItems);
    favorites_get(&favoriteItems);
    favorites_remove(&favoriteItems, "c");

    favorites_choose(&favoriteItems, "ccc");
}
