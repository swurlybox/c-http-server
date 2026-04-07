#ifndef HTTPDRIVER_H
#define HTTPDRIVER_H

/* Examines an HTTP request in buffer, and sends an appropriate response out
 * to the client. */
void http_parser_main(int client_sock, void *buffer, int buflen);

#endif
