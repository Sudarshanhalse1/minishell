#include "main.h"

/*Logic for removing stopped process from the single linked list after its conituation using fg/bg command */
int remove_process(Slist **head, int c_pid)
{
     //Validating whether the list is empty or not
     if(*head == NULL)
     {
         return FAILURE;
     }
     
     //Creating the pointers to traverse through the list
     Slist *temp, *prev;
     temp = prev = *head;
     
     //Condition if the first node is to be deleted
     if((*head)->c_pid == c_pid)
     {
         *head = temp->link;
         free(temp);
         return SUCCESS;
     }
 
     //Traversing through the list and checking whether the given data is present
     //If the given data is present then freeing that node
     //After freeing establishing the link between the previous node and the next node
     while(temp->link != NULL)
     {
         prev = temp;
         temp = temp -> link;
         if(temp->c_pid == c_pid)
         {
             prev->link = temp->link;
             free(temp);
         }
     }
     return SUCCESS;
}