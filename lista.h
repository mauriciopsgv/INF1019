#include <sys/types.h>
#define TRUE 1
#define FALSE 0	
#define BUFSIZE 1024

typedef struct node {
	int owner;
	char owner_permission;
	char other_permission;
	char path[BUFSIZE];
	struct node * next;
} Lista;

Lista * lista_cria(void);

Lista * lista_add(Lista * lista, char *path, int cliente, char permission_owner, char permission_other);

int lista_seek(Lista * lista, char *path);

Lista * lista_delete(Lista * lista, char *path);

int lista_get_entry_client(Lista * lista, char * path);

char lista_get_entry_permission(Lista * lista, char * path, int cliente);

void lista_print(Lista * lista);