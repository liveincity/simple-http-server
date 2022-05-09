#include "support.h"

int main(int argc, char *argv[]) {
    int portocol_num;
    int port;
    char *path_to_root;

    if (argc < 3) {
        perror("ERROR, not enough information");
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%d", &portocol_num);
    sscanf(argv[2], "%d", &port);
    path_to_root = argv[3];

    /*
    char *teststring = "apple";
    char *newstring = (char *)malloc(strlen(teststring) + strlen(path_to_root));
    strcpy(newstring, path_to_root);
    strcat(newstring, teststring);
    free(newstring);
    */
    
    printf(" %d %d %s", portocol_num, port, path_to_root);
}