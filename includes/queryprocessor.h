
#include "movieIndex.h"
#include "docidmap.h"

typedef struct searchResult {
	uint64_t docId; 
  	int rowId;
} *SearchResult;


typedef struct searchResultIter {
	int curDocId; 
	HTIter docIter; 
	LLIter *offsetIter; // TODO: Fix the LLIter def
  int numResults;
} *SearchResultIter; 

/**
 * Opens the file specified by the SearchResult as named 
 *  in the DocIdMap and writes the specified row to the dest. 
 *
 * INPUT: 
 *	result: A SearchResult that contains a docId and rowId
 * 	docIds: The DocIdMap that contains the doc names specified in result. 
 * 	dest: A pointer to a char array where the row from the given file and row should be written. 
 *		String returned in dest is null-terminated. 
 *
 * RETURNS: 
 * 	0 if successful; Not 0 if failed. 
 */
int GetRowFromFile(SearchResult result, DocIdMap docIds, char *dest);

/**
 * Destroys the SearchResultIter. It must not be used after calling this. 
 *
 * INPUT: 
 * 	iter: the SearchResultIter to be destroyed. 
 */
void DestroySearchResultIter(SearchResultIter iter);

/**
 *	Given a SearchResultIter, puts the next SearchResult in the provided 
 * 	SearchResult. 
 *
 * INPUT: 
 * 	iter: The SearchResultIter being iterated through. 
 *	output: A pointer to the SearchResult for output. 
 *
 * RETURNS: 
 * 	0 if successful; !0 else. 
 */
int GetNextSearchResult(SearchResultIter iter, SearchResult output);

/**
 *	Determines if there are more results in this SearchResultIter. 
 *
 *  INPUT: 
 * 	iter: the iterator to determine if there are more items. 
 *
 *  RETURNS: 
 * 	0 if has no more items; !0 otherwise. 
 */
int SearchResultIterHasMore(SearchResultIter iter);

/**
 * Returns the number of elements in the provided SearchResultIter. 
 * This is the number of search results. 
 *
 */
int NumResultsInIter(SearchResultIter iter); 


/**
 * Given an index and a search term, return a SearchResultIter 
 * with relevant results. 
 *
 * INPUT: 
 *	index: The index to use
 * 	term: the phrase to look up. 
 */
SearchResultIter FindMovies(Index index, char *term);
