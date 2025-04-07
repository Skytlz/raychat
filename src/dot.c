#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "bpe.h"

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

void write_dot(const char *filepath, Pairs pairs) {

  FILE *f = fopen(filepath, "wb");

  if (f == NULL)
    return;

  fprintf(f, "digraph Pairs {\n");
  for (uint32_t i = 0; i < pairs.count; ++i) {
    if (i != pairs.items[i].l) {
      fprintf(f, "	%zu -> %zu\n", i, pairs.items[i].l);
      fprintf(f, "	%zu -> %zu\n", i, pairs.items[i].r);
    }
  }
  fprintf(f, "}\n");

  if (f)
    fclose(f);
}

int main(int argc, char **argv) {

  if (argc <= 0) {
    fprintf(stderr, "Usage %s <file.bin> <output.dot>\n", argv[0]);
    fprintf(stderr, "Error: No input provided\n");
    return 1;
  }
  const char *input_filepath = argv[1];

  if (argc <= 1) {
    fprintf(stderr, "Usage %s <file.bin> <output.dot>\n", argv[0]);
    fprintf(stderr, "Error: No input provided\n");
    return 1;
  }
  const char *output_filepath = argv[2];

  Pairs pairs = {0};

  if (!load_file(input_filepath, &pairs))
    return 1;

  write_dot(output_filepath, pairs);

  printf("INFO: generated %s\n.", output_filepath);
  return 0;
}