#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>

#define ssfn int (*)(char *, size_t, char *, size_t)
typedef struct Answer {
    char *pattern;
    int idx;
} Answer;


int bmh(char *src, size_t slen, char *pattern, size_t plen) {
    // Boyer–Moore–Horspool string search algorithm
    int bchar[127];
    for (int i = 0; i < 127; ++i) {
        bchar[i] = plen;
    }
    for (int i = 0; i < (int) plen-1; ++i) {
        bchar[(int) pattern[i]] = plen-i-1;
    }
    for (int i = 0; i < (int) (slen-plen+1); ++i) {
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char srcc = src[i+j];
            if (srcc != pattern[j]) {
                jump = bchar[(int) srcc];
                break;
            }
        }
        if (!jump) {
            return i;
        }
        i += (jump-1);
    }
    return -1;
}


int bmo(char *src, size_t slen_x, char *pattern, size_t plen_x) {
    int slen = slen_x;
    int plen = plen_x;
    // Boyer–Moore string search algorithm
    int bchar[127];
    for (int i = 0; i < 127; ++i) {
        bchar[i] = plen;
    }
    for (int i = 0; i < plen-1; ++i) {
        bchar[(int) pattern[i]] = plen-i-1;
    }
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
    for (i = 0; i < slen-plen+1; ++i) {
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char srcc = src[i+j];
            if (srcc != pattern[j]) {
                int ic = srcc;
                jump = bchar[ic] > gsuff[j] ? bchar[ic] : gsuff[j];
                break;
            }
        }
        if (!jump) {
            return i;
        }
        i += (jump-1);
    }
    return -1;
}


int dmb(char *src, size_t slen, char *pattern, size_t plen) {
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


int kmp(char *src, size_t slen, char *pattern, size_t plen) {
    // Knuth–Morris–Pratt string search algorithm
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


int kra(char *src, size_t slen, char *pattern, size_t plen) {
    // Karp–Rabin string search algorithm
    const int base = 256;
    const int mod = 1000000007;
    const int window_size = plen;
    const int n = slen;
    long long power = 1;
    for (int i = 1; i < window_size; i++) {
        power = (power * base) % mod;
    }
    long long pattern_hash = 0;
    long long current_hash = 0;
    for (int i = 0; i < window_size; i++) {
        pattern_hash = (base * pattern_hash + pattern[i]) % mod;
        current_hash = (current_hash * base + src[i]) % mod;
    }
    for (int i = 0; i <= n - window_size; i++) {
        if (current_hash == pattern_hash) {
            int j = window_size - 1;
            while (j >= 0 && pattern[j] == src[i + j]) --j;
            if (j < 0) {
                return i;
            }
        }
        if (i < n - window_size) {
            current_hash = (base * (current_hash - src[i] * power)
                           + src[i+window_size]) % mod;
            if (current_hash < 0) current_hash += mod;
        }
    }
    return -1;
}


char *randstr(int len, bool allow_newline) {
    char *text = (char *) malloc(sizeof(char) * len + 1);
    for (int i = 0; i < len; ++i) {
        char toAdd = '\0';
        while (toAdd == '\0' || toAdd == '\'' || toAdd == '\"' || toAdd == '`') {
            toAdd = rand() % (allow_newline ? 96 : 95) + (allow_newline ? 31 : 32);
        }
        text[i] = toAdd == 31 ? '\n' : toAdd;
    } text[len] = '\0';
    return text;
}


Answer randsub(char *text, int flen, int plen) {
    // 10% chance to create fake, m-length substring
    // 90% chance to take random, m-length substring from file

    bool do_fake = rand() % 10 == 1;
    if (do_fake) {
        // randomly generate substring
        // ensure it doesn't exist in text
        char *pattern = randstr(plen, false);
        while (bmh(text, flen, pattern, plen) != -1) {
            free(pattern);
            pattern = randstr(plen, false);
        }

        Answer ans = { pattern, -1 };
        return ans;

    } else {
        // randomly generate idx
        // extract substring
        int idx = rand() % (flen - plen);
        char *substr = (char *) malloc(plen+1);
        substr[plen] = '\0';
        memmove(substr, text + idx, plen);

        Answer ans = { substr, idx };
        return ans;
    }

    Answer ans = { NULL, -1 };
    return ans;
}


int main(int argc, char **argv) {
    // create t, n-length files
    // 10% chance to create fake, m-length substring
    // 90% chance to take random, m-length substring from file
    // write .sh file to output time to complete all test cases
    // output solutions to stdout

    // verify and parse input
    if (argc != 6) {
        printf("usage: ./tests <number of tests> <test length> <tests per step> <pattern max length> <pattern step>\n");
        return 1;
    }
    const int N_TESTS = atoi(argv[1]);
    const int T_LEN = atoi(argv[2]);
    const int P_STEP_LEN = atoi(argv[3]);
    const int P_MAX_LEN = atoi(argv[4]);
    const int P_STEP = atoi(argv[5]);
    const int N_STEPS = P_MAX_LEN / P_STEP;
    printf("Number of steps: %d\n", N_STEPS);
    
    // initialize other values
    srand(time(0));
    FILE *csv_file = fopen("times.csv", "w");
    fwrite("plen,bmh,bmo,dmb,kmp,kra\n", 1, 25, csv_file);

    // generate tests
    printf("Generating tests...\n");
    char **tests = (char **) malloc(sizeof(char *) * N_TESTS);
    Answer **answers = (Answer **) malloc(sizeof(Answer *) * (N_TESTS));
    for (int i = 0; i < N_TESTS; ++i) {
        tests[i] = randstr(T_LEN, true);
        answers[i] = (Answer *) malloc(sizeof(Answer) * N_STEPS);
        for (int j = 0; j < N_STEPS; ++j) {
            answers[i][j] = randsub(tests[i], T_LEN, (j+1)*P_STEP);
        }
        printf("Generated test %d\n", i);
    }

    // run tests
    printf("Running tests...\n");
    int (*fns[])(char *, size_t, char *, size_t) = {bmh, bmo, dmb, kmp, kra};
    char fn_names[5][4] = {"bmh", "bmo", "dmb", "kmp", "kra"};
    double *times[5];
    for (int i = 0; i < 5; ++i) {
        times[i] = (double *) malloc(sizeof(double) * N_STEPS);
        for (int j = 0; j < N_STEPS; ++j) {
            int plen = (j+1)*P_STEP;
            clock_t start = clock();

            for (int k = 0; k < P_STEP_LEN; ++k) {
                for (int l = 0; l < N_TESTS; ++l) {
                    int _ = fns[i](tests[l], T_LEN, answers[l][j].pattern, plen);
                }
            }

            clock_t diff = clock() - start;
            times[i][j] = (double) diff / CLOCKS_PER_SEC / P_STEP_LEN;
            printf("%s at %d: %.3f\n", fn_names[i], plen, times[i][j]);
        }
    }

    // write times to file
    printf("Writing times to file...\n");
    for (int j = 0; j < N_STEPS; ++j) {
        int plen = (j+1)*P_STEP;
        char line[50];
        int line_len = sprintf(line, "%d,%.3f,%.3f,%.3f,%.3f,%.3f\n", plen,
            times[0][j], times[1][j], times[2][j], times[3][j], times[4][j]
        );
        fwrite(line, 1, line_len, csv_file);
    }

    // free all memory
    printf("Freeing memory...\n");
    for (int i = 0; i < N_TESTS; ++i) {
        free(tests[i]);
        for (int j = 0; j < N_STEPS; ++j) {
            free(answers[i][j].pattern);
        }
        free(answers[i]);
    }
    for (int i = 0; i < 5; ++i) {
        free(times[i]);
    }
    

    free(tests);
    free(answers);
    fclose(csv_file);
    chmod("tests.sh", S_IRUSR | S_IWUSR | S_IXUSR);
}