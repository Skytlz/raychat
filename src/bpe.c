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

Frequency *frequency = NULL;

int main(void){
  char * text = "The therapeutic effects of increasing carbon dioxide are being more widely recognized in recent years. Even Jane Brody, the NY Times writer on health topics, has favorably mentioned the use of the Buteyko method for asthma, and the idea of “permissive hypercapnia” during mechanical ventilation, to prevent lung damage from excess oxygen, has been discussed in medical journals. But still very few biologists recognize its role as a fundamental, universal protective factor. I think it will be helpful to consider some of the ways carbon dioxide might be controlling situations that otherwise are poorly understood.";
  int text_size = strlen(text);

  for(int i = 0; i < text_size - 1; ++i){
    Pair pair = {.pair = {text[i], text[i + 1]}};
    ptrdiff_t i = hmgeti(frequency, pair);
    if (i < 0) hmput(frequency, pair, 1);
    else frequency[i].value += 1;
  }

  for(ptrdiff_t i = 0; i < hmlen(frequency); ++i){
    printf("%c%c => %zu\n", frequency[i].key.pair[0], frequency[i].key.pair[1], frequency[i].value);
  }
  return 0;
}
