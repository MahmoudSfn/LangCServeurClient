#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 10
#define MAXTEXT 100
#define MAXROCHER 20

void print_msg(char *talker, char * chat) {
    fputs(talker, stdout);
    fputs(": ", stdout);
    fputs(chat, stdout);
    fflush(stdout);
}
void read_header(int sock, char * username) {
    int loglen ;
    read(sock, &loglen, 1);
    read(sock, username, loglen);
}

int main(int argc, char * argv[])
{


    int socket_RV, socket_service;
    int pidFils;
    int port = 6543;
    char nom[30];
    char commandeWrite[80];
    struct sockaddr_in adr, adresse;
    socklen_t lgadresse;//sizeof(struct sockaddr_in);

    char *FileName = "./carte.txt";
    FILE *FileDialog;
    int coordonnes_rocher[MAXROCHER][2];
    int counter = 0;
    int coo_rocher_max = 0;
    int dim[2];
    int vent[2]; // vent:[Force : {0->4 in N},
                 //       direction : {N -> 1, S -> 2, E -> 3, O -> 4}] 

    if (argc!=2)
    {
        fprintf(stderr,"Usage : %s port-number", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

    if ((socket_RV=socket(AF_INET, SOCK_STREAM, 0)) ==-1)
    {
        perror("socket rendez-vous");
        exit(1);
    }
    
    if (gethostname(nom, 30)==-1)
    {
        perror("Qui suis-je ?");
        exit(1);
    }
    
    printf("User: %s - %d; Machine: %s\n", getlogin(), geteuid(), nom);

    adr.sin_family=AF_INET;
    adr.sin_port=htons(port);
    adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_RV, (struct sockaddr *) &adr, sizeof(adr))==-1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(socket_RV,1)==-1)
    {
        perror("listen");
        exit(1);
    }
    
    socket_service=accept(socket_RV, (struct sockaddr *)&adresse, &lgadresse);
    close(socket_RV);

    char c;
    char *talker = (char*)malloc(MAXNAME);
    char *chat =  (char*)malloc(MAXTEXT);
    char *begchat = chat;
    switch(pidFils=fork()) {
    case -1:
        perror("fork");
        exit(1);

    case 0:
        //READ DATA FROM FILE AND SAVE IN VARIABLE!
        

        //1) Open the file "carte.txt":
        FileDialog = fopen(FileName, "r");
        if (FileDialog == NULL) {
            printf("Could not open file %s", FileName);
            return 0;
        }
        //2) Read data and fill vars :
        char str[10];
        counter = 0;
        while(fgets(str, 10, FileDialog) != NULL){
            char *p;
            p = strtok(str, ":");
            if (p != NULL){
                if (strcmp(p,"D") == 0) {
                    p = strtok(NULL, ":");
                    p = strtok(p, ".");
                    dim[0] = atoi(p);
                    p = strtok(NULL, ".");
                    dim[1] = atoi(p);
                    //printf("l = %d, L = %d\n",dim[0], dim[1]);
                }
                else if (strcmp(p,"V") == 0) {
                    p = strtok(NULL, ":");
                    p = strtok(p, ".");
                    vent[0] = atoi(p);
                    p = strtok(NULL, ".");
                    vent[1] = atoi(p);
                    //printf("Force = %d, Direction = %d\n",vent[0], vent[1]);
                }
                else if (strcmp(p,"R") == 0) {
                    p = strtok(NULL, ":");
                    p = strtok(p, ".");
                    coordonnes_rocher[counter][0] = atoi(p);
                    p = strtok(NULL, ".");
                    coordonnes_rocher[counter][1] = atoi(p);
                    //printf("x = %d, y = %d\n", coordonnes_rocher[counter][0], coordonnes_rocher[counter][1]);
                    counter++;
                }
            }
        }
        /*dont forget to set a condition tat if it wasn't*/
        coo_rocher_max = counter;
        //printf("max = %d\n",coo_rocher_max);
        //END READ DATA FROM FILE
        //Affichage de l'envirannement
        int a,b, siOno;
        for (a = 0; a < dim[0]; a++)
        {
            for (b = 0; b < dim[1]; b++)
            {
                siOno = 0;
                for (counter = 0; counter < coo_rocher_max; counter++)
                {
                    if ((a == coordonnes_rocher[counter][0]) && (b == coordonnes_rocher[counter][1]))
                    {
                        siOno = 1;
                    }
                }

                if(siOno == 0){
                    printf("O ");
                } else { printf("R "); }
            }
            printf("\n");
        }
        //TALKING FUNCTION
        read_header(socket_service, talker);
        printf("%s is connected\n", talker);
        do {
            c = EOF;
            read(socket_service, &c, 1);
            *chat = c;
            chat++;
            if (c == '\n' || c == EOF)
            {
                *chat = '\0';
                chat = begchat;
                print_msg(talker, chat);
            }
        } while (c!=EOF);
        //END TALKING

        fprintf(stderr,"Cote serveur: fin fils\n");
        break;

    default:
        do {
            c=getchar();
            write(socket_service, &c, 1);
        } while (c!=EOF);
        kill(pidFils, SIGTERM);
        fprintf(stderr,"Cote serveur: fin pere\n");
    }
    return 0;
}




