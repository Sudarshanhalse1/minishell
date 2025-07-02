/* Author       : Sudarshan Halse
   Date         : 04/06/2025
   Description  : The objective is to create our own Minishell to understand and use our application same as Ubuntu and 
                  to understand the system calls w.r.t process creation,signal handling, process synchronization, 
                  exit status.
*/
#include "main.h"

//Declaring the required variables
char *external_commands[155];
int status, pid, list_pid, bg_flag;
char input_string[100], list_cmd[100];
char prompt[50] = "minishell";
Slist *head = NULL;

/*Function definition for signal handler*/
void handler(int signum)
{
    /*Logic for executing the interrupt signal i.e ctrl+c*/
    if(signum == SIGINT)
    {
        if(pid == 0)
        {
          printf(ANSI_COLOR_MAGENTA "\n%s" ANSI_COLOR_RESET"$ " , prompt);
          fflush(stdout);
        }
        else
        {
            printf("\n");
        }
        memset(input_string, '\0', sizeof(input_string));
        return;
    }
    /*Logic for executing the stop signal*/
    else if(signum == SIGTSTP)
    { 
        if(pid != 0)
        {
            if(insert_at_last(&head, input_string, pid) == FAILURE)
            {
                printf("ERROR: Failed to store the command\n");
            }
            memset(input_string, '\0', sizeof(input_string));
            printf("\n");
        }
        else
        {
            printf(ANSI_COLOR_MAGENTA "\n%s" ANSI_COLOR_RESET"$ " , prompt);
            fflush(stdout);
        }
    }
    /*Logic for handling the SIGCHLD while executing bg command*/
    if(signum == SIGCHLD)
    {
        waitpid(-1, &status, WNOHANG);
    }

}

/*Logic for implementing the Minishell*/
int main()
{
    /*Registering all the signals that needs to be handled*/
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCHLD, handler);
    system("clear");
    /*Making a function call to extract the external command*/
    extract_external_commands(external_commands);
    /*Making a function call to read the inputs from the user*/
    scan_input(prompt, input_string);
}

/*Logic for extracting the external commands from the file*/
void extract_external_commands(char **external_commands)
{
    char ch, command[20];
    int fd, i = 0, j = 0;
    
    /*Opening the file that contains all the external commands*/
    fd = open("external_commands.txt", O_RDONLY);
    if(fd == -1)
    {
        perror("");
        exit(1);
    }
    
    /*Reading the contents of the file and storing the commands in an array of strings*/
    while(read(fd, &ch, 1) > 0)
    {
        if(ch != '\n')
        {
            command[i++] = ch;
        }
        else
        {
            command[i] = '\0';
            //Allocating memory dynamicaly to store the command
            external_commands[j] = malloc(sizeof(command));
            strcpy(external_commands[j], command);
            
            j++;
            i = 0;
        }
    }
    command[i] = '\0';
    external_commands[j] = malloc(sizeof(command));
    strcpy(external_commands[j], command);
    j++;
    
    //Storing NULL at the last row to indicate the end
    external_commands[j] = NULL;
    close(fd);
}

/*Logic for reading input from the user and executing those commands */
void scan_input(char *prompt, char *input_string)
{
    while(1)
    {
        //Printing the default prompt
        printf(ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET"$ " , prompt);
        scanf("%[^\n]", input_string);
        __fpurge(stdin);
        
        //Implementing the logic for customized prompt using environmental variable PS1
        if(strncmp("PS1=", input_string, 4) == 0)
        {
            if(input_string[4] != ' ')
            {
              strcpy(prompt, &input_string[4]);
              memset(input_string, '\0', sizeof(input_string));
              continue;
            }
            printf("PS1: Command not found\n");
        }
        else if(!(strncmp("PS1 =", input_string, 5)) || !(strncmp("PS1 = ", input_string, 6)))
        {
            printf("PS1: Command not found\n");
        }

        /*Making a function call to get the command*/
        char *command = get_command(input_string);

        /*Making a function call to check the command i.e whether external or internal*/
        int cmd_type = check_command_type(command);

        /*If it is an external command then creating parent and child process and executing the command in
        the child process making use of fork() system call to achieve this*/
        if(cmd_type == EXTERNAL)
        {
             pid = fork();
             if(pid > 0)
             {
                 waitpid(pid, &status, WUNTRACED);
                 pid = 0;
             }
             else if(pid == 0)
             {
                 //Setting the signals to its default operation in the child process
                 signal(SIGCHLD, SIG_DFL);
                 signal(SIGINT, SIG_DFL);
                 signal(SIGTSTP, SIG_DFL);

                 //Making a function call to execute the external commands
                 execute_external_command(input_string);
                 exit(0);
             }
        }
        /*Making a function call to execute the internal or the built in commands*/
        else if(cmd_type == BUILTIN)
        {
            execute_internal_commands(input_string);
        }
        /*If nothing is entered the prompt should be displayed again*/
        else if(strcmp(input_string, "\0") == 0){}
        /*If user enetered command is not found*/
        else if(cmd_type == NO_COMMAND)
        {
            printf("%s: Command not found\n", input_string);
        }
    }
}

/*Logic for getting the first word of the command from the user entered input*/
char *get_command(char *input_string)
{
    char *command = malloc(sizeof(input_string));
    int i = 0, j = 0;
    while(input_string[i] != ' ' && input_string[i] != '\0')
    {
        command[j] = input_string[i];
        i++;  j++;
    }
    command[j] = '\0';
    return command;
}

/*Logic for determining whether the command is external/internal(built-in)*/
int check_command_type(char *command)
{
    //Declaring and initializing an array of strings to store all the internal commands
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let",
         "eval","set", "unset", "export", "declare", "typeset", "readonly", "getopts","source", 
         "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","fg", "bg", "jobs", NULL};

    int i = 0;
    for(i = 0; builtins[i] != NULL; i++)
    {
        if(strcmp(builtins[i], command) == 0)
        return BUILTIN;
    }
    
    for(i = 0; external_commands[i] != NULL; i++)
    {
        if(strcmp(external_commands[i], command) == 0)
        return EXTERNAL;
    }

    return NO_COMMAND;
}

/*Logic for executing the external command*/
void execute_external_command(char *input_string)
{
     char *argv[20];
     int i = 0;

     //Splitting the command based on space and storing it in an array of strings using strtok()
     char *token = strtok(input_string, " ");
     while(token)
     {
        argv[i] = malloc(sizeof(token));
        strcpy(argv[i], token);
        i++;
        token = strtok(NULL, " ");
     }
     argv[i] = NULL;
     int argc = i;
     int count = 0;

     /*Checking whether there is/are pipe(s) in the input command*/
     for(i = 0; argv[i]!= NULL; i++)
     {
        if(strcmp(argv[i], "|") == 0)
        count++;
     }
     /*If there is no pipe then executing the command directly using execvp() system call*/
     if(!count)
     execvp(argv[0], argv);
    /*Making a function call to execute the command when pipe is present in the command*/
     else
     {
        n_pipes(argc, argv);
     }
}

/*Logic for implementing the execution of command with single/multiple pipes*/
void n_pipes(int argc, char *argv[])
{
    //Declaring the required variables
    int i, ppid, count = 0, st;
    int *arr = malloc(sizeof(int));

    /*Storing the positions of the commands  before & after the pipe*/
    arr[0] = 0;
    for(i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "|") == 0)
        {
            count++;
            arr =(int *) realloc(arr,(count+1) * sizeof(int));
            arr[count] = i + 1;
            //Making the pipe position as NULL
            argv[i] = NULL;
        }
    }
    
    /*Declaring a 2D array based on number of pipes to create the pipe*/
    int fd[count][2];
    /*Duplicating the stdout and stdin standard streams for later redirection*/
    int stdout_fd = dup(1);
    int stdin_fd = dup(0);

    /*Logic for executng the commands with single/multiple pipes*/
    for(i = 0; i < count+1; i++)
    {
        //Creating the pipes based on its count
        if(i < count)
        {
            pipe(fd[i]);
        }
        //Creating the parent and child process to execute the commads
        ppid = fork();
        if(ppid > 0 && i < count)
        {
                 close(fd[i][1]);
                 dup2(fd[i][0], 0);
                 close(fd[i][0]);
        }
        else if(ppid == 0)
        {
            if(i < count)
            {
                close(fd[i][0]);
                dup2(fd[i][1], 1);
            }
            execvp(argv[arr[i]], argv + arr[i]);
            exit(0);
        }

    }
    
    //Waiting till all the commands gets executed
    for( i = 0; i < count + 1; i++)
    {
        wait(NULL);
    }
    //Redirecting the file descriptors 1 & 0 to the standard streams i.e stdin & stdout.
    dup2(stdout_fd, 1);
    dup2(stdin_fd, 0);
    //Freeing the dynamically allocated memory after its usage
    free(arr);
}

/*Logic for execcuting the internal commands, background processes and special commands*/
void execute_internal_commands(char *input_string)
{
    char buffer[100];

    //Logic for executing the "exit" command
    if(strcmp(input_string, "exit") == 0)
    {
        exit(0);
    }
    //Logic for executing the "pwd" command using getcwd() 
    else if(strcmp(input_string, "pwd") == 0)
    {
        printf("%s\n", getcwd(buffer, 100));
    }
    //Logic for executing the "cd" command using chdir()
    else if(strstr(input_string, "cd"))
    {
        char *path = strchr(input_string, ' ');
        if(path != NULL)
        {
            path++;

            while(*path == ' ')
            path++;
            
            if(*path == '\0')
            return;
            
            int ret = chdir(path);
            if(ret == -1)
            printf("ERROR: Invalid path\n");
            else
            printf("INFO: Directory changed to %s\n", path);
        }
        else
        {
            return;
        }
    }
    /*Logic for executing the echo commands*/
    else if(strstr(input_string, "echo"))
    {
        if(strcmp(input_string, "echo $?") == 0)
        {
            printf("%d\n", status);
        }
        else if(strcmp(input_string, "echo $$") == 0)
        {
            printf("%d\n", getpid());
        }
        else if(strcmp(input_string, "echo $SHELL") == 0)
        {
            printf("%s\n", getenv("SHELL"));
        }
    }
    /*Logic for implementing the "fg" command*/
    else if(strcmp(input_string, "fg") == 0)
    {
        if(get_last_stopped_pid(&head) == SUCCESS)
        {  
            pid = list_pid;
            /*Storing the stopped commands into a single linked list for future use*/
            strcpy(input_string, list_cmd);
            //Using kill() system call to continue the process
            kill(list_pid, SIGCONT);
            printf("%s\n", list_cmd);
           
            int ret_pid = waitpid(list_pid, &status, WUNTRACED);
            pid = 0;
            if(ret_pid == list_pid && ret_pid != -1){}
            if(remove_process(&head, list_pid) == SUCCESS){}
        }
        else
        {
            printf("-bash: fg: current: no such job\n");
        }
    }
    /*Logic for implementing the "bg" command*/
    else if(strcmp(input_string, "bg") == 0)
    {
        /*Continuing the stopped process in the background using kill() system call*/
        if(get_last_stopped_pid(&head) == SUCCESS)
        {
            kill(list_pid, SIGCONT);
            printf("%s &\n", list_cmd);
            fflush(stdout);
            remove_process(&head, list_pid);
        }
        else
        {
            printf("-bash: bg: current: no such job\n");
        }
    }
    /*Printing all the stopped processes in the single linked list to execute the jobs command*/
    else if(strcmp(input_string, "jobs") == 0)
    {
        print_list(head, 2);
    }
}

 