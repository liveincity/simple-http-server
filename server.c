#include "support.h"

int main(int argc, char *argv[])
{
    int portocol_num;
    unsigned short port;
    char *path_to_root;

    if (argc != 4)
    {
        perror("ERROR, wrong parameters number");
        exit(EXIT_FAILURE);
    }

    // store parameters
    sscanf(argv[1], "%d", &portocol_num);
    sscanf(argv[2], "%hd", &port);
    path_to_root = argv[3];

    /*
    char *teststring = "apple";
    char *newstring = (char *)malloc(strlen(teststring) + strlen(path_to_root));
    strcpy(newstring, path_to_root);
    strcat(newstring, teststring);
    free(newstring);
    */

    printf(" %d %hd %s", portocol_num, port, path_to_root);

    int server_sock = -1;
    //int client_sock = -1;
    // struct sockaddr_in client_name;

    // socklen_t client_name_len = sizeof(client_name);
    // pthread_t newthread;

    if (portocol_num == 4)
    {
        server_sock = set_up_4(&port);
    }

    printf("Our server is listening on %hd", port);

    close(server_sock);
}