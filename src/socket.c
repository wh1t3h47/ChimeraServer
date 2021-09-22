#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Function: create_socket
//
// create a socket, bind it and listen for connections
//
// Parameters:
//     port - the port to listen on
//
// Returns:
//     the socket file descriptor
//

int create_socket(int port) {
    int sockfd;
    struct sockaddr_in serv_addr;
    // empty the struct
    memset(&serv_addr, '0', sizeof(serv_addr));

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set up the server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Listen for connections
    listen(sockfd, 5);

    return sockfd;
}
