#include "main.h"

extern int list_pid;
extern char list_cmd[100];

/*Logic for retrieving the last stopped process to continue its execution*/
int get_last_stopped_pid(Slist **head)
{
    //Validating the list
    if(*head == NULL)
    return FAILURE;
    
    /*Logic for single node*/
    if((*head)->link == NULL)
    {
        list_pid = (*head)->c_pid;
        strcpy(list_cmd, (*head)->command);
        return SUCCESS;
    }
    
    //Traversing through the list to get the last stopped process
    Slist *temp = *head;
    while(temp->link)
    {
        temp = temp->link;
    }
    list_pid = temp->c_pid;
    strcpy(list_cmd, temp->command);
    return SUCCESS;
}