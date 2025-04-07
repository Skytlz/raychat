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

#define reserve_space(a, c)                                                    \
  do {                                                                         \
    if ((c) > (a)->capacity) {                                                 \
      if ((a)->capacity == 0) {                                                \
        (a)->capacity = 256;                                                   \
      }                                                                        \
      while ((c) > (a)->capacity) {                                            \
        (a)->capacity *= 2;                                                    \
      }                                                                        \
      (a)->items = realloc((a)->items, (a)->capacity * sizeof(*(a)->items));   \
      assert((a)->items != NULL);                                              \
    }                                                                          \
  } while (0)

#define append(a, i)                                                           \
  do {                                                                         \
    reserve_space((a), (a)->count + 1);                                        \
    (a)->items[(a)->count++] = (i);                                            \
  } while (0)

#endif // BPE_H_