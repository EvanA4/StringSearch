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
    // Karp–Rabin string search algorithm
    // Precompute the powers of the base modulo the mod
    const int base = 256;
    const int mod = 1000000007;
    const int window_size = plen;
    const int n = slen;
    long long power = 1;
    for (int i = 1; i < window_size; i++) {
        power = (power * base) % mod;
    }

    // Compute hash of pattern and first window
    long long pattern_hash = 0;
    long long current_hash = 0;
    for (int i = 0; i < window_size; i++) {
        pattern_hash = (base * pattern_hash + pattern[i]) % mod;
        current_hash = (current_hash * base + src[i]) % mod;
    }

    // Compute the hash values of the rest of the substrings
    for (int i = 0; i <= n - window_size; i++) {
        // If hashes match, do a full string comparison
        if (current_hash == pattern_hash) {
            int j = window_size - 1;
            while (j >= 0 && pattern[j] == src[i + j]) --j;
            if (j < 0) {
                return i;
            }
        }
        
        // Compute next hash
        if (i < n - window_size) {
            current_hash = (base * (current_hash - src[i] * power)
                           + src[i+window_size]) % mod;
            if (current_hash < 0) current_hash += mod;
        }

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
