#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "queryprotocol.h"
#include "docset.h"
#include "movieIndex.h"
#include "docidmap.h"
#include "includes/Hashtable.h"
#include "queryprocessor.h"
#include "fileparser.h"
#include "filecrawler.h"

const int SEARCH_RESULT_LENGTH = 1500;

int Cleanup();

DocIdMap docs;
Index docIndex;
char movieSearchResult[1500];

void sigchld_handler(int s) {
    write(0, "Handling zombies...\n", 20);
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}


void sigint_handler(int sig) {
    write(0, "Ahhh! SIGINT!\n", 14);
    Cleanup();
    exit(0);
}


void Setup(char *dir) {
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;  // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    struct sigaction kill;

    kill.sa_handler = sigint_handler;
    kill.sa_flags = 0;  // or SA_RESTART
    sigemptyset(&kill.sa_mask);

    if (sigaction(SIGINT, &kill, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Crawling directory tree starting at: %s\n", dir);
    docs = CreateDocIdMap();
    CrawlFilesToMap(dir, docs);

    printf("Crawled %d files.\n", NumElemsInHashtable(docs));

    docIndex = CreateIndex();

    printf("Parsing and indexing files...\n");
    ParseTheFiles(docs, docIndex);
    printf("%d entries in the index.\n", NumElemsInHashtable(docIndex));
}

int Cleanup() {
    DestroyIndex(docIndex);
    DestroyDocIdMap(docs);
    return 0;
}

// Search for the results and speaks to client
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

    if (num_results <= 0) {
        return -1;
    } else {
        printf("Getting results for term: %s\n", query);
        SearchResult search_result = malloc(sizeof(SearchResult));
        for (int i = 0; i < num_results; i++) {
            char buffer[1000];
            y = recv(new_id, buffer, SEARCH_RESULT_LENGTH - 1, 0);
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
    }
    return 0;
}

// Handles one connection
int HandleConnection(int new_id) {
    printf("Handling connecting and will find results\n");
    int y;
    char query[1500];
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
        return -1;
    }
    if (SearchForResults(new_id, query) == -1) {
        return -1;
    }
    printf("Waiting for another word from client\n");
    close(new_id);
    return 0;
}


// Handles the connections; will start the fork process.
int HandleConnections(int sock_id) {
    while (1) {
        int new_id = accept(sock_id, NULL, NULL);
        if (new_id == -1) {
            perror("Accepting connection error\n");
            exit(1);
        }
        printf("Client connected\n");

        int y = SendAck(new_id);
        if (y == -1) {
            perror("Send initial ack error\n");
            Cleanup();
            exit(1);
        }

        pid_t childPid = fork();
        if (childPid < 0) {
            perror("fork() error\n");
            Cleanup();
            exit(-1);
        }
        if (childPid == 0) {
            close(sock_id);
            HandleConnection(new_id);
            close(new_id);
            exit(0);
        }
        close(new_id);
        // Cleanup();
    }
    return 0;
}

// Will begin the listening process and return the "listening" sockid.
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
    freeaddrinfo(results);
    return sockid;
}

int main(int argc, char **argv) {
    Setup(argv[1]);
    int sockid = GetConnected(argv[2]);
    HandleConnections(sockid);
    return 0;
}
