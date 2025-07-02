#include "main.h"

/*Logic for printing the single linked list*/
void print_list(Slist *head, int flag)
{
    if(head == NULL)
    return;
    
    Slist *temp = head;
    if(flag == 1)
    {
         while(temp)
        {
          printf("%s %d\n", temp->command, temp->c_pid);
          temp = temp->link;
        }
    }
    if(flag == 2)
    {
        while(temp)
        {
          printf("Stopped %s\n", temp->command);
          temp = temp->link;
        } 
    }
}