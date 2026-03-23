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
    // Brute force string search algorithm
    for (int i = 0; i < (int) (slen-plen+1); ++i) {
        bool match = true;
        for (int j = 0; j < (int) plen; ++j) {
            if (src[i+j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./dmb <pattern> <file>\n");
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