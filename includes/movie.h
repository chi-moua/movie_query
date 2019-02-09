
#ifndef MOVIE_H
#define MOVIE_H

typedef struct movie {
	char *id; 
	char *type; 
	char *title; 
	int isAdult; 
	int year; 
	int runtime; 
	char *genres; // TODO: ????

} Movie; 


Movie* CreateMovie(); 

void DestroyMovie(Movie* movie);

Movie* CreateMovieFromRow(char *dataRow); 

#endif // MOVIE