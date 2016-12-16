/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include "lista.h"

#define BUFSIZE 1024
#define TRUE 1
#define FALSE 0
extern  int alphasort(); 

char* responseBuffer;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int select_all_files(const struct dirent *entry)
{
  if ((entry->d_type == DT_DIR) ||
      (strcmp(entry->d_name, ".") == 0) || 
      (strcmp(entry->d_name, "..") == 0) )  
  {
    return (FALSE); 
  }
  else
  {
    return (TRUE);
  }
}

char * get_current_directory()
{
    char* currentDir;

    currentDir = (char *) malloc (BUFSIZE * sizeof(char));

    if (getwd(currentDir) == NULL )
    {
      printf("Error getting path\n"); exit(0);
    }

    return currentDir;
}

Lista * create_initial_directory(Lista * root)
{
    char *fullPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;
    
    fullPath = get_current_directory();

    strcat(fullPath, "/SFS-root-di");

    mkdir(fullPath, permissao);
    root = lista_add(root, "/", -1, 'W', 'W');
    chdir(fullPath);
    free(fullPath);

    return root;
}

int doIHavePermission(char myIntention, char entryPermission)
{
  if ( entryPermission == 'W' )
  {
    return TRUE;
  }
  else if ( myIntention == 'W')
  {
    return FALSE;
  }
  return TRUE;
}

Lista * directory_create(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char *dirName)
{   
    char *fullPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;
    
    fullPath = get_current_directory();

    strcat(fullPath, "/SFS-root-di");
    strcat(fullPath, path);

    if(strcmp(path, "/") != 0)
    {
      strcat(fullPath, "/");
    }
    
    strcat(fullPath, dirName);

    mkdir(fullPath, permissao);
    root = lista_add(root, fullPath, cliente, owner_permission, other_permission);
    free(fullPath);

    return root;
}

void directory_show_info(char *path)
{   
    char *fullPath;
    int count,i; 
    struct direct **files; 

    fullPath = get_current_directory();

    strcat(fullPath, path);
    printf("%s\n", fullPath);

    count = scandir(fullPath, &files, select_all_files, alphasort); 

    /* If no files found, make a non-selectable menu item */ 
    if (count <= 0) 
    {
        printf("No files in this directory\n"); exit(0); 
    }

    printf("Number of files = %d\n",count); 

    for (i=1;i<count+1;++i) 
      printf("%s  ", files[i-1]->d_name);

    printf("\n"); /* flush buffer */ 
}

Lista * directory_delete(Lista * root, char *path, char *dirName)
{   
    char *fullPath;

    fullPath = get_current_directory();

    strcat(fullPath, "/SFS-root-di");
    strcat(fullPath, path);
   
    if(strcmp(path, "/") != 0)
    {
      strcat(fullPath, "/");
    }
   
    strcat(fullPath, dirName);

    rmdir(fullPath);
    root = lista_delete(root, fullPath);
    free(fullPath);

    return root;
}

Lista * file_create(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char * payload, int numBytes)
{
    int fileDescriptor;
    char *fullPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;


    fullPath = get_current_directory();

    strcat(fullPath, path);

    fileDescriptor = open(fullPath, O_CREAT | O_WRONLY, permissao);

    write(fileDescriptor, payload, numBytes);

    root = lista_add(root, fullPath, cliente, owner_permission, other_permission);
    close(fileDescriptor);
    return root;
}

void file_read(Lista * root, int cliente, char *path, char * payload, int offset, int numBytes)
{
    int i, fileDescriptor;
    char *fullPath;
    char entryPermission;



    fullPath = get_current_directory();
    
    strcat(fullPath, path);

    fileDescriptor = open(fullPath, O_RDONLY);
    pread(fileDescriptor, payload, numBytes, offset);

    // printf("%s\n", payload);

    for (i = 0; i < numBytes; i++)
    {
      printf("%c", payload[i]);
    }

    printf("\n");
   close(fileDescriptor);
}

void file_write(Lista * root, int cliente, char *path, char * payload, int offset, int numBytes)
{
    int fileDescriptor;
    char entryPermission;
    char *fullPath;

    fullPath = get_current_directory();

    strcat(fullPath, path);

    entryPermission = lista_get_entry_permission(root, fullPath, cliente);

    printf("Entry permission is %c\n", entryPermission);

    if(!doIHavePermission('W',entryPermission))
    {
      printf("Voce nao tem permissao para escrever nesse arquivo\n");
      return;
    }

    fileDescriptor = open(fullPath, O_WRONLY);

    pwrite(fileDescriptor, payload, numBytes, offset);
    close(fileDescriptor);
}

int file_info(char *path)
{
    struct stat sb;
    char *fullPath;

    fullPath = get_current_directory();

    strcat(fullPath, path);

    stat(fullPath, &sb);

    printf("Tamanho do arquivo: %d\n ", sb.st_size);
    return sb.st_size;
}

Lista * file_delete(Lista * root, char *path)
{
    int success;
    char *fullPath;

    fullPath = get_current_directory();

    strcat(fullPath, path);

    success = remove(fullPath);
    if(success == -1)
    {
      printf("Failed to delete %s\n", path);
    }
    root = lista_delete(root, fullPath);
}

Lista * file_modify(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char * parentPath, char * payload, int offset, int numBytes)
{
  printf(" PARENT PATH .... %s\n", parentPath);
  printf(" CHILD PATH .... %s\n", path);
  printf("%d\n", offset);
  if (numBytes == 0)
  {
    printf("Vou deletar\n");
    root = file_delete(root, path);
  }
  else if (lista_seek(root, path))
  {
    printf("Cheguei pra escrever\n");
    file_write(root, cliente, path, payload, offset, numBytes);
  }
  else if (lista_seek(root, "/"))
  {
    printf("lugar certo \n");
    root = file_create(root, cliente, owner_permission, other_permission, path, payload, numBytes);
  }
  else
  {
    printf("Arquivo nao encontrado\n");
  }
  return root;
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


char* parentDirectory( char* path, int pathLength )
{
  int i = 0;
  int slashIndex;
  char* parent;

  while( i < pathLength )
  {
    if(path[i] == '/')
      slashIndex = i;

    i++;
  }

  i = 0;
  parent = (char*)malloc(sizeof(char)*slashIndex+1);

  while( i <= slashIndex)
  {
    parent[i] = path[i];
    i++;
  }
  return parent;
} 

Lista * parse_buff (char *buff, int *cmd, char *name, Lista *root )
{
    const char delimiters[] = " ";
    char *running;


    char *command;
    char *path;
    char *pathLengthStr;
    int pathLength;

    //Arquivo
    char *payload;
    char *nrBytesStr;
    int nrBytes;
    char *offsetStr;
    int offset;
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

    running = strdup(buff);

    command = strsep(&running, delimiters);
    remove_coma(command);
    printf("Command: %s \n", command);


    //MANIPULACAO DE ARQUIVOS
    if( strcmp(command, "RD-REQ") == 0 )
    {
        //printf("oi");
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);

        payload = strsep(&running, delimiters);
        remove_coma(payload);

        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        nrBytes = atoi(nrBytesStr);

        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        offset = atoi(offsetStr);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        file_read(root, clientId, path, payload, offset, nrBytes);


        responseBuffer = strcat(responseBuffer, "RD-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, payload);
        responseBuffer = strcat(responseBuffer, ", ");
        //not sure of nrBytes
        responseBuffer = strcat(responseBuffer, nrBytesStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, offsetStr);
    }

    if( strcmp(command, "WR-REQ") == 0 )
    {
        //printf("oi");
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);

        payload = strsep(&running, delimiters);
        remove_coma(payload);

        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        nrBytes = atoi(nrBytesStr);

        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        offset = atoi(offsetStr);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        ownerPermissionStr = strsep(&running, delimiters);
        remove_coma(ownerPermissionStr);

        otherPermissionStr = strsep(&running, delimiters);
        remove_coma(otherPermissionStr);

        parentPath = parentDirectory(path, pathLength);

        root = file_modify(root, clientId, ownerPermissionStr[0], otherPermissionStr[0], path, parentPath, payload, offset, nrBytes);

        responseBuffer = strcat(responseBuffer, "WR-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, " ");
        responseBuffer = strcat(responseBuffer, ", ");
        //not sure of nrBytes
        responseBuffer = strcat(responseBuffer, nrBytesStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, offsetStr);
          //free(parentPath);
    }

    if( strcmp(command, "FI-REQ") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        
        fileLength = file_info(path);        
    }

    //MANIPULACAO DE DIRETORIOS
    if( strcmp(command, "DC-REQ") == 0 )
    {
          //create_directory();
        //Lista * directory_create(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char *dirName)
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        
        dirName = strsep(&running, delimiters);
        remove_coma(dirName);

        dirNameLengthStr = strsep(&running, delimiters);
        remove_coma(dirNameLengthStr);
        dirNameLength = atoi(dirNameLengthStr);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        ownerPermissionStr = strsep(&running, delimiters);
        remove_coma(ownerPermissionStr);

        otherPermissionStr = strsep(&running, delimiters);
        remove_coma(otherPermissionStr);

        root = directory_create(root, clientId, ownerPermissionStr[0], otherPermissionStr[0], path, dirName);
    }

    if( strcmp(command, "DR-REQ") == 0 )
    {       
        //delete_directory();

          //Lista * directory_delete(Lista * root, char *path, char *dirName)
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        
        dirName = strsep(&running, delimiters);
        remove_coma(dirName);

        dirNameLengthStr = strsep(&running, delimiters);
        remove_coma(dirNameLengthStr);
        dirNameLength = atoi(dirNameLengthStr);
        
        /*
        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);

        */
        root = directory_delete(root, path, dirName);
    }

    if( strcmp(command, "DL-REQ") == 0 )
    {
        //void directory_show_info(char *path)
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        

        /*
        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        */

        directory_show_info(path);
    }

    return root;
}

int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buff[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  char name[BUFSIZE];   // name of the file received from client
  int cmd;              // cmd received from client

  //char responseBuff[BUFSIZE];
  char payloadToWriteFirst[BUFSIZE];
  char payloadToWriteSecond[BUFSIZE];
  char payloadToRead[BUFSIZE];
  char payloadToRead2[BUFSIZE];
  char payloadToRead3[BUFSIZE];

  Lista * root;

  /*
   * check command line arguments
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /*
   * socket: create the parent socket
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets
   * us rerun the server immediately after we kill it;
   * otherwise we have to wait about 20 secs.
   * Eliminates "ERROR on binding: Address already in use" error.
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /*
   * bind: associate the parent socket with a port
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr,
	   sizeof(serveraddr)) < 0)
    error("ERROR on binding");

  /*
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);

  root = lista_cria();
  root = create_initial_directory(root);

  while (1)
  {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buff, BUFSIZE);
    n = recvfrom(sockfd, buff, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");

    lista_print(root);
    root = parse_buff(buff, &cmd, name, root);



    /*
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n",
	   hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buff), n, buff);

     // lista_print(root);
     // root = directory_create("/", "dirToBeDeleted", root);
     // lista_print(root);
     // directory_show_info("/");
     // root = directory_delete("/", "dirToBeDeleted", root);
     // lista_print(root);

  
    //strcpy(payloadToWriteFirst, "manda");
   // strcpy(payloadToWriteSecond, " nudes");
  
    // file_read("/dirTeste/ultimateTeste/ultimateArquivoTeste.txt", payloadToRead, 7, 5);
    // file_write("/dirTeste/ultimateTeste/ultimateArquivoTeste.txt", payloadToWrite, 7, 5);
    // file_read("/dirTeste/ultimateTeste/ultimateArquivoTeste.txt", payloadToRead, 7, 5);

    // lista_print(root);
    // root = file_create(root, 1, 'W', 'R' , "/mandaNudes", payloadToWriteFirst, 5);
    // lista_print(root);
    // printf("Cliente 1 lendo\n");
    // file_read(root, 1, "/mandaNudes", payloadToRead, 0, 5);
    // printf("Cliente 1 escrevendo\n");
    // file_write(root, 1, "/mandaNudes", payloadToWriteSecond, 5, 6);
    // printf("Cliente 1 lendo\n");
    // file_read(root, 1, "/mandaNudes", payloadToRead2, 0, 11);
    // printf("Cliente 2 escrevendo\n");
    // file_write(root, 2, "/mandaNudes", payloadToWriteSecond, 0, 6);
    // printf("Cliente 2 lendo\n");
    // file_read(root, 2, "/mandaNudes", payloadToRead3, 0, 11);
    // root = file_delete(root, "/mandaNudes");
    // lista_print(root);

    // file_info("/dirTeste/ultimateTeste/ultimateArquivoTeste.txt");

    /*
     * sendto: echo the input back to the client
     */
    n = sendto(sockfd, responseBuffer, strlen(responseBuffer), 0,
	       (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0)
      error("ERROR in sendto");

   memset(responseBuffer,0,strlen(responseBuffer)); 
  }
}