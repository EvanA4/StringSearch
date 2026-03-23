#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>


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


char *randstr(int len, int alen) {
    char *text = (char *) malloc(sizeof(char) * len + 1);
    for (int i = 0; i < len; ++i) {
        text[i] = tobase64(rand() % alen);
    } text[len] = '\0';
    return text;
}


int strfind(char *text, size_t tlen, char *pattern, size_t plen) {
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


RandSStr randsub(char *text, int flen, int plen, int alen) {
    // 10% chance to attempt fake, m-length substring
    // 90% chance to take random, m-length substring from file
    RandSStr rss;

    bool do_fake = rand() % 10 == 1;
    if (do_fake) {
        // randomly generate substring
        char *pattern = randstr(plen, alen);
        rss.str = pattern;
        rss.idx = strfind(text, flen, pattern, plen);

    } else {
        // randomly generate idx
        // extract substring
        int idx = rand() % (flen - plen);
        char *pattern = (char *) malloc(plen+1);
        pattern[plen] = '\0';
        memmove(pattern, text + idx, plen);

        rss.str = pattern;
        rss.idx = strfind(text, flen, pattern, plen);
    }

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
    if (alen < 2 || alen > 64) {
        printf("Error: alphabet length must be in range [2, 64].\n");
        return 1;
    }

    // remove all files in tests dir
    printf("Emptying test directory...\n");
    system("rm -rf tests/*");
    sleep(1);
    
    // initialize other values
    srand(time(0));
    FILE *shell_script = fopen("tests.sh", "w");
    FILE *expected_file = fopen("expected.txt", "w");
    fwrite("rm -f actual.txt\n", 1, 17, shell_script);
    
    // start writing files
    printf("Generating tests...\n");
    for (int i = 0; i < nfiles; ++i) {
        // generate data
        char *text = randstr(flen, alen);
        RandSStr answer = randsub(text, flen, plen, alen);
        
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
        fprintf(expected_file, "%d\n", answer.idx);
        printf("\"%*s\" in %*s at index %d\n", plen, answer.str, nfiles_slen+12, rfile_name, answer.idx);
        
        // free string data
        free(text);
        free(rfile_name);
        free(cmd);
        free(answer.str);
    }
    
    fclose(shell_script);
    chmod("tests.sh", S_IRUSR | S_IWUSR | S_IXUSR);
}