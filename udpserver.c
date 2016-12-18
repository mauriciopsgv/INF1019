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
char* directoryInfo;
char* directoryPositions;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int select_all_files(const struct dirent *entry)
{
  if ((strcmp(entry->d_name, ".") == 0) || 
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

int get_owner(Lista * root, char * path)
{
    char * fullPath;
    
    fullPath = get_current_directory();

    strcat(fullPath, path);

    return lista_get_entry_client(root, fullPath);
}

char get_permission(Lista * root, char * path, int cliente)
{
    char * fullPath;
    
    fullPath = get_current_directory();

    strcat(fullPath, path);

    return lista_get_entry_permission(root, fullPath, cliente);
}

Lista * create_initial_directory(Lista * root)
{
    char *fullPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;
    
    fullPath = get_current_directory();

    strcat(fullPath, "/SFS-root-di");

    mkdir(fullPath, permissao);
    chdir(fullPath);

    strcat(fullPath, "/"); 
    root = lista_add(root, fullPath, -1, 'W', 'W');
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
    char entryPermission;
    char *fullPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;

    fullPath = get_current_directory();

    strcat(fullPath, path);

    entryPermission = lista_get_entry_permission(root, fullPath, cliente);

    if(!doIHavePermission('W',entryPermission))
    {
      printf("Voce nao tem permissao para criar um diretorio nesse diretorio\n");
      free(fullPath);
      return root;
    }

    if(strcmp(path, "/") != 0)
    {
      strcat(fullPath, "/");
    }
    
    strcat(fullPath, dirName);

    printf("%s \n", fullPath);

    mkdir(fullPath, permissao);
    root = lista_add(root, fullPath, cliente, owner_permission, other_permission);
    free(fullPath);

    return root;
}

void directory_show_info(char *path)
{   
    char *fullPath;
    int count,i,j; 
    struct direct **files;

    int initialPosition;
    int finalPosition;
    int lastPosition;

    char initialPositionStr[10];
    char finalPositionStr[10]; 

    j = 0;

    //Aloca vetor com informações de diretório
    directoryInfo = (char*)malloc(sizeof(char)*BUFSIZE);
    strcpy(directoryInfo, "");
    //Aloca vetor com posição das informações do diretorio
    directoryPositions = (char*)malloc(sizeof(char)*BUFSIZE);
    strcpy(directoryPositions, "");

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
    { 
      strcat(directoryInfo, files[i-1]->d_name);

      if( j == 0 )
      {
        initialPosition = 0;
        finalPosition = strlen(directoryInfo) - 1;
        lastPosition = finalPosition;

        sprintf(initialPositionStr, "%d", initialPosition);
        sprintf(finalPositionStr, "%d", finalPosition);

        strcat(directoryPositions,"{");
        strcat(directoryPositions, initialPositionStr);
        strcat(directoryPositions, ",");
        strcat(directoryPositions, finalPositionStr);
        strcat(directoryPositions, "}");
      }
      else
      {
        initialPosition = lastPosition + 1;
        finalPosition = strlen(directoryInfo) - 1;
        lastPosition = finalPosition;

        sprintf(initialPositionStr, "%d", initialPosition);
        sprintf(finalPositionStr, "%d", finalPosition);

        //strcat(directoryPositions, "-");
        strcat(directoryPositions,"{");
        strcat(directoryPositions, initialPositionStr);
        strcat(directoryPositions, ",");
        strcat(directoryPositions, finalPositionStr);
        strcat(directoryPositions, "}");
      }

      printf("%s  ", files[i-1]->d_name);

      j++;
    }

    printf("Directory Info: %s \n", directoryInfo);

    printf("\n"); /* flush buffer */ 
}

Lista * directory_delete(Lista * root, int cliente, char *path, char *dirName)
{   
    char entryPermission;
    char *fullPath;

    fullPath = get_current_directory();

    strcat(fullPath, path);

    entryPermission = lista_get_entry_permission(root, fullPath, cliente);

    if(!doIHavePermission('W',entryPermission))
    {
      printf("Voce nao tem permissao para deletar um diretorio nesse diretorio\n");
      free(fullPath);
      return root;
    }
   
    if(strcmp(path, "/") != 0)
    {
      strcat(fullPath, "/");
    }
   
    strcat(fullPath, dirName);

    printf("%s \n", fullPath);

    rmdir(fullPath);
    root = lista_delete(root, fullPath);
    free(fullPath);

    return root;
}

Lista * file_create(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char *parentPath, char * payload, int numBytes)
{
    int fileDescriptor;
    char entryPermission;
    char *fullPath, * fullParentPath;
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;


    fullPath = get_current_directory();
    fullParentPath = get_current_directory();

    strcat(fullPath, path);
    strcat(fullParentPath, parentPath);

    entryPermission = lista_get_entry_permission(root, fullParentPath, cliente);

    if(!doIHavePermission('W',entryPermission))
    {
      printf("Voce nao tem permissao para criar um arquivo nesse diretorio\n");
      free(fullPath);
      free(fullParentPath);
      return root;
    }
    
    fileDescriptor = open(fullPath, O_CREAT | O_WRONLY, permissao);

    write(fileDescriptor, payload, numBytes);

    root = lista_add(root, fullPath, cliente, owner_permission, other_permission);
    close(fileDescriptor);
    free(fullPath);
    free(fullParentPath);
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

     //printf("%s\n", payload);

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

    printf("oi\n");
    printf("Tamanho do arquivo: %d\n ", sb.st_size);
    printf("oi\n");

    return sb.st_size;
}

Lista * file_delete(Lista * root, int cliente, char *path, char * parentPath)
{
    int success;
    char entryPermission;
    char *fullPath, *fullParentPath;

    fullPath = get_current_directory();
    fullParentPath = get_current_directory();

    strcat(fullPath, path);
    strcat(fullParentPath, parentPath);

    entryPermission = lista_get_entry_permission(root, fullParentPath, cliente);

    if(!doIHavePermission('W',entryPermission))
    {
      printf("Voce nao tem permissao para deletar um arquivo nesse diretorio\n");
      free(fullPath);
      free(fullParentPath);
      return root;
    }

    success = remove(fullPath);
    if(success == -1)
    {
      printf("Failed to delete %s\n", path);
    }
    root = lista_delete(root, fullPath);
    free(fullPath);
    free(fullParentPath);
    return root;
}

Lista * file_modify(Lista * root, int cliente, char owner_permission, char other_permission, char *path, char * parentPath, char * payload, int offset, int numBytes)
{
    char * fullParentPath, *fullChildPath;

    fullParentPath = get_current_directory();
    fullChildPath = get_current_directory();

    printf(" PARENT PATH .... %s\n", parentPath);
    printf(" CHILD PATH .... %s\n", path);

    strcat(fullParentPath, parentPath);
    strcat(fullChildPath, path);

    if (numBytes == 0)
    {
        printf("Vou deletar\n");
        root = file_delete(root, cliente, path, parentPath);
    }
    else if (lista_seek(root, fullChildPath))
    {
        file_write(root, cliente, path, payload, offset, numBytes);
    }
    else if (lista_seek(root, fullParentPath))
    {
        root = file_create(root, cliente, owner_permission, other_permission, path, parentPath, payload, numBytes);
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

  printf("\n");

  i=0;

  while( i <= slashIndex)
  {
    printf("%c",path[i]);
    i++;
  }
  printf("\n");

  parent = (char*)malloc(sizeof(char)*(slashIndex+1));

  i = 0;

  while( i < slashIndex)
  {
    parent[i] = path[i];
    i++;
  }
  if(slashIndex == 0)
  {
    parent[0] = '/';
    i = 1;
  }
  parent[i] = '\0';

  printf("%s", parent);
  printf("\n");

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
    char fileLengthStr[10];

    char* newPath;
    int newPathLength;
    char* newPathLengthStr;

    int owner;
    char permission_owner;
    char permission_other;
    char ownerStr[3];
    char permissionStr[5];

    int i = 0;

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
        printf("\n");
        printf("Path: %s", path); 
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("\n");
        printf("Path Length: %s", pathLengthStr); 
        printf("\n");

        payload = strsep(&running, delimiters);
        remove_coma(payload);
        printf("\n");
        printf("Payload: %s", payload); 
        printf("\n");

        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        //gambiarra
        nrBytesNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(nrBytesStr));
        strcpy(nrBytesNOBUGEDStr, nrBytesStr);
        nrBytes = atoi(nrBytesStr);
        printf("\n");
        printf("\nnrBytes: %s", nrBytesStr); 
        printf("\n");


        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        //gambiarra
        offsetNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(offsetStr));
        strcpy(offsetNOBUGEDStr, offsetStr);
        offset = atoi(offsetStr);
        printf("\n");
        printf("Offset: %s", offsetStr); 
        printf("\n");

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");

        file_read(root, clientId, path, payload, offset, nrBytes);

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        responseBuffer = strcat(responseBuffer, "RD-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, payload);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, nrBytesNOBUGEDStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, offsetNOBUGEDStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr);

        printf("\n");
        printf("%s", responseBuffer); 
        printf("\n");    
    }

    if( strcmp(command, "WR-REQ") == 0 )
    {
        //printf("oi");
        path =  strsep(&running, delimiters);
        remove_coma(path);
        printf("\n");
        printf("Path: %s", path); 
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("\n");
        printf("Path Length: %s", pathLengthStr); 
        printf("\n");

        payload = strsep(&running, delimiters);
        remove_coma(payload);
        printf("\n");
        printf("Payload: %s", payload); 
        printf("\n");

        nrBytesStr = strsep(&running, delimiters);
        remove_coma(nrBytesStr);
        //gambiarra
        nrBytesNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(nrBytesStr));
        strcpy(nrBytesNOBUGEDStr, nrBytesStr);
        nrBytes = atoi(nrBytesStr);
        printf("\n");
        printf("\nnrBytes: %s", nrBytesStr); 
        printf("\n");

        offsetStr = strsep(&running, delimiters);
        remove_coma(offsetStr);
        //gambiarra
        offsetNOBUGEDStr = (char*)malloc(sizeof(char)*strlen(offsetStr));
        strcpy(offsetNOBUGEDStr, offsetStr);
        offset = atoi(offsetStr);
        printf("\n");
        printf("Offset: %s", offsetStr); 
        printf("\n");

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");

        ownerPermissionStr = strsep(&running, delimiters);
        remove_coma(ownerPermissionStr);
        printf("\n");
        printf("Owner permission: %s", ownerPermissionStr); 
        printf("\n");

        otherPermissionStr = strsep(&running, delimiters);
        remove_coma(otherPermissionStr);
        printf("\n");
        printf("Other permission: %s", otherPermissionStr); 
        printf("\n");

        parentPath = parentDirectory(path, pathLength);

        root = file_modify(root, clientId, ownerPermissionStr[0], otherPermissionStr[0], path, parentPath, payload, offset, nrBytes);

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        responseBuffer = strcat(responseBuffer, "WR-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, ""); // payload 
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, nrBytesNOBUGEDStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, offsetNOBUGEDStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr);

        // FAZER FLEI DE CRIAR ARQUIVO SE NAO EXISTIR AQUI 

        printf("\n");
        printf("%s", responseBuffer); 
        printf("\n");   
    }

    if( strcmp(command, "FI-REQ") == 0 )
    {
        path =  strsep(&running, delimiters);
        remove_coma(path);

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        
        fileLength = file_info(path);
        sprintf(fileLengthStr, "%d", fileLength);

        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");   

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        owner = get_owner(root, path);
        permission_owner = get_permission(root, path, owner);
        permission_other = get_permission(root, path, owner+1);
        sprintf(ownerStr, "%d", owner);
        sprintf(permissionStr, "%c-%c", permission_owner, permission_other);

        responseBuffer = strcat(responseBuffer, "FI-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, ownerStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, permissionStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, fileLengthStr);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr);
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

        /*
        newPath = strcat(path,dirName);
        printf("SAQUDNS : %s", path);

        newPathLength = strlen(newPath);
        sprintf(newPathLengthStr, "%d", newPathLength);
        */
        printf("Cheguei aqui\n");
        root = directory_create(root, clientId, ownerPermissionStr[0], otherPermissionStr[0], path, dirName);

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        responseBuffer = strcat(responseBuffer, "DC-REP, ");
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr);

        // FAZER FLEI DE CRIAR ARQUIVO SE NAO EXISTIR AQUI 

        printf("\n");
        printf("%s", responseBuffer); 
        printf("\n");  
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
              
        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");
        
        root = directory_delete(root, clientId, path, dirName);

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        responseBuffer = strcat(responseBuffer, "DR-REP, ");
        //verificar se é esse mesmo o path
        responseBuffer = strcat(responseBuffer, path);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, pathLengthStr );
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr);

        // FAZER FLEI DE CRIAR ARQUIVO SE NAO EXISTIR AQUI 

        printf("\n");
        printf("%s", responseBuffer); 
        printf("\n");  
    }

    if( strcmp(command, "DL-REQ") == 0 )
    {   
        //void directory_show_info(char *path)
        path =  strsep(&running, delimiters);
        remove_coma(path);
        printf("\n");
        printf("Path: %s", path); 
        printf("\n");

        pathLengthStr = strsep(&running, delimiters);
        remove_coma(pathLengthStr);
        pathLength = atoi(pathLengthStr);
        printf("\n");
        printf("path length: %s", pathLengthStr); 
        printf("\n");
        
        clientIdStr = strsep(&running, delimiters);
        remove_coma(clientIdStr);
        clientId = atoi(clientIdStr);
        printf("\n");
        printf("cliente: %s", clientIdStr); 
        printf("\n");

        directory_show_info(path);

        responseBuffer = (char*)malloc(sizeof(char)*BUFSIZE);
        strcpy(responseBuffer, "");

        responseBuffer = strcat(responseBuffer, "DL-REP, ");
        responseBuffer = strcat(responseBuffer, directoryInfo);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, directoryPositions);
        responseBuffer = strcat(responseBuffer, ", ");
        responseBuffer = strcat(responseBuffer, clientIdStr); 
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

  // memset(responseBuffer,0,strlen(responseBuffer)); 

  }

}