#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "lista.h"

Lista * lista_cria(void) {
	return NULL;
}

Lista * lista_add(Lista * lista, char *path, int cliente, char permission_owner, char permission_other){
	Lista* aux = lista;
	Lista* node = (Lista*)malloc(sizeof(Lista));

	(*node).owner = cliente;
	(*node).owner_permission = permission_owner;
	(*node).other_permission = permission_other;
	strcpy((*node).path, path);
	(*node).next = NULL;

	if (aux == NULL)
	{
		lista = node;
		return lista;
	}
	else
	{
		while (aux->next != NULL)
		{
			aux = aux->next;
		}

		aux->next = node;
	}
	return lista;
}

int lista_seek(Lista * lista, char * path)
{
	Lista * temp;
	Lista * aux = lista;

	if (lista == NULL)
	{
		printf("Lista ja esta vazia\n");
		return FALSE;
	}
	else
	{
		while( aux != NULL)
		{
			if(strcmp(aux->path, path) == 0)
			{
				return TRUE;
			}
			aux = aux->next;
		}
		return FALSE;
	}
}

char lista_get_entry_permission(Lista * lista, char * path, int cliente)
{
	{
	Lista * temp;
	Lista * aux = lista;

	if (lista == NULL)
	{
		printf("Nao existe nenhuma entrada no sistema de arquivos\n");
	}
	else
	{
		while( aux != NULL)
		{
			if(strcmp(aux->path, path) == 0)
			{
				if (aux->owner == cliente)
				{
					return aux->owner_permission;
				}
				return aux->other_permission;
			}
			aux = aux->next;
		}
		return FALSE;
	}
}
}

Lista * lista_delete(Lista * lista, char *path) {
	Lista * temp;
	Lista * aux = lista;

	if (lista == NULL)
	{
		printf("Lista ja esta vazia\n");
		return lista;
	}
	else
	{
		if(strcmp(aux->path, path) == 0)
		{
			lista = aux->next;
			free(aux);
			return lista;
		}

		while( aux->next != NULL)
		{
			temp = aux->next;
			if(strcmp(temp->path, path) == 0)
			{
				aux->next = temp->next;
				free(temp);
				return lista;
			}
			aux = aux->next;
		}
		printf("Entrada nao encontrada\n");
		return lista;
	}
}

void lista_print(Lista * lista) {
	Lista * aux = lista;

	if (lista == NULL)
	{
		printf("A lista esta vazia\n");
		return;
	}

	printf("Lista inteira\n");
	while (aux->next != NULL)
	{
		printf("Entrei aqui\n");
		printf(" %d %s -> ", (*aux).owner, (*aux).path);
		aux = aux->next;
	}
	printf("Cheguei\n");
	printf("%d %s\n", (*aux).owner, (*aux).path);
	printf("Passei\n");
}