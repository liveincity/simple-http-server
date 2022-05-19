#include "support.h"

int main(int argc, char *argv[])
{
    int portocol_num;
    unsigned short port;
    char *path_to_root;
    char *port_str;

    if (argc != 4)
    {
        perror("ERROR, wrong parameters number");
        exit(EXIT_FAILURE);
    }

    // store parameters
    sscanf(argv[1], "%d", &portocol_num);
    sscanf(argv[2], "%hd", &port);
    port_str = argv[2];
    path_to_root = argv[3];

    int server_sock = -1;
    int client_sock = -1;
    struct sockaddr_in client_name;

    socklen_t client_name_len = sizeof(client_name);

    if (!isdigit(port_str[0])) {
        perror("We need a digital port!");
        exit(EXIT_FAILURE);
    }
    
    if (portocol_num == 4)
    {
        server_sock = set_up_4(&port);
    }
    else if (portocol_num == 6)
    {
        server_sock = set_up_6(&port);
    }
    else
    {
        perror("What the hell IP ver are you using?");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        perror("mutex init error");
        exit(EXIT_FAILURE);
    }


    while (1)
    {
        // disable SIGPIPE
        struct sigaction sa;
        sa.sa_handler = handle_pipe;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGPIPE,&sa,NULL);
        
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);

        if (client_sock == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex);
        CONNECTION_t new_connection;
        new_connection.client = client_sock;
        new_connection.path_to_root = path_to_root;

        pthread_t newthread;

        // accept_request((void *)&new_connection);
        
        // creat a new thread to handle the connection
        if (pthread_create(&newthread, NULL, (void *)accept_request, (void *)&new_connection) != 0)
        {
            perror("pthread_creat");
            exit(EXIT_FAILURE);
        }
    }

    close(server_sock);
}