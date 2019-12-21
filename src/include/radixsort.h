/*
 radixsort.h        header file for radix sort implementation

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef RADIXSORT_H
#define RADIXSORT_H

#include <math.h>
#include <stddef.h>

#include "hstr_utils.h"

#define RADIX_SLOT_SIZE 1000

#define RADIX_BIG_KEYS_SKIP     0
#define RADIX_BIG_KEYS_FLOOR    1
#define RADIX_BIG_KEYS_EXIT     2

#define RADIX_DEBUG_LEVEL_NONE  0
#define RADIX_DEBUG_LEVEL_WARN  1
#define RADIX_DEBUG_LEVEL_DEBUG 2

typedef struct radixitem {
    unsigned key;
    void* data;
    struct radixitem* next;
} RadixItem;

typedef struct radixslot {
    unsigned min;
    unsigned max;
    unsigned size;
} RadixSlot;

typedef struct {
    unsigned size;
    unsigned maxKey;
    unsigned keyLimit;
    RadixItem*** topDigits;

    int optionBigKeys;

    RadixSlot** _slotDescriptors;
    unsigned _slotsCount;
    unsigned _topIndexLimit;
    unsigned _debug;
} RadixSorter;

void radixsort_init(RadixSorter* rs, unsigned keyLimit);
void radixsort_set_debug_level(RadixSorter* rs, unsigned debugLevel);
void radixsort_add(RadixSorter* rs, RadixItem* item);
RadixItem* radix_cut(RadixSorter* rs, unsigned key, void* data);
RadixItem** radixsort_dump(RadixSorter* rs);
void radixsort_destroy(RadixSorter* rs);
void radixsort_stat(RadixSorter* rs, bool listing);

#endif
