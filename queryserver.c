#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "queryprotocol.h"
#include "docset.h"
#include "movieIndex.h"
#include "docidmap.h"
#include "includes/Hashtable.h"
#include "queryprocessor.h"
#include "fileparser.h"
#include "filecrawler.h"

DocIdMap docs;
Index docIndex;

const int SEARCH_RESULT_LENGTH = 1500;
char movieSearchResult[1500];
char buffer[1500];

int Cleanup();

void Setup(char *dir);

void sigint_handler(int sig) {
    write(0, "Exit signal sent. Cleaning up...\n", 34);
    Cleanup();
    exit(0);
  }

void Setup(char *dir) {
    printf("Crawling directory tree starting at: %s\n", dir);
    // Create a DocIdMap
    docs = CreateDocIdMap();
    CrawlFilesToMap(dir, docs);

    printf("Crawled %d files.\n", NumElemsInHashtable(docs));

    // Create the index
    docIndex = CreateIndex();

    // Index the files
    printf("Parsing and indexing files...\n");
    ParseTheFiles_MT(docs, docIndex);
    printf("%d entries in the index.\n", NumElemsInHashtable(docIndex));
}

int Cleanup() {
    DestroyIndex(docIndex);
    DestroyDocIdMap(docs);

    return 0;
}

// Gets connected to the port, returns the sockid
int GetConnected(char* port) {
    printf("Waiting for connection\n");

    int status;
    struct addrinfo inputs;
    struct addrinfo *results;

    memset(&inputs, 0, sizeof inputs);
    inputs.ai_family = AF_UNSPEC;
    inputs.ai_socktype = SOCK_STREAM;
    inputs.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, port, &inputs, &results)) != 0) {
        perror("Getaddrinfo error\n");
        exit(1);
    }
    int sockid;
    sockid = socket(results->ai_family, results->ai_socktype,
      results->ai_protocol);
    if (sockid == -1) {
        perror("Socket error\n");
        exit(1);
    }
    int y = bind(sockid, results->ai_addr, results->ai_addrlen);
    if (y == -1) {
        perror("Binding error\n");
        exit(1);
    }
    y = listen(sockid, 10);
    if (y == -1) {
        perror("Listening error\n");
        exit(1);
    }
    return sockid;
}

// Search for the results and communicates with the client
int SearchForResults(int new_id, char* query) {
    int y;
    SearchResultIter iter = FindMovies(docIndex, query);
    int num_results;
    if (iter == NULL) {
        num_results = 0;
        y = send(new_id, &num_results, 4, 0);
        if (y == -1) {
            perror("Error sending zero results\n");
            Cleanup();
            exit(1);
        }
        return -1;
    } else {
        num_results = NumResultsInIter(iter);
    }

    y = send(new_id, &num_results, 4, 0);
    if (y == -1) {
        perror("Error sending number of results\n");
        Cleanup();
        exit(1);
    }
    SearchResult search_result = malloc(sizeof(SearchResult));
    printf("Getting results for term: %s\n", query);
    for (int i = 0; i < num_results; i++) {
        y = recv(new_id, buffer, SEARCH_RESULT_LENGTH, 0);
        if (y == -1) {
            perror("Receive ack error\n");
            Cleanup();
            exit(1);
        }
        buffer[y] = '\0';
        y = CheckAck(buffer);
        if (y == -1) {
            perror("Check ack error\n");
            Cleanup();
            exit(1);
        }
        GetNextSearchResult(iter, search_result);
        GetRowFromFile(search_result, docs, movieSearchResult);
        y = send(new_id, movieSearchResult, strlen(movieSearchResult), 0);
    }
    if (SendGoodbye(new_id) == -1) {
      perror("Send goodbye error\n");
      Cleanup();
      exit(1);
    }
    free(search_result);
    DestroySearchResultIter(iter);
    return 0;
}
// Runs the query
int RunQuery(int sockid) {
    int y;
    int new_id;
    char query[1500];

    while (1) {
        new_id = accept(sockid, NULL, NULL);
        if (new_id == -1) {
            perror("Accepting connection error\n");
            exit(1);
        }
        printf("Client connected\n");

        y = SendAck(new_id);
        if (y == -1) {
            perror("Send initial ack error\n");
            Cleanup();
            exit(1);
        }
        y = recv(new_id, query, SEARCH_RESULT_LENGTH, 0);
        if (y == -1) {
            perror("Receive query error\n");
            Cleanup();
            exit(1);
        }
        query[y] = '\0';
        if (CheckKill(query) == 0) {
            printf("Kill server message received\n");
            Cleanup();
            close(new_id);
            close(sockid);
            exit(0);
        }
        if (SearchForResults(new_id, query) == -1) {
            printf("Did not find results.\n");
        }
        printf("Waiting for another word from client\n");
    }
    close(new_id);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("There should be three arguments as follow:\n");
        printf("./queryerver [datadir] [port]\n");
        exit(1);
    }

    Setup(argv[1]);
    int sockid = GetConnected(argv[2]);
    RunQuery(sockid);

    return 0;
}
