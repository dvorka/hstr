/*
 ============================================================================
 Name        : radixsort.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Radix sort
 ============================================================================
*/

#include "include/radixsort.h"

#define GET_TOP_INDEX(KEY) KEY/SLOT_SIZE
#define GET_LOW_INDEX(KEY) KEY%SLOT_SIZE

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
	descriptor->size=0;

	rs->_slotDescriptors[topIndex]=descriptor;
	rs->_slotsCount++;
	return slot;
}

void radixsort_add(RadixSorter *rs, RadixItem *item) {
	unsigned topIndex = GET_TOP_INDEX(item->key);
	unsigned lowIndex = GET_LOW_INDEX(item->key);


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
	rs->_slotDescriptors[topIndex]->size++;
}

void radix_dec_slot_descriptor_size(RadixSorter *rs, RadixSlot *descriptor, unsigned key, unsigned topIndex) {
	descriptor->size--;
	if(!descriptor->size) {
		descriptor->min=rs->keyLimit;
		descriptor->max=0;
	} else {
		if(descriptor->size==1) {
			if(rs->topDigits[topIndex][GET_LOW_INDEX(descriptor->max)]) {
				descriptor->min=descriptor->max;
			} else {
				if(rs->topDigits[topIndex][GET_LOW_INDEX(descriptor->min)]) {
					descriptor->max=descriptor->min;
				}
			}
		}
	}
}

RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data) {
	// TODO optimization: fix min/max on cut of a value
	if(key<=rs->maxKey) {
		unsigned topIndex = GET_TOP_INDEX(key);
		unsigned lowIndex = GET_LOW_INDEX(key);

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
				radix_dec_slot_descriptor_size(rs, rs->_slotDescriptors[topIndex], key, topIndex);
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
		int slotMin, slotSize, slotCount, l;
		unsigned items=0;
		do {
			if(rs->topDigits[t]) {
				if(rs->_slotDescriptors[t]->size>0) {
					l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max);
					slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min);
					slotSize=rs->_slotDescriptors[t]->size;
					slotCount=0;
					do {
						if(rs->topDigits[t][l]) {
							result[items++]=rs->topDigits[t][l];
							slotCount++;
							RadixItem *ri=rs->topDigits[t][l]->next;
							while(ri) {
								result[items++]=ri;
								slotCount++;
								ri=ri->next;
							}
						}
					} while(--l>=slotMin && slotCount<slotSize);
				}
			}
		} while(--t>=0);
		return result;
	}
	return NULL;
}

void radixsort_stat(RadixSorter *rs) {
	printf("\n Radixsort (size/max/limit): %u %u %u", rs->size, rs->maxKey, rs->keyLimit);
	if(rs->size>0) {
		int t = GET_TOP_INDEX(rs->maxKey);
		int slotMin, slotSize, slotCount, l;
		unsigned items=0;
		do {
			if(rs->topDigits[t]) {
				printf("\n  Slot %u (size/min/max): %u %u %u",t, rs->_slotDescriptors[t]->size, rs->_slotDescriptors[t]->min, rs->_slotDescriptors[t]->max);
				if(rs->_slotDescriptors[t]->size>0) {
					l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max);
					slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min);
					slotSize=rs->_slotDescriptors[t]->size;
					slotCount=0;
					do {
						if(rs->topDigits[t][l]) {
							printf("\n    > %d #%u",l, ++items);
							++slotCount;
							RadixItem *ri=rs->topDigits[t][l]->next;
							while(ri) {
								printf(" #%u",++items);
								++slotCount;
								ri=ri->next;
							}
						}
					} while(--l>=slotMin && slotCount<slotSize);
					printf("\n    STOP @ %d",l);
				} else {
					printf(" > SKIPPED");
				}
			}
		} while(--t>=0);
	}
	fflush(stdout);
}

void radixsort_destroy(RadixSorter *rs) {
	// radix items: DONE (passed on dump() by reference)
	// rs: DONE (created and destroyed by caller)
	// slots:
	int topIndex = GET_TOP_INDEX(rs->maxKey);
	do {
		if(rs->topDigits[topIndex]) {
			free(rs->topDigits[topIndex]);
			free(rs->_slotDescriptors[topIndex]);
		}
	} while(--topIndex>=0);
}
