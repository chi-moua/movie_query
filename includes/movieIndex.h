#ifndef MOVIEINDEX_H
#define MOVIEINDEX_H

#include "Hashtable.h"
#include "LinkedList.h"
#include "movie.h"
#include "docset.h"

/**
 *  An index is just a hashtable from a term to a set of documents. 
 */
typedef Hashtable Index; 


/**
 *  Indexes a given movie. 
 *  Does not use to movie struct for anything other than 
 *   extracting out the information; the movie is safe to 
 *   free after calling this method. 
 *
 * INPUT: 
 *  index: the index to add the movie to. 
 *  movie: a Movie to be added to the index. 
 *  docId: the ID of the file that contains the movie info. 
 *  rowId: the row of the file that contains the movie 
 *
 */
int AddMovieToIndex(Index index, Movie *movie, uint64_t docId, int rowId); 

/**
 * Gets a MovieSet for a given word from the supplied index. 
 *
 * INPUT: 
 *  index: the index to search
 *  term: the term to search for in the index. 
 * 
 * RETURN: 
 *  A MovieSet. Returns NULL if the term is not in the index. 
 *    (that is, there are no movies with the given word in the title)
 *
 */
MovieSet GetMovieSet(Index index, const char *term);

/**
 * 
 *  Destroys the supplied index, freeing up all 
 *   resources used by this index. 
 *
 */
int DestroyIndex(Index index);

/**
 * Creates a new Index. Allocates all the memory 
 *  necessary for this index. 
 *
 */
Index CreateIndex();

#endif
