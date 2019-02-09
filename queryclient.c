#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "queryprotocol.h"

int MyCheckGoodbye(char *response) {
  if (strcmp("GOODBYE", response) == 0) {
    return 0;
  } else {
    printf("Not goodbye. Response received: %s", response);
    return -1;
  }
}

int GetConnected(char* ip, char* port) {
    int status;
    struct addrinfo inputs;
    struct addrinfo *results;
    memset(&inputs, 0, sizeof inputs);
    inputs.ai_family = AF_UNSPEC;
    inputs.ai_socktype = SOCK_STREAM;
    inputs.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(ip, port, &inputs, &results)) != 0) {
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
    int con;
    con = connect(sockid, results->ai_addr, results->ai_addrlen);
    if (con == -1) {
        perror("Connect error\n");
        exit(1);
    }
    printf("Connected to movie serve\n");
    freeaddrinfo(results);
    return sockid;
}

int SearchForResults(int sockid) {
    char buffer[1000];
    int searchResults;
    int y = recv(sockid, &searchResults, 4, 0);
    if (y == -1) {
        perror("Error receiving number of search results\n");
        exit(1);
    }
    printf("%d search Results\n", searchResults);
    if (searchResults == 0) {
        return - 1;
    }
    if (SendAck(sockid) == -1) {
        perror("Error sending acknowledgement to server\n");
        exit(1);
    }
    for (int i = 0; i < searchResults; i++) {
        y = recv(sockid, buffer, 1000, 0);
        if (y == -1) {
            perror("Error receiving movie file\n");
            exit(1);
        }
        buffer[y] = '\0';
        printf("%s\n", buffer);
        if (SendAck(sockid) == -1) {
            perror("Error sending acknowledgement to serve\n");
            exit(1);
        }
    }
    y = recv(sockid, buffer, 1000, 0);
    if (y == -1) {
        perror("Error receiving movie file\n");
        exit(1);
    }
    buffer[y] = '\0';
    MyCheckGoodbye(buffer);
    return 0;
}

void RunQuery(char *query, char* ip, char* port) {
    int sockid = GetConnected(ip, port);
    char buffer[1000];
    int x = recv(sockid, &buffer, 1000, 0);
    buffer[x] = '\0';
    if (x == -1) {
        perror("Error receiving initial ack from server\n");
    }
    if (CheckAck(buffer) == -1) {
        perror("Error reading ack\n");
        exit(1);
    }
    int y = send(sockid, query, strlen(query), 0);
    if (y == -1) {
        perror("Error sending message\n");
        exit(1);
    }
    SearchForResults(sockid);
}

void KillServer(char* ip, char* port) {
    int sockid = GetConnected(ip, port);
    if (SendKill(sockid) == -1) {
        perror("Error sending kill\n");
        exit(1);
    }
    close(sockid);
    printf("Goodbye Server. I'm leaving now.\n");
    exit(0);
}

void RunPrompt(char* ip, char* port) {
    char input[1000];
    while (1) {
        printf("Enter a term to search for, q to quit or k to kill: ");
        scanf("%s", input);
        if (strlen(input) == 1) {
            if (input[0] == 'q') {
                exit(0);
            } else {
                if ((input[0] == 'k') || input[0] == 'K') {
                    KillServer(ip, port);
                }
            }
        }
        RunQuery(input, ip, port);
    }
}

int main(int argc, char **argv) {
  if (argc != 3) {
        perror("There are the wrong number of arguments.");
        exit(1);
    }
    RunPrompt(argv[1], argv[2]);
    return 0;
}
