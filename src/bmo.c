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

int strfind(char *text, size_t tlen, char *pattern, size_t plen) {
    // Boyer–Moore string search algorithm

    

    // bad character rule
    int bchar[256];
    for (int i = 0; i < 127; ++i) bchar[i] = -1;
    for (int i = 0; i < (int) plen-1; ++i) bchar[(int) pattern[i]] = i;



    // good suffix rule
    int borders[plen+1];
    int gsuff[plen];
    for (int i = 0; i < (int) plen; ++i) {
        gsuff[i] = 0;
    }
    int i = plen;
    int j = plen + 1;
    borders[i] = j;

    while (i > 0) {
        while (j <= (int) plen && pattern[i-1] != pattern[j-1]) {
            if ((j-1) >= 0 && gsuff[j-1] == 0) {
                gsuff[j-1] = j-i;
            }
            j = borders[j];
        }
        --i;
        --j;
        borders[i] = j;
    }

    int b = borders[0];

    for (i = 0; i < (int) plen; ++i) {
        if (i == b) {
            b = borders[b];
        }
        if (gsuff[i] == 0) {
            gsuff[i] = b;
        }
    }
    


    // actual string search
    int s = 0;
    while (s <= (int) (tlen-plen)) {
        // printf("Starting idx %d\n", i);

        // start matching from back
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char textc = text[s+j];
            // printf("\tComparing \'%c\' and \'%c\'\n", srcc, pattern[j]);
            if (textc != pattern[j]) {
                int bchar_jump = j - bchar[(int) textc] > 1 ? j - bchar[(int) textc] > 1 : 1;
                jump = bchar_jump > gsuff[j] ? bchar_jump : gsuff[j];
                // printf("\tJumping %d!\n", jump);
                break;
            }
        }

        // if !jump, match found
        if (!jump) {
            return s;
        }
        s += jump;
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
