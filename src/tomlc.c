
#include "tomlc.h"
#include "block.h"

#include "../tomlc99/toml.h"

const char *delimiter = "  ";
int delimiter_len;

int max_block_output_length = 45;
int clickable_blocks = 1;
int leading_delimiter = 0;
int trailing_delimiter = 0;

int n_blocks = 0;
block **c_blocks;

int cfg_read_str(toml_table_t *conf, char *key, const char **dest) {
    toml_datum_t d = toml_string_in(conf, key);
    if (!d.ok)
        return 0;
    *dest = d.u.s;
    return 1;
}

void cfg_read_int(toml_table_t *conf, char *key, int *dest) {
    toml_datum_t d = toml_int_in(conf, key);
    if (!d.ok)
        return;
    *dest = d.u.i;
}

void read_cfgfile() {
    const char *config_file = strcat(getenv("XDG_CONFIG_HOME"), "/dwm/blocks.toml");
    FILE *fp = fopen(config_file, "r");
    if (fp) {
        char errbuf[200];
        toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
        fclose(fp);
        if (conf) {
            cfg_read_int(conf, "max_block_output_length", &max_block_output_length);
            cfg_read_int(conf, "clickable_blocks", &clickable_blocks);
            cfg_read_int(conf, "leading_delimiter", &leading_delimiter);
            cfg_read_int(conf, "trailing_delimiter", &trailing_delimiter);
            cfg_read_str(conf, "delimiter", &delimiter);
            delimiter_len = strlen(delimiter);

            /* blocks */
            toml_array_t *d = toml_array_in(conf, "blocks");
            n_blocks = toml_array_nelem(d);
            c_blocks = malloc(n_blocks * sizeof(block));
            const char *command = "";
            int interval = 0;
            int signal = 0;
            for (int i = 0; i < n_blocks; i++) {
                toml_table_t *tbl = toml_table_at(d, i);
                if (!tbl)
                    continue;
                cfg_read_str(tbl, "command", &command);
                cfg_read_int(tbl, "interval", &interval);
                cfg_read_int(tbl, "signal", &signal);
                c_blocks[i] = block_new(command, interval, signal);
            }
            toml_free(conf);
        }
        fprintf(stderr, errbuf);
    } else {
        fprintf(stderr, "error: could not open config file %s\n", config_file);
    }
}

