#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// Truly excessive 64-bit seed generation.
uint64_t excess64() {
    struct timespec tv; clock_gettime(CLOCK_REALTIME, &tv);
    uint64_t x = tv.tv_nsec|1; x *= getpid();
    x += tv.tv_nsec;
    x ^= x >> 32; x *= 0xbea225f9eb34556d;
    x ^= x >> 29; x *= 0xbea225f9eb34556d;
    x ^= x >> 32; x *= 0xbea225f9eb34556d; x ^= x >> 29;
    x ^= (x >> 25 | x << 39) ^ (x >> 47 | x << 17);
    x *= 0x9e6c63d0676a9a99; x ^= x >> 23 ^ x >> 51;
    x *= 0x9e6d62d06f6a9a9b; x ^= x >> 23 ^ x >> 51;
    return x;
}
// augmented sfc64.
uint64_t _a = 0, _b, _c, _d = 1;
uint64_t sfc64() { // 0xbadbee
    uint64_t x = _a + _b + _d++;
    _a = _b ^ (_b >> 11); _b = _c + (_c << 3);
    _c = (_c << 24 | _c >> 40) + x;
    x ^= x >> 32; x *= 0xbea225f9eb34556d;
    x ^= x >> 29; x *= 0xbea225f9eb34556d;
    x ^= x >> 32; x *= 0xbea225f9eb34556d; x ^= x >> 29;
    x ^= (x >> 25 | x << 39) ^ (x >> 47 | x << 17);
    x *= 0x9e6c63d0676a9a99; x ^= x >> 23 ^ x >> 51;
    x *= 0x9e6d62d06f6a9a9b; x ^= x >> 23 ^ x >> 51;
    return x;
}



char *array;

unsigned hash(unsigned char *s, int seed, int a, int b) {
    unsigned hash;
    for (hash = seed; *s != '\n'; s++) {
        hash += *s; hash += (hash << a) + (hash << b); 
    }
    /* // force avalanche; should not affect collisions at all.
    hash ^= hash >> 17; hash *= 0xed5ad4bb;
    hash ^= hash >> 11; hash *= 0xac4c1b51;
    hash ^= hash >> 15; hash *= 0x31848bab;
    hash ^= hash >> 14;
    */
    return hash;
}

long n_trials = 0;
long total_3o = 0;
long total_4o = 0;
long total_collisions = 0;
double total_mean = 0;
double avg_score = 0;
double pos_score = 0;

double min = 0;
double max = 0;

int kill = 0;

int getCollisions(char *dafile, int seed, int a, int b, int count) {
    memset(array, 0, 4294967296 * sizeof(char));
    FILE* file = fopen(dafile, "r");
    char line[999];
    int collisions = 0;
    double n = 0;
    n_trials++;

    uint32_t hval;
    //for(int j = 0; j < count; j++) { hval = sfc64() >> 32;
    while (fgets(line, sizeof(line), file)) { hval = hash(line, seed, a, b);
        if(array[hval] == 69) collisions++;
        array[hval] = 69;
        n++;
    }
    fclose(file);

    total_collisions += collisions;
    double mean = (n*n - n) / 8589934592;
    total_mean += mean;
    double sd = sqrt(mean);
    double score = (collisions - mean) / sd;

    avg_score += score;
    if(score > 0) pos_score += score;
    if(seed == 0) { min = score; max = score; }
    if(score > max) max = score;
    if(score < min) min = score;
    
    if(score >= 2.9) total_3o++;
    if(score >= 3.9) total_4o++;

    double absScore = fabs(score); // only if >= 1.8
         if(absScore >= 4.4) printf("\x1b[30m\x1b[41m");
    else if(absScore >= 3.7) printf("\x1b[31m");
    else if(absScore >= 2.9) printf("\x1b[33m");
    else if(absScore >= 2.6) printf("\x1b[93m");
    else if(absScore >= 2.1) printf("\x1b[36m");
    else printf("\x1b[32m");
    printf("%.3f", score);
    printf("\x1b[0m ");

    if(score >= 5) { // OLD: 4.5
        printf("\x1b[30m\x1b[41m");
        printf("\n[%2d,%2d] Aborted. SD %.3f exceeds threshold. Seed: %d (Trials: %d)",
        a, b, score, seed,  n_trials);
        printf("\x1b[0m\n");
        exit(0);
    }
    if(total_4o >= 3) { // if we hit three 4o's 
        printf("\x1b[30m\x1b[41m");
        printf("\n[%2d,%2d] Found too many deviations >= 4. Seed: %d (Trials: %d)",
        a, b, seed, n_trials);
        printf("\x1b[0m\n");
        kill = 1; return 1;
    }
}

int main(int argc, char *argv[]) {
    //uint64_t rngseed = excess64(); _a = _b = _c = rngseed;
    //for(int i = 0; i < 12; i++) sfc64();

    int a = strtol(argv[1], NULL, 10);
    int b = strtol(argv[2], NULL, 10);
    array = (char *)malloc(4294967296 * sizeof(char));
    
    printf("Testing [%d,%d]\n", a, b);

    for(int i = 0; i < 99; i++) { // OLD: 40
        getCollisions("data/words_466k.txt", i, a,b, 466550);
        if(kill == 1) break;
        getCollisions("data/usernames_624k.txt", i, a,b, 624370);
        if(kill == 1) break;
        getCollisions("data/words_616k.txt", i, a,b, 616057);
        if(kill == 1) break;
        getCollisions("data/passwords_1000k.txt", i, a,b, 1044698);
        if(kill == 1) break;
        getCollisions("data/domains_1000k.txt", i, a,b,   1000000);
        if(kill == 1) break;
        getCollisions("data/words_1517k.txt", i, a,b, 1516998);
        if(kill == 1) break;
        getCollisions("data/passwords_1470k.txt", i, a,b, 1470579);
        if(kill == 1) break;
        getCollisions("data/passwords_1600k.txt", i, a,b, 1652877);
        if(kill == 1) break;
        //getCollisions("data/passwords_3989k.txt", i, a,b, 3989290);
        //getCollisions("data/passwords_5189k.txt", i, a,b, 5189402);
        //getCollisions("data/words_5446k.txt", i, a,b, 5446758);
        //getCollisions("data/usernames_8295k.txt", i, a,b, 8295419);
    }

    double sd2 = sqrt(total_mean);
    double total_zscore = (total_collisions - total_mean) / sd2;
    double zavg = avg_score / n_trials;
    double pavg = pos_score / n_trials;
    printf("\n\x1b[32m[%2d,%2d] Z-All: %.4f, Z-Avg: %.4f, P-Avg: %.4f, X: (%d|%d), max: %.3f\x1b[0m\n",
     a, b, total_zscore, zavg, pavg, total_3o, total_4o, max);
    printf("Expected: %.3f, Actual: %d (Trials: %d)\n", total_mean, total_collisions, n_trials);

    if(total_zscore > 2.5) {
        printf("\x1b[31m");
        printf("Warning: Z-All too damn high"); 
        printf("\x1b[0m\n");
    }
    if(zavg > 0.109) {
        printf("\x1b[31m");
        printf("Warning: Z-Avg too damn high"); 
        printf("\x1b[0m\n");
    }
    if(pavg > 0.48) {
        printf("\x1b[31m");
        printf("Warning: P-Avg too damn high"); 
        printf("\x1b[0m\n");
    }
    if(total_3o > 6) {
        printf("\x1b[31m");
        printf("Warning: Found %d outliers > 3 sd - could be bad.", total_3o); 
        printf("\x1b[0m\n");
    }
    if(total_4o > 1) {
        printf("\x1b[31m");
        printf("Warning: Found %d outliers > 4 sd - could be bad.", total_4o); 
        printf("\x1b[0m\n");
    }
    return 0;
}
