#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *array;

// Hash function to test
unsigned hash(unsigned char *c) {
    unsigned hash;
    for(hash = 1; *c != '\n'; c++) {
        hash += *c; hash += hash << 7; 
    }
    return hash;
}

int getCollisions(char *fPath) {
    memset(array, 0, 4294967296*sizeof(char)); //clear
    FILE* file = fopen(fPath, "r");
    
    // Read lines, calc hashes, find collisions
    int collisions = 0;
    double n = 0;
    char line[999]; // current line
    while(fgets(line, sizeof(line), file)) { n++;
        unsigned hval = hash(line);
        if(array[hval] == 1) collisions++;
        array[hval] = 1;
    }
    fclose(file);

    // Calc standard deviation, z-score, etc.
    double mean = (n*n - n) / 8589934592;
    double sd = sqrt(mean);
    double score = (collisions - mean) / sd;
    
    // Print result of collision test
    printf("%5.0f, %5d, ", mean, collisions);
    double scoreAbs = fabs(score);
         if(scoreAbs >= 4.5) printf("\x1b[30m\x1b[41m");
    else if(scoreAbs >= 3.7) printf("\x1b[31m");
    else if(scoreAbs >= 2.9) printf("\x1b[33m");
    else if(scoreAbs >= 2.6) printf("\x1b[93m");
    else if(scoreAbs >= 2.1) printf("\x1b[36m");
    else printf("\x1b[32m");
    char scoreStr[12]; sprintf(scoreStr, "%.12f", score);
    printf("%.12s\x1b[0m %s (%.0f) \n", scoreStr, &fPath[5], n);
}

int main(int argc, char *argv[]) {
    array = (char *)malloc(4294967296*sizeof(char));
    
    for(int i = 0; i < 1; i++) {
        getCollisions("data/words_466k.txt");
        getCollisions("data/words_616k.txt");
        getCollisions("data/words_1517k.txt");
        getCollisions("data/words_5446k.txt");
        getCollisions("data/usernames_624k.txt");
        getCollisions("data/usernames_8295k.txt");
        getCollisions("data/passwords_1000k.txt");
        getCollisions("data/passwords_1470k.txt");
        getCollisions("data/passwords_1600k.txt");
        getCollisions("data/passwords_3989k.txt");
        getCollisions("data/passwords_5189k.txt");
        getCollisions("data/domains_1000k.txt");
    }
    return 0;
}
