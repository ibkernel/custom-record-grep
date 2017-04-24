#ifndef CRGREP_SEARCH_H_
#define CRGREP_SEARCH_H_

#define MIN_3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

unsigned int levenshteinDistance(char *s, const char *t);
char* toleranceSearch(char *haystack, const char *pattern, unsigned int distanceTolerance);


#endif