#ifndef DOCIDMAP_H
#define DOCIDMAP_H

#include "Hashtable.h"

//===========================
//
// A docid map connects unique IDs to files. 
//
//===========================

// A wrapper to iterate through docIds. 
typedef HTIter DocIdIter;

// A docId is a Hashtable, where
// the key is an int and the value
// is a filename. 
typedef Hashtable DocIdMap;


void DestroyString(void *val);


/**
 *  Creates and returns a pointer to a DocIdMap. 
 *
 * 
 *
 */
DocIdMap CreateDocIdMap(); 


/**
 * Wrapper to destroy DocIdMap. 
 * 
 * Destroys and frees all data in the docidmap. 
 */
void DestroyDocIdMap(DocIdMap map); 

/**
 * Given a map and a pointer to a filename, puts the 
 * filename in the map and gives it a unique ID. 
 *
 * Assumes that the filename has been malloc'd 
 * prior to being added to the map. 
 *
 */
void PutFileInMap(char *filename, DocIdMap map);

// Creates an iterator to go through all of the
// document IDs in the DocIdMap. 
DocIdIter CreateDocIdIterator(DocIdMap map); 

// Destroy the DocIdIterator. 
void DestroyDocIdIterator(DocIdIter iter); 

// Given a map and a docId, returns the relevant
// filename. 
char *GetFileFromId(DocIdMap docs, int docId);


#endif
