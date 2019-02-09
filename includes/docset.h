#ifndef DOCSET_H
#define DOCSET_H

#include "Hashtable.h"

typedef struct movieSet {
  char *desc; 
  Hashtable docIndex;
  int numMovies; 
} *MovieSet;


int NumMoviesInSet(MovieSet set); 

int AddMovieToSet(MovieSet set, uint64_t docId, int rowId); 

void DestroyMovieSet(void *val); 

void PrintOffsetList(LinkedList *list);

int MovieSetContainsDoc(MovieSet set, uint64_t docId);

MovieSet CreateMovieSet(char *desc);

void DestroyOffsetList(void *val);

void DestroyMovieSet(void *val);

#endif
