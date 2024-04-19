#ifndef TOMLC_H
#define TOMLC_H

#include "../tomlc99/toml.h"
#include "block.h"
#include <stdlib.h>
#include <string.h>



extern const char *delimiter;
extern int delimiter_len;

extern int max_block_output_length;
extern int clickable_blocks;
extern int leading_delimiter;
extern int trailing_delimiter;

extern int n_blocks;
extern block **c_blocks;

void read_cfgfile();

int cfg_read_str(toml_table_t *conf, char *key, const char **dest);
void cfg_read_float(toml_table_t *conf, char *key, float *dest);
void cfg_read_int(toml_table_t *conf, char *key, int *dest);
int cfg_read_strarr(toml_table_t *conf, char *key, const char ***dest, int ins_null);

#endif  // TOMLC_H
