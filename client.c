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

#define MAXTEXT 100

void write_header(int sock, char * username) {
  int loglen = strlen(username);
  write(sock, &loglen, 1);
  write(sock, username, loglen);
}

int main(int argc, char * argv[])
{
  int sock;
  int port = 6543;
  int pidFils;
  char *username;
  struct hostent * hote;
  struct sockaddr_in adr;
  
  FILE *FileBat = fopen("bateau.txt", "w");
  

  if (argc!=2)
  {
    fprintf(stderr,"Usage : %s name,vitesse,direction\n", argv[0]);
    exit(1);
  }

  if ((sock=socket(AF_INET, SOCK_STREAM, 0)) ==-1)
  {
    perror("socket rendez-vous");
    exit(1);
  }
  
  fprintf(FileBat, "%s\n",argv[1]);
  printf("%s\n", argv[1]);
  fclose(FileBat);

  hote=gethostbyname("Mahs");
  port=atoi("6543");
  username = getlogin(); /*pb : signature not conform to man doc.*/
  printf("User: %s - %d; Machine: Mahs\n", username, geteuid());

  adr.sin_family=AF_INET;
  adr.sin_port=htons(port);
  bcopy(hote->h_addr, &adr.sin_addr.s_addr, hote->h_length);

  if (connect(sock, (struct sockaddr *)&adr, sizeof(adr))==-1)
  {
    perror("connect");
    exit(1);
  }

  char c;
  switch(pidFils=fork()) {

  case -1:
    perror("fork");
    exit(1);
  case 0:
    do
    {
      c = EOF;
      read(sock, &c, 1);
      putchar(c);
    }
    while (c!=EOF);
    fprintf(stderr,"Cote client: fin fils\n");
    break;
  default:
    /* First message sends the name of talking user */
    write_header(sock, username);
    do
    {
      c=getchar();
      write(sock, &c, 1);
    }
    while (c!=EOF);

    kill(pidFils,SIGTERM);
    fprintf(stderr,"Cote client: fin pere\n");
  }
  return 0;
}
