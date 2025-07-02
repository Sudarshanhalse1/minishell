#ifndef SLL_H
#define SLL_H

#define SUCCESS 0
#define FAILURE -1

//Defining a node of the single linked list
typedef struct node
{
    char command[100];
    int c_pid;
    struct node *link;
}Slist;

int insert_at_last(Slist **head, char *command, int c_pid);

void print_list(Slist *head, int flag);

int get_last_stopped_pid(Slist **head);
int remove_process(Slist **head, int c_pid);

#endif
