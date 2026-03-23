#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>


typedef struct RandSStr {
    char *str;
    int idx;
} RandSStr;


char tobase64(int num) {
    if (num < 26) return 'A' + num;
    else if (num < 52) return 'a' + num - 26;
    else if (num < 62) return '0' + num - 52;
    else if (num == 62) return '+';
    else return '/';
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


int strfind(char *text, size_t tlen, char *pattern, size_t plen) {
    // Boyer–Moore–Horspool string search algorithm
    int bchar[127];
    for (int i = 0; i < 127; ++i)
        bchar[i] = plen;
    for (int i = 0; i < (int) plen-1; ++i)
        bchar[(int) pattern[i]] = plen-i-1;
    for (int i = 0; i < (int) (tlen-plen+1); ++i) {
        int jump = 0;
        for (int j = plen-1; j >= 0; --j) {
            char srcc = text[i+j];
            if (srcc != pattern[j]) {
                jump = bchar[(int) srcc];
                break;
            }
        }
        if (!jump)
            return i;
        i += (jump-1);
    }
    return -1;
}


RandSStr randsub(char *text, int flen, int plen) {
    // 10% chance to create fake, m-length substring
    // 90% chance to take random, m-length substring from file

    bool do_fake = rand() % 10 == 1;
    if (do_fake) {
        // randomly generate substring
        // ensure it doesn't exist in text
        char *pattern = randstr(plen, false);
        while (strfind(text, flen, pattern, plen) != -1) {
            free(pattern);
            pattern = randstr(plen, false);
        }

        RandSStr rss = { pattern, -1 };
        return rss;

    } else {
        // randomly generate idx
        // extract substring
        int idx = rand() % (flen - plen);
        char *substr = (char *) malloc(plen+1);
        substr[plen] = '\0';
        memmove(substr, text + idx, plen);

        RandSStr rss = { substr, idx };
        return rss;
    }

    RandSStr rss = { NULL, -1 };
    return rss;
}


int main(int argc, char **argv) {
    // create t, n-length files
    // 10% chance to create fake, m-length substring
    // 90% chance to take random, m-length substring from file
    // write .sh file to output time to complete all test cases
    // output solutions to stdout

    // verify and parse input
    if (argc != 5) {
        printf("usage: ./tests <number of tests> <file length> <pattern length> <alphabet len>\n");
        return 1;
    }
    int nfiles = atoi(argv[1]);
    int nfiles_slen = strlen(argv[1]);
    int flen = atoi(argv[2]);
    int plen = atoi(argv[3]);
    int alen = atoi(argv[4]);
    if (alen < 2 || alen < 64) {
        printf("Error: alphabet length must be in range [2, 64].\n");
        return 1;
    }
    
    // initialize other values
    srand(time(0));
    FILE *shell_script = fopen("tests.sh", "w");
    fwrite("rm -f actual.txt\n", 1, 17, shell_script);
    
    // start writing files
    for (int i = 0; i < nfiles; ++i) {
        // generate data
        char *text = randstr(flen, true);
        RandSStr answer = randsub(text, flen, plen);
        
        // write rand file
        char *rfile_name = (char *) malloc(14 + nfiles_slen);
        sprintf(rfile_name, "./tests/%0*d.txt", nfiles_slen, i);
        FILE *rfile = fopen(rfile_name, "w");
        fwrite(text, 1, flen, rfile);
        fclose(rfile);
        
        // write to shell script
        char *cmd = (char *) malloc(36 + plen + nfiles_slen);
        sprintf(cmd, "$1 \'%s\' %s >> actual.txt\n", answer.str, rfile_name);
        fwrite(cmd, 1, (int) strlen(cmd), shell_script);
        
        // print data for file
        printf("%d\n", answer.idx);
        
        // free string data
        free(text);
        free(rfile_name);
        free(cmd);
        free(answer.str);
    }
    
    fclose(shell_script);
    chmod("tests.sh", S_IRUSR | S_IWUSR | S_IXUSR);
}