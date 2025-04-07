#ifndef BPE_H_
#define BPE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t l, r;
} Pair;

typedef struct {
  Pair *items;
  size_t count;
  size_t capacity;
} Pairs;

void reserve_space_pairs(Pairs *pairs, size_t space) {
  if (space > pairs->capacity) {
    if (pairs->capacity == 0) {
      pairs->capacity = 256;
    }
    while (space > pairs->capacity) {
      pairs->capacity *= 2;
    }
    pairs->items =
        realloc(pairs->items, pairs->capacity * sizeof(*pairs->items));
    if (pairs->items == NULL) {
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_pairs(Pairs *pairs, Pair p) {
  reserve_space_pairs(pairs, pairs->count + 1);
  pairs->items[pairs->count++] = p;
}

#endif // BPE_H_