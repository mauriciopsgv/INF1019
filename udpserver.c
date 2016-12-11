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
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>

#define BUFSIZE 1024
#define TRUE 1
#define FALSE 0
extern  int alphasort(); 

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

void directory_create(char *path, char *dirName)
{   
    char fullPath[BUFSIZE];
    mode_t permissao = S_IRWXU | S_IROTH | S_IWOTH | S_IXOTH;

    if (getwd(fullPath) == NULL )
    {
      printf("Error getting path\n"); exit(0);
    }
    printf("Current Working Directory = %s\n",fullPath);

    strcat(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, dirName);

    mkdir(fullPath, permissao);
}

void directory_delete(char *path, char *dirName)
{   
    char fullPath[BUFSIZE];

    if (getwd(fullPath) == NULL )
    {
      printf("Error getting path\n"); exit(0);
    }
    printf("Current Working Directory = %s\n",fullPath);

    strcat(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, dirName);

    rmdir(fullPath);
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

void directory_show_info(char *path, int pathSize)
{   
    char pathname[BUFSIZE];
    int count,i; 
    struct direct **files; 

    if (getwd(pathname) == NULL ) 
    { 
       printf("Error getting path\n"); exit(0); 
    }

    printf("Current Working Directory = %s\n",pathname); 

    count = scandir( pathname, &files, select_all_files, alphasort); 

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

void parse_buff (char *buff, int *cmd, char *name)
{
    const char delimiters[] = " ";
    char *running;


    char *command;

    running = strdup(buff);

    command = strsep(&running, delimiters);
    remove_coma(command);
    printf("Command: %s \n", command);


    //MANIPULACAO DE ARQUIVOS
    if( strcmp(command, "RD-REQ") == 0 )
    {
        printf("oi");
    }

    if( strcmp(command, "RD-REP") == 0)
    {
        printf("oi");
    }

    if( strcmp(command, "WR-REQ") == 0 )
    {
        printf("oi");
    }

    if( strcmp(command, "WR-REP") == 0 )
    {
        printf("oi");
    }

    if( strcmp(command, "FI-REQ") == 0 )
    {
        printf("oi");
    }

    if( strcmp(command, "FI-REQ") == 0 )
    {
        printf("oi");
    }

    //MANIPULACAO DE DIRETORIOS
    if( strcmp(command, "DC-REQ") == 0 )
    {
        //create_directory();
    }

    if( strcmp(command, "DC-REP") == 0 )
    {
        printf("TO-DO");
    }

    if( strcmp(command, "DR-REQ") == 0 )
    {
        //delete_directory();
    }

    if( strcmp(command, "DR-REP") == 0 )
    {
        printf("TO-DO");
    }

    if( strcmp(command, "DL-REQ") == 0 )
    {
        printf("TO-DO");
    }

    if( strcmp(command, "DL-REQ") == 0 )
    {
        printf("TO-DO");
    }

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

  // while (1)
  // {

  //   /*
  //    * recvfrom: receive a UDP datagram from a client
  //    */
  //   bzero(buff, BUFSIZE);
  //   n = recvfrom(sockfd, buff, BUFSIZE, 0,
		//  (struct sockaddr *) &clientaddr, &clientlen);
  //   if (n < 0)
  //     error("ERROR in recvfrom");

  //   parse_buff(buff, &cmd, name);



  //   /*
  //    * gethostbyaddr: determine who sent the datagram
  //    */
  //   hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
  //   if (hostp == NULL)
  //     error("ERROR on gethostbyaddr");
  //   hostaddrp = inet_ntoa(clientaddr.sin_addr);
  //   if (hostaddrp == NULL)
  //     error("ERROR on inet_ntoa\n");
  //   printf("server received datagram from %s (%s)\n",
	 //   hostp->h_name, hostaddrp);
  //   printf("server received %d/%d bytes: %s\n", strlen(buff), n, buff);


     // directory_create("/", "dirToBeDeleted");
     directory_delete("/dirTeste", "dirToBeDeleted");

    /*
     * sendto: echo the input back to the client
     */
  //   n = sendto(sockfd, buff, strlen(buff), 0,
	 //       (struct sockaddr *) &clientaddr, clientlen);
  //   if (n < 0)
  //     error("ERROR in sendto");
  // }
}