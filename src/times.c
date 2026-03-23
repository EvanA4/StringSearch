#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <math.h>


#define ssfn int (*)(char *, size_t, char *, size_t)
typedef struct Answer {
    char *pattern;
    int idx;
} Answer;
typedef struct Trial {
    char *text;
    char *pattern;
    int plen;
    int answer;
} Trial;
typedef struct RandSStr {
    char *str;
    int idx;
} RandSStr;


int bmh(char *text, size_t tlen, char *pattern, size_t plen) {
    // Boyer–Moore–Horspool string search algorithm
    int bchar[256];
    for (int i = 0; i < 256; ++i) bchar[i] = -1;
    for (int i = 0; i < (int) plen-1; ++i) bchar[(int) pattern[i]] = i;
    int s = 0;
    while (s <= (int) (tlen-plen)) {
        // start matching from back
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char textc = text[s+j];
            if (textc != pattern[j]) {
                jump = j - bchar[(int) textc] > 1 ? j - bchar[(int) textc] > 1 : 1;
                break;
            }
        }
        // if !jump, match found
        if (!jump) return s;
        s += jump;
    }
    return -1;
}


int bmo(char *text, size_t tlen, char *pattern, size_t plen) {
    // Boyer–Moore string search algorithm
    // bad character rule
    int bchar[256];
    for (int i = 0; i < 127; ++i) bchar[i] = -1;
    for (int i = 0; i < (int) plen-1; ++i) bchar[(int) pattern[i]] = i;
    // good suffix rule
    int borders[plen+1];
    int gsuff[plen];
    for (int i = 0; i < (int) plen; ++i) gsuff[i] = 0;
    int i = plen;
    int j = plen + 1;
    borders[i] = j;
    while (i > 0) {
        while (j <= (int) plen && pattern[i-1] != pattern[j-1]) {
            if ((j-1) >= 0 && gsuff[j-1] == 0) gsuff[j-1] = j-i;
            j = borders[j];
        }
        --i;
        --j;
        borders[i] = j;
    }
    int b = borders[0];
    for (i = 0; i < (int) plen; ++i) {
        if (i == b) b = borders[b];
        if (gsuff[i] == 0) gsuff[i] = b;
    }
    // actual string search
    int s = 0;
    while (s <= (int) (tlen-plen)) {
        // start matching from back
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char textc = text[s+j];
            if (textc != pattern[j]) {
                int bchar_jump = j - bchar[(int) textc] > 1 ? j - bchar[(int) textc] > 1 : 1;
                jump = bchar_jump > gsuff[j] ? bchar_jump : gsuff[j];
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


int dmb(char *text, size_t tlen, char *pattern, size_t plen) {
    // Brute force string search algorithm
    for (int i = 0; i < (int) (tlen-plen+1); ++i) {
        int match = 1;
        for (int j = 0; j < (int) plen; ++j) {
            if (text[i+j] != pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;
}


int kmp(char *text, size_t tlen, char *pattern, size_t plen) {
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
    // actual search algorithm
    int i = 0;
    int q = 0;
    while (i < (int) tlen) {
        if (text[i] == pattern[q]) {
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


int kra(char *text, size_t tlen, char *pattern, size_t plen) {
    // Karp–Rabin string search algorithm
    // Precompute the powers of the base modulo the mod
    const int base = 256;
    const int mod = 1000000007;
    const int window_size = plen;
    const int n = tlen;
    long long power = 1;
    for (int i = 1; i < window_size; i++) {
        power = (power * base) % mod;
    }
    // Compute hash of pattern and first window
    long long pattern_hash = 0;
    long long current_hash = 0;
    for (int i = 0; i < window_size; i++) {
        pattern_hash = (base * pattern_hash + pattern[i]) % mod;
        current_hash = (current_hash * base + text[i]) % mod;
    }
    // Compute the hash values of the rest of the substrings
    for (int i = 0; i <= n - window_size; i++) {
        // If hashes match, do a full string comparison
        if (current_hash == pattern_hash) {
            int j = window_size - 1;
            while (j >= 0 && pattern[j] == text[i + j]) --j;
            if (j < 0) {
                return i;
            }
        }
        // Compute next hash
        if (i < n - window_size) {
            current_hash = (base * (current_hash - text[i] * power)
                           + text[i+window_size]) % mod;
            if (current_hash < 0) current_hash += mod;
        }
    }
    return -1;
}


int (*const SEARCH_FNS[])(char *, size_t, char *, size_t) = {bmh, bmo, dmb, kmp, kra};
const char *FN_NAMES[] = {"bmh", "bmo", "dmb", "kmp", "kra"};
const int N_FNS = 5;
const int N_TRIALS = 100;
const int N_ITR = 5;
const int TEXT_LEN = 300000;
const int DEFAULT_ALEN = 26;
const int DEFAULT_PLEN = 16;
const int ALPHABET_LEN_BOUNDS[] = {4, 64};
const int PATTERN_LEN_BOUNDS[] = {4, 100};


char tobase64(int num) {
    if (num < 26) return 'A' + num;
    else if (num < 52) return 'a' + num - 26;
    else if (num < 62) return '0' + num - 52;
    else if (num == 62) return '+';
    else return '/';
}


char *randstr(int len, int alen) {
    char *text = (char *) malloc(sizeof(char) * len + 1);
    for (int i = 0; i < len; ++i) {
        text[i] = tobase64(rand() % alen);
    } text[len] = '\0';
    return text;
}


RandSStr randsub(char *text, int flen, int plen, int alen) {
    // 10% chance to attempt fake, m-length substring
    // 90% chance to take random, m-length substring from file
    RandSStr rss;

    bool do_fake = rand() % 10 == 1;
    if (do_fake) {
        // randomly generate substring
        char *pattern = randstr(plen, alen);
        rss.str = pattern;
        rss.idx = dmb(text, flen, pattern, plen);

    } else {
        // randomly generate idx
        // extract substring
        int idx = rand() % (flen - plen);
        char *pattern = (char *) malloc(plen+1);
        pattern[plen] = '\0';
        memmove(pattern, text + idx, plen);

        rss.str = pattern;
        rss.idx = dmb(text, flen, pattern, plen);
    }

    return rss;
}



double avg_time(Trial *trials, int (*fn)(char *, size_t, char *, size_t), const char *fn_name) {
    clock_t start = clock();

    for (int i = 0; i < N_ITR; ++i) {
        for (int j = 0; j < N_TRIALS; ++j) {
            int actual = fn(trials[j].text, TEXT_LEN, trials[j].pattern, trials[j].plen);
            if (actual != trials[j].answer) {
                FILE *err_file = fopen("trial.txt", "w");
                fwrite(trials[j].text, sizeof(char), TEXT_LEN, err_file);
                fclose(err_file);
                fprintf(
                    stderr, (
                        "Error: %s returned %d instead of %d for pattern \"%s\"."
                        " Text written to in trial.txt.\n"
                    ),
                    fn_name, actual, trials[j].answer, trials[j].pattern
                );
                return nan("");
            }
        }
    }

    clock_t diff = clock() - start;
    return (double) diff / CLOCKS_PER_SEC / N_ITR;
}


Trial *gen_trials(int plen, int alen) {
    Trial *trials = (Trial *) malloc(N_TRIALS * sizeof(Trial));
    for (int i = 0; i < N_TRIALS; ++i) {
        trials[i].text = randstr(TEXT_LEN, alen);
        RandSStr rss = randsub(trials[i].text, TEXT_LEN, plen, alen);
        trials[i].plen = plen;
        trials[i].answer = rss.idx;
        trials[i].pattern = rss.str;
    }
    return trials;
}


int run_trials(Trial *trials, FILE *csv_file) {
    for (int i = 0; i < N_FNS; ++i) {
        double time_res = avg_time(trials, SEARCH_FNS[i], FN_NAMES[i]);
        if (isnan(time_res)) {
            for (int i = 0; i < N_TRIALS; ++i) {
                free(trials[i].pattern);
                free(trials[i].text);
            }
            free(trials);
            return -1;
        }
        fprintf(csv_file, ",%f", time_res);
    }
    fprintf(csv_file, "\n");
    return 0;
}


int test_alen(FILE *csv_file) {
    for (
        int alen = ALPHABET_LEN_BOUNDS[0]; 
        alen <= ALPHABET_LEN_BOUNDS[1]; 
        alen += ALPHABET_LEN_BOUNDS[0]
    ) {
        printf("alen = %d\n", alen);
        printf("\tGenerating trials...\n");
        Trial *trials = gen_trials(DEFAULT_PLEN, alen);

        // get and write time for each function
        printf("\tRunning trials...\n");
        fprintf(csv_file, "%d,%d", DEFAULT_PLEN, alen);
        if (run_trials(trials, csv_file) < 0) {
            return -1;
        }

        // free trials data
        for (int i = 0; i < N_TRIALS; ++i) {
            free(trials[i].pattern);
            free(trials[i].text);
        }
        free(trials);
    }

    return 0;
}


int test_plen(FILE *csv_file) {
    for (
        int plen = PATTERN_LEN_BOUNDS[0]; 
        plen <= PATTERN_LEN_BOUNDS[1]; 
        plen += PATTERN_LEN_BOUNDS[0]
    ) {
        printf("plen = %d\n", plen);
        printf("\tGenerating trials...\n");
        Trial *trials = gen_trials(plen, DEFAULT_ALEN);

        // get and write time for each function
        printf("\tRunning trials...\n");
        fprintf(csv_file, "%d,%d", plen, DEFAULT_ALEN);
        if (run_trials(trials, csv_file) < 0) {
            return -1;
        }

        // free trials data
        for (int i = 0; i < N_TRIALS; ++i) {
            free(trials[i].pattern);
            free(trials[i].text);
        }
        free(trials);
    }

    return 0;
}


int main() {
    // initialize other values
    srand(time(0));
    FILE *csv_file = fopen("times.csv", "w");
    fwrite("plen,alen,bmh,bmo,dmb,kmp,kra\n", 1, 30, csv_file);

    // start testing
    if (test_alen(csv_file) < 0) {
        fclose(csv_file);
        return -1;
    }
    if (test_plen(csv_file) < 0) {
        fclose(csv_file);
        return -1;
    }
    
    fclose(csv_file);
}