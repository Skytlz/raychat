#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct {
  char pair[2];
} Pair;

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

Frequency *frequency = NULL;

void reserve_space(Frequencys* freqs, size_t space) {
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
  reserve_space(freqs, freqs->count + 1);
  freqs->items[freqs->count++] = f;
}


int main(void){
  char * text = "The therapeutic effects of increasing carbon dioxide are being more widely recognized in recent years. Even Jane Brody, the NY Times writer on health topics, has favorably mentioned the use of the Buteyko method for asthma, and the idea of “permissive hypercapnia” during mechanical ventilation, to prevent lung damage from excess oxygen, has been discussed in medical journals. But still very few biologists recognize its role as a fundamental, universal protective factor. I think it will be helpful to consider some of the ways carbon dioxide might be controlling situations that otherwise are poorly understood.";
  int text_size = strlen(text);

  for(int i = 0; i < text_size - 1; ++i){
    Pair pair = {.pair = {text[i], text[i + 1]}};
    ptrdiff_t i = hmgeti(frequency, pair);
    if (i < 0) hmput(frequency, pair, 1);
    else frequency[i].value += 1;
  }

  Frequencys sorted_frequency = {.count = 0, .capacity = 0};

  for(ptrdiff_t i = 0; i < hmlen(frequency); i++){
    append_freqs(&sorted_frequency, frequency[i]);
  }

  qsort(sorted_frequency.items, sorted_frequency.count, sizeof(*sorted_frequency.items), compare_frequencys);

  for(size_t i = 0; i < sorted_frequency.count; ++i) {
    Frequency *f = &sorted_frequency.items[i];
    printf("%c%c => %zu\n", f->key.pair[0], f->key.pair[1], f->value);
  }
  return 0;
}
