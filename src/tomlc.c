
#include "tomlc.h"
#include "block.h"

#include "../tomlc99/toml.h"

#define PATH_MAX        4096	/* # chars in a path name including nul */

const char *delimiter = NULL;
int delimiter_len;

int max_block_output_length = 45;
int clickable_blocks = 1;
int leading_delimiter = 0;
int trailing_delimiter = 0;

int n_blocks = 0;
block **c_blocks;

int cfg_read_str(toml_table_t *conf, char *key, const char **dest)
{
    toml_datum_t d = toml_string_in(conf, key);
    if (!d.ok) {
        return 0;
    }
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
    const char *suffix = "/dwm/blocks.toml";
    char path[PATH_MAX];
    const char *xdg = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");

    if (xdg && xdg[0] != '\0') {
        /* use xdg_config_home if set */
        snprintf(path, sizeof(path), "%s%s", xdg, suffix);
    } else if (home && home[0] != '\0') {
        /* fallback to ~/.config */
        snprintf(path, sizeof(path), "%s/.config%s", home, suffix);
    }

    FILE *fp = fopen(path, "r");
    if (fp) {
        char errbuf[200];
        toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
        fclose(fp);
        if (conf) {
            cfg_read_int(conf, "max_block_output_length", &max_block_output_length);
            cfg_read_int(conf, "clickable_blocks", &clickable_blocks);
            cfg_read_int(conf, "leading_delimiter", &leading_delimiter);
            cfg_read_int(conf, "trailing_delimiter", &trailing_delimiter);

            const char *tmp_delim = NULL;
            if (cfg_read_str(conf, "delimiter", &tmp_delim)) {
                delimiter = tmp_delim;
            } else {
                delimiter = strdup("  ");
            }
            delimiter_len = strlen(delimiter);

            /* blocks */
            toml_array_t *d = toml_array_in(conf, "blocks");
            n_blocks = toml_array_nelem(d);
            c_blocks = malloc(n_blocks * sizeof(block *));
            for (int i = 0; i < n_blocks; i++) {
                const char *command = "";
                int interval = 0;
                int signal = 0;
                const char *tmp_cmd = NULL;
                toml_table_t *tbl = toml_table_at(d, i);
                if (!tbl)
                    continue;
                if (cfg_read_str(tbl, "command", &tmp_cmd)) {
                    command = tmp_cmd;
                }
                cfg_read_int(tbl, "interval", &interval);
                cfg_read_int(tbl, "signal", &signal);
                c_blocks[i] = block_new(command, interval, signal);

                if (tmp_cmd) free(tmp_cmd);
            }
            toml_free(conf);
        }
        fprintf(stderr, errbuf);
    } else {
        fprintf(stderr, "error: could not open config file %s\n", path);
    }
}

