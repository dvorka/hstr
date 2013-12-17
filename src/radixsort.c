/*
 ============================================================================
 Name        : radixsort.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Radix sort
 ============================================================================
*/

#include "include/radixsort.h"
#include "include/hstr_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#define GET_TOP_INDEX(KEY) (unsigned)trunc(((double)KEY)/(double)SLOT_SIZE);
#define GET_LOW_INDEX(KEY, TOPINDEX) KEY-TOPINDEX*SLOT_SIZE;

void radixsort_init(RadixSorter *rs, unsigned keyLimit) {
	unsigned topIndexLimit=GET_TOP_INDEX(keyLimit);
	rs->size=0;
	rs->topDigits=malloc(topIndexLimit * sizeof(RadixItem ***));
	memset(rs->topDigits, 0, topIndexLimit * sizeof(RadixItem ***));
	rs->maxKey=0;
	rs->keyLimit=keyLimit;

	rs->_slotDescriptors=malloc(topIndexLimit * sizeof(RadixSlot **));
	rs->_slotsCount=0;
}

RadixItem **radixsort_get_slot(RadixSorter *rs, unsigned topIndex) {
	RadixItem **slot=malloc(SLOT_SIZE * sizeof(RadixItem *));
	memset(slot, 0, SLOT_SIZE * sizeof(RadixItem *));

	RadixSlot *descriptor=malloc(sizeof(RadixSlot));
	descriptor->min=rs->keyLimit;
	descriptor->max=0;

	rs->_slotDescriptors[topIndex]=descriptor;
	rs->_slotsCount++;
	return slot;
}

void radixsort_add(RadixSorter *rs, RadixItem *item) {
	unsigned topIndex = GET_TOP_INDEX(item->key);
	unsigned lowIndex = GET_LOW_INDEX(item->key, topIndex);


	if(!rs->topDigits[topIndex]) {
		rs->topDigits[topIndex]=radixsort_get_slot(rs, topIndex);
	}

	RadixItem *chain=rs->topDigits[topIndex][lowIndex];
	rs->topDigits[topIndex][lowIndex]=item;
	if(chain==NULL) {
		item->next=NULL;
	} else {
		item->next=chain;
	}

	rs->size++;
	rs->maxKey=MAX(rs->maxKey,item->key);
	rs->_slotDescriptors[topIndex]->min=MIN(rs->_slotDescriptors[topIndex]->min,item->key);
	rs->_slotDescriptors[topIndex]->max=MAX(rs->_slotDescriptors[topIndex]->max,item->key);
}

RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data) {
	// TODO optimization: remove empty slots (or just set size to 0 & don't traverse it
	// TODO optimization: fix min/max on cut of a value
	if(key<=rs->maxKey) {
		unsigned topIndex = GET_TOP_INDEX(key);
		unsigned lowIndex = GET_LOW_INDEX(key, topIndex);

		if(rs->topDigits[topIndex]) {
			RadixItem *ri=rs->topDigits[topIndex][lowIndex];
			RadixItem *lastRi=NULL;
			while(ri->data!=data) {
				if(ri->next) {
					lastRi=ri;
					ri=ri->next;
				} else {
					break;
				}
			}
			if(ri->data==data) {
				if(lastRi) {
					lastRi->next=ri->next;
				} else {
					rs->topDigits[topIndex][lowIndex]=ri->next;
				}
				ri->next=NULL;
				rs->size--;
				return ri;
			}
		}
	}
	return NULL;
}

RadixItem **radixsort_dump(RadixSorter *rs) {
	if(rs->size>0) {
		RadixItem **result=malloc(rs->size * sizeof(RadixItem *));
		int t = GET_TOP_INDEX(rs->maxKey);
		int slotMin, l;
		unsigned items=0;
		do {
			l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max,t);
			slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min,t);
			do {
				if(rs->topDigits[t]) {
					if(rs->topDigits[t][l]) {
						result[items++]=rs->topDigits[t][l];
						RadixItem *ri=rs->topDigits[t][l]->next;
						while(ri) {
							result[items++]=ri;
							ri=ri->next;
						}
					}
				}
			} while(--l>=slotMin);
		} while(--t>=0);
		return result;
	}

	return NULL;
}

void radixsort_stat(RadixSorter *rs) {
	printf("\n Radixsort (size/max/limit): %u %u %u", rs->size, rs->maxKey, rs->keyLimit);
	if(rs->size>0) {
		int t = GET_TOP_INDEX(rs->maxKey);
		int l, slotMin;
		unsigned i=1;
		do {
			printf("\n  Slot %u (min/max): %u %u",t, rs->_slotDescriptors[t]->min, rs->_slotDescriptors[t]->max);
			l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max,t);
			slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min,t);
			if(rs->topDigits[t]) {
				do {
					if(rs->topDigits[t][l]) {
						printf("\n    > %d #%u",l, i++);
						RadixItem *ri=rs->topDigits[t][l]->next;
						while(ri) {
							printf(" #%u",i++);
							ri=ri->next;
						}
					}
				} while(--l>=slotMin);
			}
		} while(--t>=0);
	}
	fflush(stdout);
}

void radixsort_destroy(RadixSorter *rs) {
	// radix items: DONE (passed on dump() by reference)
	// rs: DONE (created and destroyed by caller)
	// slots:
	unsigned topIndex = GET_TOP_INDEX(rs->maxKey);
	do {
		if(rs->topDigits[topIndex]) {
			free(rs->topDigits[topIndex]);
			free(rs->_slotDescriptors[topIndex]);
		}
	} while(--topIndex>=0);
}
