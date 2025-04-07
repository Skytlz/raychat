#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "bpe.h"

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

typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} String;

void reserve_space_tokens(Tokens *tokens, size_t space) {
  if (space > tokens->capacity) {
    if (tokens->capacity == 0) {
      tokens->capacity = 256;
    }
    while (space > tokens->capacity) {
      tokens->capacity *= 2;
    }
    tokens->items =
        realloc(tokens->items, tokens->capacity * sizeof(*tokens->items));
    if (tokens->items == NULL) {
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_tokens(Tokens *tokens, uint32_t t) {
  reserve_space_tokens(tokens, tokens->count + 1);
  tokens->items[tokens->count++] = t;
}

void reserve_space_freqs(Frequencys *freqs, size_t space) {
  if (space > freqs->capacity) {
    if (freqs->capacity == 0) {
      freqs->capacity = 256;
    }
    while (space > freqs->capacity) {
      freqs->capacity *= 2;
    }
    freqs->items =
        realloc(freqs->items, freqs->capacity * sizeof(*freqs->items));
    if (freqs->items == NULL) {
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_freqs(Frequencys *freqs, Frequency f) {
  reserve_space_freqs(freqs, freqs->count + 1);
  freqs->items[freqs->count++] = f;
}

void render_tokens(Pairs pairs, Tokens tokens) {
  for (uint32_t i = 0; i < tokens.count; ++i) {
    uint32_t token = tokens.items[i];
    // printf("%d < %d\n", token, pairs.count);
    assert(token < pairs.count);
    if (pairs.items[token].l == token) {
      printf("%c", token);
    } else {
      printf("[%u]", token);
    }
  }
  printf("\n");
}

#define swap(Type, a, b)                                                       \
  do {                                                                         \
    Type t = (a);                                                              \
    (a) = (b);                                                                 \
    (b) = t;                                                                   \
  } while (0)

void generate_dot(Pairs pairs) {
  printf("digraph Pairs {\n");
  for (uint32_t i = 0; i < pairs.count; ++i) {
    if (i != pairs.items[i].l) {
      printf("	%zu -> %zu\n", i, pairs.items[i].l);
      printf("	%zu -> %zu\n", i, pairs.items[i].r);
    }
  }
  printf("}\n");
}

bool write_to_file(const char *filepath, const void *data, size_t size) {
  FILE *f = fopen(filepath, "wb");

  if (f == NULL) {
    printf("Could not open file");
    return false;
  }

  const char *buffer = data;
  while (size > 0) {
    size_t n = fwrite(buffer, 1, size, f);
    if (ferror(f)) {
      printf("Could not write to file");
      return false;
    }
    size -= n;
    buffer += n;
  }
  if (f)
    fclose(f);
  return true;
}

bool load_file(const char *filepath, String *string) {
  FILE *f = fopen(filepath, "rb");
  if (f == NULL)
    return false;
  if (fseek(f, 0, SEEK_END) < 0) {
    fclose(f);
    return false;
  }

  long m = ftell(f);
  if (m < 0) {
    fclose(f);
    return false;
  }
  if (fseek(f, 0, SEEK_SET) < 0) {
    fclose(f);
    return false;
  }

  size_t new_count = string->count + m; // m bytes = m strings
  if (new_count > string->capacity) {
    string->items = realloc(string->items, new_count);
    if (string->items == NULL) {
      fclose(f);
      return false;
    }
    string->capacity = new_count;
  }

  // Read m bytes into the buffer, starting after existing strings
  fread(string->items + string->count, m, 1, f);

  string->count = new_count;
  fclose(f);
  return true;
}

int main(int argc, char **argv) {
  if (argc < 0) {
    fprintf(stderr, "Usage %s <file.txt> <output.bin>\n", argv[0]);
    fprintf(stderr, "Error: No input provided\n");
    return 1;
  }
  const char *input_filepath = argv[1];

  if (argc < 1) {
    fprintf(stderr, "Usage  %s <file.txt> <output.bin>\n", argv[0]);
    fprintf(stderr, "Error: No output provided\n");
    return 1;
  }
  const char *output_filepath = argv[2];

  Frequency *frequency = NULL;
  Tokens tokens_in = {0};
  Tokens tokens_out = {0};
  String text = {0};
  Pairs pairs = {0};

  if (!load_file(input_filepath, &text)) {
    fprintf(stderr, "Failed to load file\n");
    return 1;
  }

  for (uint32_t i = 0; i < 256; ++i) {
    append_pairs(&pairs, ((Pair){.l = i}));
  }

  for (int i = 0; i < text.count; ++i) {
    append_tokens(&tokens_in, text.items[i]);
  }

  // printf("%d\n", tokens_in.count);
  for (;;) {
    // render_tokens(pairs, tokens_in);
    hmfree(frequency);
    for (size_t i = 0; i < tokens_in.count - 1; ++i) {
      Pair pair = {.l = tokens_in.items[i], .r = tokens_in.items[i + 1]};
      ptrdiff_t i = hmgeti(frequency, pair);
      if (i < 0)
        hmput(frequency, pair, 1);
      else
        frequency[i].value += 1;
    }

    ptrdiff_t max_frequency_index = 0;
    for (ptrdiff_t i = 1; i < hmlen(frequency); ++i) {
      if (frequency[i].value > frequency[max_frequency_index].value) {
        max_frequency_index = i;
      }
    }

    if (frequency[max_frequency_index].value <= 1)
      break;

    append_pairs(&pairs, frequency[max_frequency_index].key);

    tokens_out.count = 0;
    for (size_t i = 0; i < tokens_in.count;) {
      if (i + 1 >= tokens_in.count) {
        append_tokens(&tokens_out, tokens_in.items[i]);
        i += 1;
      } else {
        Pair pair = {.l = tokens_in.items[i], .r = tokens_in.items[i + 1]};
        if (memcmp(&pair, &frequency[max_frequency_index].key, sizeof(pair)) ==
            0) {
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

  // printf("%d\n", tokens_out.count);

  // generate_dot(pairs);
  if (!write_to_file(output_filepath, pairs.items,
                     pairs.count * sizeof(*pairs.items))) {
    fprintf(stderr, "Failed to write to output");
    return 1;
  }
  //  Frequencys sorted_frequency = {.count = 0, .capacity = 0};

  //  for(ptrdiff_t i = 0; i < hmlen(frequency); i++){
  //    append_freqs(&sorted_frequency, frequency[i]);
  //  }

  //  qsort(sorted_frequency.items, sorted_frequency.count,
  //  sizeof(*sorted_frequency.items), compare_frequencys);

  //  for(size_t i = 0; i < 10; ++i) {
  //    Frequency *f = &sorted_frequency.items[i];
  //    printf("(%u, %u) => %zu\n", f->key.l, f->key.r, f->value);
  //  }

  return 0;
}
