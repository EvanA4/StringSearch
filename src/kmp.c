#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct ReadResult {
    char *data;
    size_t len;
    const char *err;
} ReadResult;

ReadResult readFile(char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        ReadResult rr = {NULL, 0, "Failed to get file statistics."};
        return rr;
    }
    size_t fsize = st.st_size;
    FILE *fptr;
    if (!(fptr = fopen(path, "r"))) {
        ReadResult rr = { NULL, 0, "Failed to open file."};
        return rr;
    }

    char *output = (char *) malloc(fsize+1);
    size_t nread = fread(output, 1, fsize, fptr);
    output[nread] = '\0';

    fclose(fptr);

    ReadResult rr = { output, nread, NULL };
    return rr;
}

int strfind(char *src, size_t slen, char *pattern, size_t plen) {
    // Knuth–Morris–Pratt string search algorithm


    // construct failure links
    int *fail = (int *) malloc(sizeof(int) * plen);
    fail[0] = 0;
    int x = 0;
    for (int q = 1; q < (int) plen; ++q) {
        fail[q] = x;
        while (pattern[q] != pattern[x]) {
            if (x == 0) {
                x = -1;
                break;
            }
            x = fail[x];
        }
        ++x;
    }
    // for (int i = 0; i < (int) plen; ++i) {
    //     printf("[%d]: %d\n", i, fail[i]);
    // }


    // actual search algorithm
    int i = 0;
    int q = 0;
    while (i < (int) slen) {
        if (src[i] == pattern[q]) {
            ++i;
            ++q;
            if (q == (int) plen) {
                free(fail);
                return i-q;
            }
        } else if (q) {
            q = fail[q];
        } else {
            ++i;
        }
    }


    free(fail);
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./bmh <pattern> <file>\n");
        return 1;
    }

    char *pattern = argv[1];
    ReadResult toSearch = readFile(argv[2]);
    if (toSearch.err) {
        fprintf(stderr, "Error: %s\n", toSearch.err);
        return 1;
    }

    printf("%d\n", strfind(
        toSearch.data,
        toSearch.len,
        pattern,
        strlen(pattern)
    ));

    free(toSearch.data);
}
