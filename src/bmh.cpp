#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

struct ReadResult {
    char *data;
    size_t len;
    const char *err;
};

ReadResult readFile(char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return { NULL, 0, "Failed to get file statistics."};
    }
    size_t fsize = st.st_size;
    FILE *fptr;
    if (!(fptr = fopen(path, "r"))) {
        return { NULL, 0, "Failed to open file."};
    }

    char *output = (char *) malloc(fsize+1);
    size_t nread = fread(output, 1, fsize, fptr);
    output[nread] = '\0';

    fclose(fptr);

    return { output, nread, NULL };
}

int strfind(char *src, size_t slen, char *pattern, size_t plen) {
    // Boyer–Moore–Horspool string search algorithm
    int bchar[127];
    for (int i = 0; i < 127; ++i) {
        bchar[i] = plen;
    }
    for (int i = 0; i < (int) plen-1; ++i) {
        bchar[(int) pattern[i]] = plen-i-1;
    }

    for (int i = 0; i < (int) (slen-plen+1); ++i) {
        // printf("Starting idx %d\n", i);

        // start matching from back
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char srcc = src[i+j];
            // printf("\tComparing \'%c\' and \'%c\'\n", srcc, pattern[j]);
            if (srcc != pattern[j]) {
                jump = bchar[(int) srcc];
                // printf("\tJumping %d!\n", jump);
                break;
            }
        }

        // if !jump, match found
        if (!jump) {
            return i;
        }
        i += (jump-1);
    }

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