/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

void remove_coma(char* str)
{
    char *pr = str, *pw = str;
    char coma = ',';

    while (*pr) {
        *pw = *pr++;
        pw += (*pw != coma);
    }
    *pw = '\0';
}

void remove_left_bracket(char* str)
{
    char *pr = str, *pw = str;
    char coma = '{';

    while (*pr) {
        *pw = *pr++;
        pw += (*pw != coma);
    }
    *pw = '\0';
}

void parse_buff (char *buff)
{
    const char delimiters[] = " ";
    const char fileInfoDelimiter[] = "}";
    const char positionsDelimiter[] = ",";

    char *running;

    char *command;
    char *path;
    char *pathLengthStr;
    int pathLength;

    //Arquivo
    char *payload;
    char *nrBytesStr;
    char *nrBytesNOBUGEDStr;
    int nrBytes;
    char *offsetStr;
    int offset;
    char *offsetNOBUGEDStr;
    char* parentPath;

    char *clientIdStr;
    int clientId;
    //Diretório

    char* ownerPermissionStr;
    char* otherPermissionStr;

    char* dirName;
    char* dirNameLengthStr;
    int dirNameLength;

    int fileLength;
    char* fileLengthStr;

    char* owner;
    char* permissions;

    char* allfilenames;
    char* allfilepositions;

    char* initialPositionStr;
    int initialPosition = 0;
    char* finalPositionStr;
    int finalPosition = 0;
    char* token1;
    char* token2;
    char* token3;

    int numberOfFiles = 0;

    int i = 0;
    int j = 0;
    int k = 0;

    running = strdup(buff);

    command = strsep(&running, delimiters);
    remove_coma(command);
    printf("Command: %s \n", command);

    //MANIPULACAO DE ARQUIVOS
    if( strcmp(command, "RD-REP") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);
        printf("\n");
        printf("Path: %s", path); 
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("Path Length: %s", pathLengthStr); 
        printf("\n");

        payload = strsep(&running, delimiters);
        remove_coma(payload);
       
        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        //gambiarra
        nrBytesNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(nrBytesStr));
        strcpy(nrBytesNOBUGEDStr, nrBytesStr);
        nrBytes = atoi(nrBytesStr);

        printf("Payload: ");
        for(k = 0; k < nrBytes; k++)
        {
            printf("%c", payload[k]);
        } 
        printf("\n");

        printf("Número de Bytes Lidos: %s", nrBytesStr); 
        printf("\n");

        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        //gambiarra
        offsetNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(offsetStr));
        strcpy(offsetNOBUGEDStr, offsetStr);
        offset = atoi(offsetStr);
        printf("Offset: %s", offsetStr); 
        printf("\n");
        printf("\n");

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");
    }

    if( strcmp(command, "WR-REP") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);
        printf("\n");
        printf("Path: %s", path); 
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("Path Length: %s", pathLengthStr); 
        printf("\n");

        payload = strsep(&running, delimiters);
        remove_coma(payload);
        printf("Payload: %s", payload); 
        printf("\n");

        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        //gambiarra
        nrBytesNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(nrBytesStr));
        strcpy(nrBytesNOBUGEDStr, nrBytesStr);
        nrBytes = atoi(nrBytesStr);
        printf("Número de Bytes Lidos: %s", nrBytesStr); 
        printf("\n");

        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        //gambiarra
        offsetNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(offsetStr));
        strcpy(offsetNOBUGEDStr, offsetStr);
        offset = atoi(offsetStr);
        printf("Offset: %s", offsetStr); 
        printf("\n");
        printf("\n");

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");
    }

    if( strcmp(command, "FI-REP") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);
        printf("\n");
        printf("Path: %s", path);
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("\n");
        printf("Path length: %s", pathLengthStr);

        owner = strsep(&running, delimiters);
        remove_coma(owner);
        printf("\n");
        printf("Owner: %s", owner); 

        permissions = strsep(&running, delimiters);
        remove_coma(permissions);
        printf("\n");
        printf("Permissions: %c%c%c", permissions[0], permissions[1], permissions[2]); 
        printf("\n");

        fileLengthStr = strsep(&running, delimiters);
        remove_coma(fileLengthStr);
        printf("File length: %s", fileLengthStr); 
        printf("\n");
        printf("\n");

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");
    }

    //MANIPULACAO DE DIRETORIOS
    if( strcmp(command, "DC-REP") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        if(pathLength == 0)
        {
            printf("Operação não pode ser realizada \n");
        }
        else
        {
            printf("Novo Path contendo novo diretorio: %s\n", path);
            printf("Tamanho do path: %d\n", pathLength);
            printf("cliente: %s\n", clientIdStr);
            printf("\n");
        }
    }

    if( strcmp(command, "DR-REP") == 0 )
    {       
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        if(pathLength == 0)
        {
            printf("Operação não pode ser realizada \n");
        }
        else
        {
            printf("Novo Path com remocao de diretorio: %s \n", path);
            printf("Tamanho do path: %d \n", pathLength);
            printf("cliente: %s\n", clientIdStr);
            printf("\n");
        }

    }

    if( strcmp(command, "DL-REP") == 0 )
    {   
        allfilenames =  strsep(&running, delimiters);
        remove_coma(allfilenames);
        printf("\n");
        printf("allfilenames: %s", allfilenames); 
        printf("\n");

        allfilepositions = strsep(&running, delimiters);
        printf("positions: %s", allfilepositions); 
        printf("\n");   

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");
   
        while(j < strlen(allfilepositions))
        {
            if(allfilepositions[j] == '}')
            {
                numberOfFiles++;
            }

            j++;
        }

        j = 0;

        //printf("Numero de files : %d \n", numberOfFiles);
        //printf("Tamanho de allfile: %d \n", strlen(allfilepositions));

        while( j < numberOfFiles )
        {   
            //finalPositionStr = {x,y
            finalPositionStr =  strsep(&allfilepositions, fileInfoDelimiter);
            //finalPositionStr = x,y
            remove_left_bracket(finalPositionStr);

            //initialPositionStr = x
            //finalPositionStr = y
            initialPositionStr = strsep(&finalPositionStr,positionsDelimiter);

            //printf("Posicao Inicial: %s\n", initialPositionStr);
            //printf("Posicao Final: %s\n", finalPositionStr);

            initialPosition = atoi(initialPositionStr);
            finalPosition = atoi(finalPositionStr);

            printf("Nome: ");
            for(i = initialPosition; i < finalPosition+1; i++)
            {
                printf("%c",allfilenames[i]);
            }
            printf("\n");

            j++;
         }

    } 
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    char response[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    bzero(response, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    if (n < 0)
      error("ERROR in sendto");


    /* print the server's reply */
    n = recvfrom(sockfd, response, BUFSIZE, 0, &serveraddr, &serverlen);
    if (n < 0)
      error("ERROR in recvfrom");
    printf("Echo from server: %s\n ", response);

    parse_buff(response);

    return 0;
}