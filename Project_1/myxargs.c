#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

#define MAX_LEN 1000   // Max string length
#define MAX_LINES 1000 // Max num of lines for -n flag

void sanitize_input(char *input)
{
    char sanitized[MAX_LEN];
    int j = 0;
    // Iterate over each character in the input

    for (int i = 0; input[i] != '\0'; i++)
    {
        // Check if the character is in the blacklist
        if (strchr(";&|><*?()$", input[i]) == NULL)
        {
            // Only add to sanitized if there is enough space and doesn't contain illegal character
            if (j < MAX_LEN - 1)
            {
                sanitized[j++] = input[i];
            }
            else
            {
                break; // Prevent buffer overflow
            }
        }
    }
    sanitized[j] = '\0';                    // Null terminate sanitized
    strncpy(input, sanitized, MAX_LEN - 1); // Copy sanitized to input param
    input[MAX_LEN - 1] = '\0';              // Null terminate input
}

void execute_command(char *command)
{
    // Split command into arguments
    char *args[MAX_LEN];
    int i = 0;

    // Tokenize command
    char *token = strtok(command, " ");
    while (token != NULL && i < MAX_LEN - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the argument list

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
        execvp(args[0], args); // Execute command at args[0]

        // If exec fails
        perror("exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        wait(NULL); // Wait until child process is finished
    }
}

int main(int argc, char *argv[])
{
    // Variable declarations
    int option, i;
    char fullCommand[MAX_LEN] = "";
    char placeholder[MAX_LEN] = "";
    char command[MAX_LEN];
    char *args[MAX_LEN];
    char stringArgs[MAX_LEN] = "";
    int numArgs;
    int n, t, r, rep, perLine = 0;

    while ((option = getopt(argc, argv, "n:I:tr")) != -1) // Getting the options passed in
    {
        switch (option)
        {
        case 'n': // n flag
            n = 1;
            perLine = atoi(optarg); // Integer argument for n flag
            break;
        case 'I': // I flag
            rep = 1;
            strcpy(placeholder, optarg); // stores whatever string i pass in for -I in placeholder
            break;
        case 't': // t flag
            t = 1;
            break;
        case 'r': // r flag
            r = 1;
            break;
        default: // default case
            fprintf(stderr, "Usage: %s [-n num] [-I replace] [-t] [-r] command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) // if no command is passed after flags
    {
        fprintf(stderr, "Usage: %s [-n num] [-I replace] [-t] [-r] command\n", argv[0]); // usage statement
        exit(EXIT_FAILURE);
    }

    while (fgets(command, sizeof(command), stdin))
    {
        command[strcspn(command, "\n")] = 0;
        strcpy(stringArgs, command);
        sanitize_input(stringArgs);

        i = 0; // index variale for tokenizing args

        int j = 0;
        command[sizeof(command) - 1] = '\0';

        char *element = strtok(command, " "); // variable to store command elements, split on spaces
        while (element != NULL && i < MAX_LEN - 1)
        {
            args[i++] = element; // store element in args
            element = strtok(NULL, " ");
        }
        numArgs = i;    // since i was incremented on spaces, it's also the num of arguments
        args[i] = NULL; // null terminate

        if (r && command[0] == '\0') // if r is active
        {
            // No command execution if input is empty
            exit(EXIT_SUCCESS);
        }

        if (n) // if n is active
        {
            int end;
            if (perLine <= 0 || perLine > MAX_LINES) // checking to make sure the argument passed to -n works
            {
                fprintf(stderr, "Invalid -n value\n");
                exit(EXIT_FAILURE);
            }

            // Split args into chunks of perLine size
            for (int i = 0; i < numArgs; i += perLine)
            {
                char *chunks[perLine + 1];
                if (i + perLine < numArgs) // determines end of index
                {
                    end = i + perLine;
                }
                else
                {
                    end = numArgs;
                }
                int k = 0;

                for (int j = i; j < end; j++) // populate chunks array
                {
                    chunks[k++] = args[j];
                }
                chunks[k] = NULL; // null terminate

                char command[MAX_LEN];
                snprintf(command, sizeof(command), "%s", argv[optind]); // initializes command with base command (argv[optind])

                for (int j = 0; j < k; j++) // iterate over chunk array
                {
                    snprintf(command + strlen(command), sizeof(command) - strlen(command), " %s", chunks[j]); // append each argument form chunks to command
                }
                execute_command(command); // execute the command
            }
        }
        if (rep)
        {
            char *pos;
            char fullRight[MAX_LEN] = ""; // initialize buffer for arguments

            // concatenate arguments into fullRight
            for (int i = optind; i < argc; i++)
            {
                strcat(fullRight, argv[i]);
                if (i < argc - 1)
                {
                    strcat(fullRight, " ");
                }
            }

            // replace placeholders in fullRight
            while ((pos = strstr(fullRight, placeholder)) != NULL)
            {
                int beforeLen = pos - fullRight;          // calculate length of string in fullRight before the placeholder
                int placeholderLen = strlen(placeholder); // length of placeholder var

                char newCommand[MAX_LEN];
                strncpy(newCommand, fullRight, beforeLen);
                newCommand[beforeLen] = '\0';
                strcat(newCommand, stringArgs); // replace placeholder with command input
                strcat(newCommand, pos + placeholderLen);

                strncpy(fullRight, newCommand, MAX_LEN - 1);
                fullRight[MAX_LEN - 1] = '\0';
            }

            sanitize_input(fullRight); // sanitize

            if (t) // if t is flagged with I
            {
                printf("+ %s\n", fullRight); // print command
            }

            execute_command(fullRight); // execute
        }
        else
        {

            if (t) // if t is active
            {
                char fullRight[MAX_LEN] = "";       // gets right side of pipe symbol
                for (int i = optind; i < argc; i++) // iterates over argv starting at optind
                {
                    strcat(fullRight, argv[i]); // concatenate fullRight into argv[i]
                    if (i < argc - 1)
                    {
                        strcat(fullRight, " "); // add spaces
                    }
                }
                if (args[0] == NULL)
                {
                    sanitize_input(fullRight);
                    printf("+ %s\n", fullRight);
                    break;
                }
                else
                {
                    sanitize_input(fullRight);
                    printf("+ %s %s \n", fullRight, stringArgs);
                    execute_command(fullRight);
                }
            }
        }
    }

    return 0;
}
