/*
    Usage :
        1) Compile: gcc -o client client.c 
        2) Run : ./server (machine) (PortNumber)
                 exemple :  ./server Mahs 6543 
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>

void write_header(int sock, char * username) {
  int loglen = strlen(username);
  write(sock, &loglen, 1);
  write(sock, username, loglen);
}

int main(int argc, char * argv[])
{
    /*Declaration des variables*/
    int sock;
    struct hostent * hote;
    int port = 6543;
    char *username;
    struct sockaddr_in adr;
    int pidFils;

    char c;
    /***************************/

    if (argc!=3)
    {
        fprintf(stderr,"Usage : %s machine port-number\n", argv[0]);
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket rendez-vous");
        exit(1);
    }

    hote = gethostbyname(argv[1]);
    port = atoi(argv[2]);
    username = getlogin();

    printf("User: %s - %d; Machine: %s\n", username, geteuid(), argv[1]);

    adr.sin_family=AF_INET;
    adr.sin_port=htons(port);
    bcopy(hote->h_addr, &adr.sin_addr.s_addr, hote->h_length);

    if (connect(sock, (struct sockaddr *)&adr, sizeof(adr))==-1)
    {
        perror("connect");
        exit(1);
    }

    switch(pidFils=fork()) 
    {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            do {
                c = EOF; //EndOfFile
                read(sock, &c, 1);
                putchar(c);
            } while(c != EOF);
            fprintf(stderr, "Cote client: fin fils\n");
            break;
        default:
            /* First message sends the name of talking user */
            write_header(sock, username);

            do {
                c = getchar();
                write(sock, &c, 1);
            } while (c != EOF);
            kill(pidFils, SIGTERM);
            fprintf(stderr, "Cote client: fin pere\n");
    }
    return 0;
}
