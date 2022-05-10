#define _POSIX_C_SOURCE 200112L

#define BUFF_SIZE_BIG 2048

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
//#include <pthread.h>
#include <stdlib.h>

typedef struct {
    char *path_to_root;
    int client;
} CONNECTION_t;

int set_up_4(unsigned short *port);
void accept_request(void *new_connection);
int get_line(int sockfd, char *buff, int size);
void not_found(int client);
void serve_file(int client, FILE *file_pointer, char *extension);
int is_get_method(char *method);
int is_same_str(char *extension, char *stand_extension);
