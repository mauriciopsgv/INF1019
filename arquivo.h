struct node {
	int owner;
	char owner_permission;
	char other_permission;
	char * name;
	struct node * next;
};

typedef struct node Entry;

Entry * create_node ()
{
	node = (Entry *) malloc(sizeof(Entry));
	node->num_children = 0;
	node->next = entry;
	return node;
}

void add_entry(Entry *entry, char * name)
{
	Entry * newEntry = create_node();
	newEntry->name = name;

	entry->next[entry->num_children] = newEntry;
	entry->num_children++;
}

