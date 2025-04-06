#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct {
  uint32_t l, r;
} Pair;

typedef struct {
  Pair *items;
  size_t count;
  size_t capacity;
} Pairs;

typedef struct {
  Pair key;
  size_t value;
} Frequency;

int compare_frequencys(const void *a, const void *b) {
  const Frequency *af = a;
  const Frequency *bf = b;
  return (int)bf->value - (int)af->value;
}

typedef struct {
  Frequency *items;
  size_t count;
  size_t capacity;
} Frequencys;


typedef struct {
  uint32_t *items;
  size_t count;
  size_t capacity;
} Tokens;


void reserve_space_pairs(Pairs* pairs, size_t space) {
  if (space > pairs->capacity) {
    if(pairs->capacity == 0) {
      pairs->capacity = 256;
    }
    while(space > pairs->capacity) {
       pairs->capacity *= 2;
    }
    pairs->items = realloc(pairs->items, pairs->capacity * sizeof(*pairs->items));
    if(pairs->items == NULL){
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_pairs(Pairs* pairs, Pair p) {
  reserve_space_pairs(pairs, pairs->count + 1);
  pairs->items[pairs->count++] = p;
}

void reserve_space_tokens(Tokens* tokens, size_t space) {
  if (space > tokens->capacity) {
    if(tokens->capacity == 0) {
      tokens->capacity = 256;
    }
    while(space > tokens->capacity) {
       tokens->capacity *= 2;
    }
    tokens->items = realloc(tokens->items, tokens->capacity * sizeof(*tokens->items));
    if(tokens->items == NULL){
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_tokens(Tokens* tokens, uint32_t t) {
  reserve_space_tokens(tokens, tokens->count + 1);
  tokens->items[tokens->count++] = t;
}

void reserve_space_freqs(Frequencys* freqs, size_t space) {
  if (space > freqs->capacity) {
    if(freqs->capacity == 0) {
      freqs->capacity = 256;
    }
    while(space > freqs->capacity) {
       freqs->capacity *= 2;
    }
    freqs->items = realloc(freqs->items, freqs->capacity * sizeof(*freqs->items));
    if(freqs->items == NULL){
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_freqs(Frequencys* freqs, Frequency f) {
  reserve_space_freqs(freqs, freqs->count + 1);
  freqs->items[freqs->count++] = f;
}


int main(void){
  char * text = "The therapeutic effects of increasing carbon dioxide are being more widely recognized in recent years. Even Jane Brody, the NY Times writer on health topics, has favorably mentioned the use of the Buteyko method for asthma, and the idea of “permissive hypercapnia” during mechanical ventilation, to prevent lung damage from excess oxygen, has been discussed in medical journals. But still very few biologists recognize its role as a fundamental, universal protective factor. I think it will be helpful to consider some of the ways carbon dioxide might be controlling situations that otherwise are poorly understood.";
  int text_size = strlen(text);

  Frequency *frequency = NULL;
  Tokens tokens = {0};
  Pairs pairs = {0};

  for(uint32_t i = 0; i < 256; ++i) {
    append_pairs(&pairs, ((Pair) {.l = i}));
  }

  for(int i = 0; i < text_size - 1; ++i) {
    append_tokens(&tokens, text[i]);
  }

  for(size_t i = 0; i < tokens.count - 1; ++i) {
    Pair pair = {.l = tokens.items[i], .r = tokens.items[i + 1]};
    ptrdiff_t i = hmgeti(frequency, pair);
    if (i < 0) hmput(frequency, pair, 1);
    else frequency[i].value += 1;
  }

  Frequencys sorted_frequency = {.count = 0, .capacity = 0};

  for(ptrdiff_t i = 0; i < hmlen(frequency); i++){
    append_freqs(&sorted_frequency, frequency[i]);
  }

  qsort(sorted_frequency.items, sorted_frequency.count, sizeof(*sorted_frequency.items), compare_frequencys);

  for(size_t i = 0; i < 10; ++i) {
    Frequency *f = &sorted_frequency.items[i];
    printf("(%u, %u) => %zu\n", f->key.l, f->key.r, f->value);
  }
  return 0;
}
