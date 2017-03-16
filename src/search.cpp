#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include "search.h"

using namespace std;


int levenshteinDistance(char *s, const char *t){
    int costOfInsert = 1 , costOfDelete = 1, costOfReplace;
    int s_len = strlen(s), t_len = strlen(t);
    if (strcmp(s, t) == 0) return 0;
    if (t_len == 0)    return s_len;
    if (s_len == 0)    return t_len;

    int *v0 = (int *) calloc(t_len+1, sizeof(int));
    int *v1 = (int *) calloc(t_len+1, sizeof(int));

    for (int i=0; i < t_len+1; i++)
        v0[i] = i;
    for (int i=0; i < s_len; i++){
        v1[0] = i + 1;
        for (int j=0; j< t_len; j++){
            costOfReplace = ((s[i] == t[j]) ? 0 : 1 );
            v1[j+1] = MIN_3(v1[j]+costOfInsert,v0[j+1]+costOfDelete,v0[j]+costOfReplace);
        }
        for (int j=0; j < t_len+1; j++)
            v0[j] = v1[j];
    }
    return v1[t_len];
}

char* fuzzySearch(char *haystack, const char *pattern, unsigned int distanceTolerance) {
    unsigned int tokenLen, patternLen = strlen(pattern);
    char *tofree = strdup(haystack);
    char *tmp = tofree, *token;
    strcpy(tmp, haystack);
    unsigned int lenOfOffset = 0;
    const int LEN_OF_DELIM = 1;
    char  delim[] = " .,()?!:";
    bool foundFlag = false;
    
    
    for(token=strsep(&tmp, delim); token!=NULL; token=strsep(&tmp, delim)){
      int tokenLen = strlen(token);
      //cout << "token len:" << tokenLen << " token:" << token << endl;
      if((tokenLen>(patternLen + distanceTolerance)) || (tokenLen < (patternLen - distanceTolerance))){
            lenOfOffset += (tokenLen + LEN_OF_DELIM);
            continue;
        } else if(levenshteinDistance(token, pattern) <= distanceTolerance){
            foundFlag = true;
            break;
        }
        lenOfOffset += (tokenLen + LEN_OF_DELIM);
    }
    
    free(tofree);
    if (foundFlag)
      return (haystack + lenOfOffset);
    else
      return NULL;
}

