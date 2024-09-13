#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 256
#define MAX_LINES 1000

void sanitize_input(char *input)
{
    char sanitized[MAX_LEN];
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (strchr(";&|><*?()$", input[i]) == NULL)
        {
            sanitized[j++] = input[i];
        }
    }
    sanitized[j] = '\0';
    strcpy(input, sanitized);
}

int main(int argc, char *argv[])
{
    int option, i;
    char temp[MAX_LEN];
    char command[MAX_LEN];
    char *args[MAX_LEN];
    int n, t, r, rep = 0;
    int perLine = 0;

    // args[0] = argv[1];

    while ((option = getopt(argc, argv, "n:I:tr")) != -1)
    {
        switch (option)
        {
        case 'n':
            printf("%c was passed\n", argv[1][1]);
            n = 1;
            perLine = atoi(argv[2]);
            // argv[2] gets the number associated with -n
            // argc = num of arguments passed
            break;
        case 'I':
            // strcpy(temp, argv[2]); // stores whatever string i pass in for -I in temp
            rep = 1;
            break;
        case 't':
            t = 1;
            // i = 0;
            //  printf("+ ");
            //  printf("%s ", args[i]);
            //  printf("\n");

            break;
        case 'r':
            printf("%c was passed", argv[1][1]);
            r = 1;
            break;
        default:
            printf("Usage: myxargs [-n num] [-I replace] [-t] [-r] command");
            break;
        }
    }

    // Handle remaining arguments after options
    if (optind < argc)
    {
        // Fork a new process
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            execvp(argv[optind], &argv[optind]);
            // If execvp fails
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }

    char *lines[MAX_LINES];
    int numArgs;
    for (int l = 0; l < MAX_LINES; l++)
    {
        lines[i] = NULL;
    }

    while (fgets(command, sizeof(command), stdin))
    {
        command[strcspn(command, "\n")] = 0;

        i = 0;
        char *element = strtok(command, " ");
        while (element != NULL && i < MAX_LEN - 1)
        {
            args[i++] = element;
            element = strtok(NULL, " ");
        }
        numArgs = i;
        args[i] = NULL;

        if (n)
        {
            if (perLine >= MAX_LINES)
            {
                fprintf(stderr, "Too many lines\n");
                exit(EXIT_FAILURE);
            }
            for (int start = 0; start < numArgs; start += perLine)
            {
                for (int j = start; j < start + perLine && j < numArgs; j++)
                {
                    printf("%s ", args[j]);
                }
                printf("\n");
            }
        }
        // __pid_t pid = fork();

        // if (pid < 0)
        // {
        //     perror("fork failed\n");
        //     exit(1);
        // }
        // else if (pid == 0)
        // {
        //     // child process
        //     // execvp(args[0], args);
        //     perror("execvp failed");
        //     exit(1);
        // }
        // else
        // {
        //     // parent process;
        //     wait(NULL);
        // }
    }

    return 0;
}