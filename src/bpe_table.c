#include "bpe.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool load_file(const char *filepath, Pairs *pairs) {

  if (strcmp("pairs.bin", filepath) != 0)
    return false;

  FILE *f = fopen(filepath, "rb");
  if (f == NULL)
    return false;
  if (fseek(f, 0, SEEK_END) < 0)
    return false;

  long m = ftell(f);

  if (m < 0)
    return false;
  if (fseek(f, 0, SEEK_SET) < 0)
    return false;

  Pairs loaded_pairs = {0};
  size_t new_count = loaded_pairs.count + m;

  if (new_count > loaded_pairs.capacity) {
    loaded_pairs.items = realloc(loaded_pairs.items, new_count);
    assert(loaded_pairs.items != NULL);
    loaded_pairs.capacity = new_count;
  }

  fread(loaded_pairs.items + loaded_pairs.count, m, 1, f);
  if (ferror(f))
    return false;

  loaded_pairs.count = new_count;

  if (f)
    fclose(f);

  if (loaded_pairs.count % sizeof(*pairs->items) != 0) {
    fprintf(stderr, "ERROR: size of %s (%zu) must be divisible by %zu\n",
            filepath, loaded_pairs.count, sizeof(*pairs->items));
    return false;
  }

  size_t items_count = loaded_pairs.count / sizeof(*pairs->items);
  for (size_t i = 0; i < items_count; ++i) {
    append_pairs(pairs, loaded_pairs.items[i]);
  }

  return true;
}

typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} String;

void reserve_space_str(String *string, size_t space) {
  if (space > string->capacity) {
    if (string->capacity == 0) {
      string->capacity = 256;
    }
    while (space > string->capacity) {
      string->capacity *= 2;
    }
    string->items =
        realloc(string->items, string->capacity * sizeof(*string->items));
    if (string->items == NULL) {
      printf("FAILURE TO REALLOCATE");
      return;
    }
  }
}

void append_str(String *string, char c) {
  reserve_space_str(string, string->count + 1);
  string->items[string->count++] = c;
}

void render_token(uint32_t token, Pairs pairs, String *string) {
  if (token == pairs.items[token].l) {
    append_str(string, (char)token);
    return;
  }

  render_token(pairs.items[token].l, pairs, string);
  render_token(pairs.items[token].r, pairs, string);
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

  Pairs pairs = {0};
  String str = {0};

  if (!load_file(input_filepath, &pairs))
    return 1;

  for (uint32_t token = 1; token < pairs.count; ++token) {
    printf("%u => |", token);
    str.count = 0;
    render_token(token, pairs, &str);
    // append_str(&str, (char)0);
    for (size_t i = 0; i < str.count; ++i) {
      if (isprint(str.items[i])) {
        printf("%c", str.items[i]);
      } else {
        printf("\\x%02X", (uint8_t)str.items[i]);
      }
    }
    printf("|\n");
  }

  return 0;
}