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

    printf("%s", buff);

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
                //take a look at the next char, if '\n', read it
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
