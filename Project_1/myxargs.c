#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 256

// typedef struct
// {
//     char val[MAX_LEN];
//     struct Node *next;
// } Node;

// typedef struct
// {
//     Node *head;
//     Node *tail;
// } Queue;

// void init(Queue *q)
// {
//     q->head = q->tail = NULL;
// }

// Node *peek(Queue *q)
// {
//     if (q == NULL)
//     {
//         printf("Q is null");
//         return NULL;
//     }
//     if (q->head != NULL)
//     {
//         printf("Value at head: %s\n", q->head->val);
//         return q->head;
//     }
//     printf("Queue is empty");
//     return q->head;
// }

// Node *printQueue(Queue *q)
// {
//     Node *current = q->head;
//     if (current == NULL)
//     {
//         printf("Queue is empty\n");
//     }
//     while (current != NULL)
//     {
//         printf("%s", current->val);
//         current = current->next;
//     }
//     return q->head;
// }

// void enqueue(Queue *q, char val[MAX_LEN])
// {
//     Node *newNode = (Node *)malloc(sizeof(Node));

//     if (newNode == NULL)
//     {
//         perror("Error allocating memory for new node");
//         exit(EXIT_FAILURE);
//     }

//     strncpy(newNode->val, val, MAX_LEN - 1);
//     if (q->tail == NULL)
//     {
//         q->head = q->tail = newNode;
//     }
//     else
//     {
//         q->tail->next = newNode;
//         q->tail = newNode;
//     }
// }

int main(int argc, char *argv[])
{
    int option, i;
    char temp[MAX_LEN];
    char command[MAX_LEN];
    char *args[MAX_LEN];

    // args[0] = argv[1];

    if (fgets(command, sizeof(command), stdin))
    {
        command[strcspn(command, "\n")] = 0;

        i = 0;
        char *element = strtok(command, " ");
        while (element != NULL && i < MAX_LEN - 1)
        {
            args[i++] = element;
            element = strtok(NULL, " ");
        }
        args[i] = NULL;
        // printf("Command read: %s\n", command);
        for (int j = 0; args[j] != NULL; j++)
        {
            printf("args[%d]: %s\n", j, args[j]);
        }
        __pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed\n");
            exit(1);
        }
        else if (pid == 0)
        {
            // child process
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        }
        else
        {
            // parent process;
            wait(NULL);
        }
    }

    while ((option = getopt(argc, argv, "n:I:tr")) != -1)
    {
        switch (option)
        {
        case 'n':
            printf("%c was passed\n", argv[1][1]);
            // argv[2] gets the number associated with -n
            // argc = num of arguments passed
            break;
        case 'I':
            strcpy(temp, argv[2]); // stores whatever string i pass in for -I in temp
            break;
        case 't':
            i = 0;
            printf("+ ");
            printf("%s ", args[i]);
            printf("\n");

            break;
        case 'r':
            printf("%c was passed", argv[1][1]);
            break;
        default:
            printf("Usage: myxargs [-n num] [-I replace] [-t] [-r] command");
            break;
        }
    }
    return 0;
}