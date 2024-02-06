#ifndef __client_h__
#define __client_h__

#include <netdb.h>
#include <stdbool.h>

char *addr_string (struct addrinfo *);
char *serv_string (struct addrinfo *);
struct addrinfo *get_server_list (const char *, const char *, bool, bool);
char *web (const char *, char *, const char *, char **, bool);

#endif
