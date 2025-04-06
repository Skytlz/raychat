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


void reserve_space_pairs(Pairs* pairs, size_t space)
{
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

void append_pairs(Pairs* pairs, Pair p)
{
  reserve_space_pairs(pairs, pairs->count + 1);
  pairs->items[pairs->count++] = p;
}

void reserve_space_tokens(Tokens* tokens, size_t space)
{
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

void append_tokens(Tokens* tokens, uint32_t t)
{
  reserve_space_tokens(tokens, tokens->count + 1);
  tokens->items[tokens->count++] = t;
}

void reserve_space_freqs(Frequencys* freqs, size_t space)
{
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

void append_freqs(Frequencys* freqs, Frequency f)
{
  reserve_space_freqs(freqs, freqs->count + 1);
  freqs->items[freqs->count++] = f;
}


void render_tokens(Pairs pairs, Tokens tokens)
{
  for (size_t i = 0; i < tokens.count; ++i) {
    uint32_t token = tokens.items[i];
    //printf("%d < %d\n", token, pairs.count);
    assert(token < pairs.count);
    if (pairs.items[token].l == token) {
      printf("%c", token);
    } else {
      printf("[%u]", token);
    }
  }
  printf("\n");
}

#define swap(Type, a, b) \
  do { \
    Type t = (a); \
    (a) = (b); \
    (b) = t; \
  } while(0)

void generate_dot(Pairs pairs)
{
  printf("digraph Pairs {\n");
  for(size_t i = 0; i < pairs.count; ++i) {
    if(i != pairs.items[i].l) {
      printf("	%zu -> %zu\n", i, pairs.items[i].l);
      printf("	%zu -> %zu\n", i, pairs.items[i].r);
    }
  }
  printf("}\n");
}

int main(void)
{
  char * text = "The therapeutic effects of increasing carbon dioxide are being more widely recognized in recent years. Even Jane Brody, the NY Times writer on health topics, has favorably mentioned the use of the Buteyko method for asthma, and the idea of \"permissive hypercapnia\" during mechanical ventilation, to prevent lung damage from excess oxygen, has been discussed in medical journals. But still very few biologists recognize its role as a fundamental, universal protective factor. I think it will be helpful to consider some of the ways carbon dioxide might be controlling situations that otherwise are poorly understood.";
  int text_size = strlen(text);

  Frequency *frequency = NULL;
  Tokens tokens_in = {0};
  Tokens tokens_out = {0};
  Pairs pairs = {0};

  for (uint32_t i = 0; i < 256; ++i) {
    append_pairs(&pairs, ((Pair) {.l = i}));
  }

  for (int i = 0; i < text_size; ++i) {
    append_tokens(&tokens_in, text[i]);
  }

  for(;;) {
    //render_tokens(pairs, tokens_in);
    hmfree(frequency);
    for (size_t i = 0; i < tokens_in.count - 1; ++i) {
      Pair pair = {.l = tokens_in.items[i], .r = tokens_in.items[i + 1]};
      ptrdiff_t i = hmgeti(frequency, pair);
      if (i < 0) hmput(frequency, pair, 1);
      else frequency[i].value += 1;
    }

    ptrdiff_t max_frequency_index = 0;
    for (ptrdiff_t i = 1; i < hmlen(frequency); ++i) {
      if (frequency[i].value > frequency[max_frequency_index].value) {
        max_frequency_index = i;
      }
    }

    if (frequency[max_frequency_index].value <= 1) break;

    append_pairs(&pairs, frequency[max_frequency_index].key);

    tokens_out.count = 0;
    for (size_t i = 0; i < tokens_in.count;) {
       if (i + 1 >= tokens_in.count) {
         append_tokens(&tokens_out, tokens_in.items[i]);
         i += 1;
       } else {
         Pair pair = {.l = tokens_in.items[i], .r = tokens_in.items[i + 1]};
         if (memcmp(&pair, &frequency[max_frequency_index].key, sizeof(pair)) == 0) {
           append_tokens(&tokens_out, pairs.count - 1);
           i += 2;
         } else {
           append_tokens(&tokens_out, tokens_in.items[i]);
           i += 1;
         }
       }
    }
    swap(Tokens, tokens_in, tokens_out);
  }

  generate_dot(pairs);

//  Frequencys sorted_frequency = {.count = 0, .capacity = 0};

//  for(ptrdiff_t i = 0; i < hmlen(frequency); i++){
//    append_freqs(&sorted_frequency, frequency[i]);
//  }

//  qsort(sorted_frequency.items, sorted_frequency.count, sizeof(*sorted_frequency.items), compare_frequencys);

//  for(size_t i = 0; i < 10; ++i) {
//    Frequency *f = &sorted_frequency.items[i];
//    printf("(%u, %u) => %zu\n", f->key.l, f->key.r, f->value);
//  }

  return 0;
}
