#include "support.h"

int set_up_4(unsigned short *port)
{
    int sockfd = 0;
    struct sockaddr_in name;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void accept_request(void *new_connection)
{
    CONNECTION_t *connection = (CONNECTION_t *)new_connection;
    int client = connection->client;
    char *path_to_root = connection->path_to_root;

    char buff[BUFF_SIZE_BIG];
    int command_len;

    command_len = get_line(client, buff, sizeof(buff));

    int i = 0;

    // get the method from the first line
    char method[BUFF_SIZE_BIG];

    while (!isspace(buff[i]) && (i < command_len))
    {
        method[i] = buff[i];
        i++;
    }
    method[i] = '\0';

    // jump over spaces
    while (isspace(buff[i]) && (i < command_len))
    {
        i++;
    }

    // read the relevant file path
    char relevant_file_path[BUFF_SIZE_BIG];
    int j = 0;
    while (!isspace(buff[i]) && (i < command_len))
    {
        relevant_file_path[j] = buff[i];
        i++;
        j++;
    }

    relevant_file_path[j] = '\0';

    char filepath[BUFF_SIZE_BIG];
    strcpy(filepath, path_to_root);
    strcat(filepath, relevant_file_path);

    // read and throw all useless infromation
    buff[0] = 'A';
    buff[1] = '\0';
    while ((command_len > 0) && (strcmp("\n", buff)))
    {
        command_len = get_line(client, buff, sizeof(buff));
    }

    // For this project, not supported method with get 404
    if (!is_get_method(method))
    {
        not_found(client);
    }
    else if (has_invalid_component(filepath))
    {
        not_found(client);
    }
    else
    {
        // printf("we are also here");
        // fflush(stdout);

        // find the extension
        char *extension = strrchr(filepath, '.');

        FILE *file_pointer = NULL;
        file_pointer = fopen(filepath, "r");

        if (file_pointer == NULL)
        {
            not_found(client);
        }
        else
        {
            serve_file(client, file_pointer, extension);
            fclose(file_pointer);
        }

        // printf("%s", filepath);
        // fflush(stdout);
    }

    close(client);
}

/* read in a line of data, one char at a time, if the char is '\n', it should be the line
 * if '\r' check id the next is '\n', if so, replace '/r/n' to '/n' , it become a line
 * if not, stop reading and it become a line
 */
int get_line(int sockfd, char *buff, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sockfd, &c, 1, 0);
        // DEBUG printf("%02X\n", c);
        if (n > 0)
        {
            if (c == '\r')
            {
                // take a look at the next char, if '\n', read it
                n = recv(sockfd, &c, 1, MSG_PEEK);
                // DEBUG printf("%02X\n", c);
                if ((n > 0) && (c == '\n'))
                {
                    recv(sockfd, &c, 1, 0);
                }
                else
                {
                    // there is no char after it or is belonging to next line, jump out of the loop
                    c = '\n';
                }
            }
            buff[i] = c;
            i++;
        }
        else
        {
            c = '\n';
        }
    }
    buff[i] = '\0';

    return (i);
}

/* this function is used to reply a 404 to the client
 */
void not_found(int client)
{
    char buff[BUFF_SIZE_BIG];
    sprintf(buff, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buff, strlen(buff), 0);
    sprintf(buff, "\r\n");
    send(client, buff, strlen(buff), 0);
}

void serve_file(int client, FILE *file_pointer, char *extension)
{
    char buff[BUFF_SIZE_BIG];
    sprintf(buff, "HTTP/1.0 200 OK\r\n");
    send(client, buff, strlen(buff), 0);

    // sent differnt content-type depend on the extension
    if (extension == NULL)
    {
        sprintf(buff, "Content-Type: application/octet-stream\r\n");
        send(client, buff, strlen(buff), 0);
    }
    else if (is_same_str(extension, ".html"))
    {
        sprintf(buff, "Content-Type: text/html\r\n");
        send(client, buff, strlen(buff), 0);
    }
    else if (is_same_str(extension, ".jpg"))
    {
        sprintf(buff, "Content-Type: image/jpeg\r\n");
        send(client, buff, strlen(buff), 0);
    }
    else if (is_same_str(extension, ".css"))
    {
        sprintf(buff, "Content-Type: text/css\r\n");
        send(client, buff, strlen(buff), 0);
    }
    else if (is_same_str(extension, ".js"))
    {
        sprintf(buff, "Content-Type: text/javascript\r\n");
        send(client, buff, strlen(buff), 0);
    }
    else
    {
        sprintf(buff, "Content-Type: application/octet-stream\r\n");
        send(client, buff, strlen(buff), 0);
    }

    // printf("hello");
    // printf("%s", extension);
    // fflush(stdout);

    sprintf(buff, "\r\n");
    send(client, buff, strlen(buff), 0);

    // send the file
    int n_count;
    while ((n_count = fread(buff, sizeof(char), BUFF_SIZE_BIG, file_pointer)) > 0)
    {
        send(client, buff, n_count, 0);
    }
}

// this funtion would check if the string is "GET"
int is_get_method(char *method)
{
    int method_len = strlen(method);
    if (method_len < 3)
    {
        return 0;
    }

    if (method[0] == 'G' && method[1] == 'E' && method[2] == 'T')
    {
        return 1;
    }

    return 0;
}

int is_same_str(char *extension, char *stand_extension)
{
    int extension_len = strlen(extension);
    int stand_extension_len = strlen(stand_extension);

    if (extension_len != stand_extension_len)
    {
        return 0;
    }

    for (int i = 0; i < stand_extension_len; i++)
    {
        if (extension[i] != stand_extension[i])
        {
            return 0;
        }
    }

    return 1;
}

int has_invalid_component(char *file_path)
{
    int file_path_len = strlen(file_path);

    if (file_path_len < 3)
    {
        return 0;
    }

    // printf("There are %d chars in  %s\n",file_path_len, file_path);
    // fflush(stdout);

    for (int i = 0; i < (file_path_len - 2); i++)
    {
        // printf("The %d char is %c\n",i, file_path[i]);
        // fflush(stdout);
        if (file_path[i] == '.' && file_path[i + 1] == '.' && file_path[i + 2] == '/')
        {
            return 1;
        }
    }

    return 0;
}

int set_up_6(unsigned short *port)
{
    int sockfd = 0;
    struct sockaddr_in6 name;

    sockfd = socket(PF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket6");
        exit(EXIT_FAILURE);
    }

    memset(&name, 0, sizeof(name));
    name.sin6_family = AF_INET6;
    name.sin6_port = htons(*port);
    name.sin6_addr = in6addr_any;

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt6");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
        perror("bind6");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("listen6");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}