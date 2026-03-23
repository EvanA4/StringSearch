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

int strfind(char *src, int slen, char *pattern, int plen) {
    // Boyer–Moore string search algorithm

    

    // bad character rule
    int bchar[127];
    for (int i = 0; i < 127; ++i) {
        bchar[i] = plen;
    }
    for (int i = 0; i < plen-1; ++i) {
        bchar[(int) pattern[i]] = plen-i-1;
    }



    // good suffix rule
    int borders[plen+1];
    int gsuff[plen];
    for (int i = 0; i < plen; ++i) {
        gsuff[i] = 0;
    }
    int i = plen;
    int j = plen + 1;
    borders[i] = j;

    while (i > 0) {
        while (j <= plen && pattern[i-1] != pattern[j-1]) {
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

    for (i = 0; i < plen; ++i) {
        if (i == b) {
            b = borders[b];
        }
        if (gsuff[i] == 0) {
            gsuff[i] = b;
        }
    }
    


    // actual string search
    for (i = 0; i < slen-plen+1; ++i) {
        // printf("Starting idx %d\n", i);

        // start matching from back
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char srcc = src[i+j];
            // printf("\tComparing \'%c\' and \'%c\'\n", srcc, pattern[j]);
            if (srcc != pattern[j]) {
                int ic = srcc;
                jump = bchar[ic] > gsuff[j] ? bchar[ic] : gsuff[j];
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
        fprintf(stderr, "usage: ./bmo <pattern> <file>\n");
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