
#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "movieIndex.h"
#include "docidmap.h"


int ParseTheFiles(DocIdMap docs, Index index);
int ParseTheFiles_MT(DocIdMap docs, Index index); 


#endif
