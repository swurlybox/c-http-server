#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT    ("3000")
#define BACKLOG (10)    /* Maximum allowed connections in listen queue. */
#define RECV_SZ (10000) /* Reasonable request size */
#define STATUS_OK   (0)
#define STATUS_EXIT (1)

typedef struct {
    int serv_sock;
    int client_sock;
} resources_t;

resources_t rsrc;

void sig_handler(int signo){
    /* cleanup resources */
    printf("\ncleaning up resources...\n");
    close(rsrc.serv_sock);
    close(rsrc.client_sock);
    exit(1);
}

/* Single-threaded listener on port 3000. */
int main() {
    struct addrinfo hints, *res; 
    struct sockaddr_storage client_addr;
    char recvbuf[RECV_SZ];
    int addr_size = sizeof client_addr;
    signal(SIGINT, sig_handler);

    memset(&hints, 0, sizeof hints);
    hints.ai_family     = AF_UNSPEC;    /* either IPv4 or IPv6 is fine */
    hints.ai_socktype   = SOCK_STREAM;  /* connection-oriented (def. TCP) */
    hints.ai_flags      = AI_PASSIVE;   /* choose a local socket */

    printf("getting local socket addrinfo...\n");
    if (getaddrinfo(NULL, PORT, &hints, &res)) {
        printf("error with getaddrinfo()\n");
        exit(1);
    }

    printf("creating a local socket to handle incoming requests...\n");
    rsrc.serv_sock = socket(res->ai_family, res->ai_socktype, 
        res->ai_protocol);
    if (rsrc.serv_sock < 0) {
        printf("error creating a socket\n");
        exit(1);
    }

    printf("binding local socket to a specific ip address and port...\n");
    if (bind(rsrc.serv_sock, res->ai_addr, res->ai_addrlen) < 0) {
        printf("error binding the socket\n");
        exit(1);
    }
    freeaddrinfo(res);
    
    printf("listening on local socket for connections...\n");
    if (listen(rsrc.serv_sock, BACKLOG) < 0) {
        printf("error listening on socket\n");
        exit(1);
    }

    printf("socket ready to accept connections!\n");
    while(1) {
        if ((rsrc.client_sock = accept(rsrc.serv_sock, 
            (struct sockaddr *) &client_addr, &addr_size)) < 0) {
            printf("error socket couldn't accept an incoming connection\n");
        }

        memset(&recvbuf, 0, sizeof recvbuf);
        if (recv(rsrc.client_sock, recvbuf, sizeof recvbuf, 0) < 0) {
            printf("error receiving bytes from connection\n");
        }

        printf("%s\n", recvbuf);
        
        /* TODO:
         * After receiving the data (presumably the HTTP GET request), 
         * we need to parse it! This will need to be its own method.
         * 
         * Once we know for sure its a valid HTTP request, the parser
         * will respond with the appropriate response depending on the
         * request. However, typically after accepting a connection, we'd
         * fork a child process and then handle that request there. But
         * single-threaded web server is okay for now, I'd rather not deal
         * with reaping children right now.
         *
         * */


        close(rsrc.client_sock);
    }
   
    printf("cleaning up resources...\n"); 
    close(rsrc.client_sock);
    close(rsrc.serv_sock);
    return 0;
}
