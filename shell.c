#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define HISTORY_SIZE 10
#define PATH_COUNT 100

char *history[HISTORY_SIZE];
int history_count = 0; // Number of stored commands

char *path[PATH_COUNT];
int path_count = 0;

int intMode(char *args[]);
void errorMsg();
int builtins(char *args[]);
int shellLoop();
int cleanInput(char *input);
int parseInput(char *input);
int printHistory(char *input);
int changeDir(char *args[]);
int countArgs(char *args[]);
int printDir();
int setPath(char *args[]);
int initializePath();
int execCommand(char *args[]);
int processChange(char *args[], int k);
int sendInput(char *args[], int k);
int parallelProcess(char *args[], int);
int batchMode(char *argv[]);
int redirectOut(char *args[], int k);
int redirectIn(char *args[], int k);
int pipes(char *args[], int k);
int runHistory(char *args);
int killProcess(char *args[]);

void errorMsg()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char *argv[])
{
    initializePath();

    if (argc == 1)
    {
        // printf("Interactive Mode\n");
        shellLoop();
    }

    else if (argc == 2)
    {
        // printf("Batch Mode\n");
        batchMode(argv);
    }

    else
    {
        errorMsg();
        exit(1);
    }

    return 0;
}

int intMode(char *args[])
{
    // printf("DEBUG: %s %s", args[0], args[1]);
    if (args == NULL || args[0] == NULL)
    {
        errorMsg();
        return 1;
    }

    if (!strcmp(args[0], "exit") || !strcmp(args[0], "cd") || !strcmp(args[0], "kill") ||
        !strcmp(args[0], "history") || !strcmp(args[0], "pwd") || !strcmp(args[0], "path"))
    {
        //  printf("BUILTIN MODE\n");
        builtins(args);
    }
    else
    {
        // printf("EXEC MODE\n");
        execCommand(args);
        //  for (int i = 0; i < 6 && args[i] != NULL; i++)
        // printf("%s", args[i]);
    }
    return 0;
}

int builtins(char *args[])
{
    if (!strcmp(args[0], "exit"))
    {
        exit(0);
    }
    else if (!strcmp(args[0], "cd"))
    {
        // printf("CD");
        changeDir(args);
    }
    else if (!strcmp(args[0], "kill"))
    {
        killProcess(args);
    }

    else if (!strcmp(args[0], "pwd"))
    {
        printDir();
    }
    else if (!strcmp(args[0], "path"))
    {
        setPath(args);
    }

    return 0;
}

int shellLoop()
{
    size_t buff = 256;
    char *input = NULL;

    while (true)
    {
        printf("$> ");
        getline(&input, &buff, stdin);

        if (strcmp(input, "\n") == 0)
        {
            free(input);
            input = NULL;
            continue;
        }

        if (strcmp(input, "exit\n") == 0)
        {
            free(input);
            break;
        }

        cleanInput(input);
        free(input);
        input = NULL;
    }
    return 0;
}

int cleanInput(char *input)
{
    char *originalInput = strdup(input);
    while (*input == ' ' | *input == '\t')
    {
        input++;
    }
    if (input[strlen(input) - 1] == '\n')
    {
        input[strlen(input) - 1] = '\0';
        originalInput[strlen(originalInput) - 1] = '\0';
    }
    //  printf("%s\n", input);
    parseInput(input);
    printHistory(originalInput);
    return 0;
}

int parseInput(char *input)
{
    int i = 0;
    char *args[100];
    int len = strlen(input);
    int pos = 0;

    while (pos < len)
    {
        // Skip any leading whitespace
        while (pos < len && (input[pos] == ' ' || input[pos] == '\t'))
            pos++;
        if (pos >= len)
            break;

        // If the token starts with a quote, handle it specially
        if (input[pos] == '\"' || input[pos] == '\'')
        {
            char quote = input[pos];
            pos++; // Skip the opening quote
            int start = pos;
            // Find the closing matching quote
            while (pos < len && input[pos] != quote)
                pos++;
            int token_len = pos - start;
            char *token = malloc(token_len + 1);
            if (!token)
            {
                errorMsg();
                exit(1);
            }
            strncpy(token, input + start, token_len);
            token[token_len] = '\0';
            args[i++] = token;
            if (pos < len)
            {
                pos++; // Skip the closing quote
            }
        }
        else
        {
            // Normal token: read until next whitespace
            int start = pos;
            while (pos < len && input[pos] != ' ' && input[pos] != '\t')
                pos++;
            int token_len = pos - start;
            char *token = malloc(token_len + 1);
            if (!token)
            {
                errorMsg();
                exit(1);
            }
            strncpy(token, input + start, token_len);
            token[token_len] = '\0';
            args[i++] = token;
        }
    }

    args[i] = NULL;
    // INPUT, SENDING TO SEND INPUT");
    sendInput(args, i);

    // Free allocated tokens
    for (int j = 0; j < i; j++)
        free(args[j]);

    return 0;
}

int printHistory(char *input)
{
    // printf("INPUT IN HISTORY %s", input);
    if (!strcmp(input, "history"))
    {
        for (int i = 0; i < history_count; i++)
        {
            printf("%d %s\n", i + 1, history[i]);
        }
    }
    else if (input[0] == '!')
        return 0;
    else
    {
        history[history_count % HISTORY_SIZE] = strdup(input);
        if (history_count < HISTORY_SIZE)
        {
            history_count++;
        }
    }
    return 0;
}

int changeDir(char *args[])
{
    // char s[100];

    if (countArgs(args) > 2)
        errorMsg();
    //  printf("%s\n", getcwd(s, 100));
    int cDir = chdir(args[1]);
    if (cDir != 0)
        errorMsg();

    // printf("%s\n", getcwd(s, 100));
    return 0;
}

int countArgs(char *args[])
{
    int count = 0;
    while (args[count] != NULL)
        count++;
    return count;
}

int printDir()
{
    char s[100];
    printf("%s\n", getcwd(s, 100));
    return 0;
}

           int
           setPath(char *args[])
{
    // printf("%s\n", path[0]);
    memset(path, 0, sizeof(path));
    path_count = 0;

    if (args[1] == NULL)
        return 0;
    int i = 0;
    while (args[i + 1] != NULL && i < PATH_COUNT)
    {
        path[i] = strdup(args[i + 1]); // Copy new paths
        i++;
    }
    path_count = i;

    return 0;
}

int initializePath()
{
    path[0] = strdup("/bin");
    path_count += 1;
    return 0;
}

int execCommand(char *args[])
{
    char *command = args[0];
    char fullPath[256];

    if (command[0] == '/')
    {
        int forkId = fork();
        if (forkId == 0)
        {
            execve(command, args, NULL);
            errorMsg();
            exit(1);
        }
        wait(NULL);
        return 0;
    }

    // Try each path
    for (int i = 0; i < path_count; i++)
    {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path[i], args[0]);
        if (access(fullPath, X_OK) == 0)
        {
            int forkId2 = fork();
            if (forkId2 == 0)
            {
                execve(fullPath, args, NULL);
                errorMsg();
                exit(1);
            }
            wait(NULL);
            return 0; // Return only after successfully executing
        }
    }

    // If we get here, command wasn't found in any path
    errorMsg();
    return 1;
}

int sendInput(char *args[], int k)
{
    // printf("IN SEND INPUT\n");
    char *command = args[0];
    int inRedirect = 0;
    int outRedirect = 0;
    int redirectCount = 0;
    int pipeCount = 0;
    int processCount = 0;
    bool isHistory = false;
    int inIndex = -1;
    int outIndex = -1;

    // Count redirections and find their positions
    for (int i = 0; i < k; i++)
    {
        if (!strcmp(args[i], "<"))
        {
            inRedirect++;
            inIndex = i;
        }
        else if (!strcmp(args[i], ">"))
        {
            outRedirect++;
            outIndex = i;
        }
        else if (!strcmp(args[i], "|"))
        {
            pipeCount++;
        }
        else if (!strcmp(args[i], "&"))
        {
            processCount++;
        }
    }

    redirectCount = inRedirect + outRedirect;

    if (command[0] == '!')
    {
        //  printf("DEBUG: %c", command[0]);
        runHistory(command);
        isHistory = true;
    }

    if (command[0] == '.')
    {
        processCount += 1;
    }

    // Validation checks
    if (redirectCount > 2 || inRedirect > 1 || outRedirect > 1)
    {
        errorMsg();
        return 1;
    }

    if (pipeCount > 4)
    {
        errorMsg();
        return 1;
    }

    if (processCount > 10)
    {
        errorMsg();
        return 1;
    }

    // Handle redirections
    if (redirectCount > 0)
    {
        //  printf("REDIRECT(S) DETECTED\n");

        // Handle both input and output redirections
        if (inRedirect > 0 && outRedirect > 0)
        {
            //   printf("BOTH REDIRECTIONS\n");

            // Save file descriptors
            int stdinCopy = dup(STDIN_FILENO);
            int stdoutCopy = dup(STDOUT_FILENO);

            // Setup input redirection
            int inFile = open(args[inIndex + 1], O_RDONLY);
            if (inFile == -1)
            {
                errorMsg();
                close(stdinCopy);
                close(stdoutCopy);
                return 1;
            }

            if (dup2(inFile, STDIN_FILENO) == -1)
            {
                errorMsg();
                close(inFile);
                close(stdinCopy);
                close(stdoutCopy);
                return 1;
            }
            close(inFile);

            // Setup output redirection
            int outFile = open(args[outIndex + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (outFile == -1)
            {
                errorMsg();
                dup2(stdinCopy, STDIN_FILENO); // Restore stdin
                close(stdinCopy);
                close(stdoutCopy);
                return 1;
            }

            if (dup2(outFile, STDOUT_FILENO) == -1)
            {
                errorMsg();
                close(outFile);
                dup2(stdinCopy, STDIN_FILENO); // Restore stdin
                close(stdinCopy);
                close(stdoutCopy);
                return 1;
            }
            close(outFile);

            char *cmdArgs[100];
            int cmdIndex = 0;

            for (int i = 0; i < k; i++)
            {
                if (i == inIndex || i == inIndex + 1 || i == outIndex || i == outIndex + 1)
                    continue;
                cmdArgs[cmdIndex++] = args[i];
            }
            cmdArgs[cmdIndex] = NULL;

            intMode(cmdArgs);

            // Restore file descriptors
            dup2(stdinCopy, STDIN_FILENO);
            dup2(stdoutCopy, STDOUT_FILENO);
            close(stdinCopy);
            close(stdoutCopy);

            return 0;
        }
        else if (inRedirect > 0)
        {
            //   printf("INPUT REDIRECTION\n");
            return redirectIn(args, inIndex);
        }
        else // outRedirect > 0
        {
            //   printf("OUTPUT REDIRECTION\n");
            return redirectOut(args, outIndex);
        }
    }
    else if (pipeCount > 0)
    {
        //   printf("PIPES\n");
        return pipes(args, k);
    }
    else if (processCount > 0)
    {
        //  printf("PROCESS\n");
        return parallelProcess(args, k);
    }
    else if (!isHistory)
    {
        return intMode(args);
    }

    return 0;
}

int parallelProcess(char *args[], int k)
{
    int start = 0;
    for (int i = 0; i <= k; i++)
    {
        if (i == k || strcmp(args[i], "&") == 0)
        {

            args[i] = NULL;

            int forkID = fork();

            if (forkID == 0)
            {
                execvp(args[start], &args[start]);
                errorMsg();
                exit(1);
            }
            else if (forkID < 0)
            {
                errorMsg();
            }

            if (i == k)
            {
                wait(NULL);
            }
            else
            {
                printf("[Background PID] %d\n", forkID);
            }

            start = i + 1;
        }
    }

    return 0;
}

int pipes(char *args[], int k)
{
    int prev_pipe = -1;
    int cmd_start = 0;

    for (int i = 0; i <= k; i++)
    {
        int pipefd[2];
        int has_next = (i < k && !strcmp(args[i], "|"));

        if (args[i] == NULL || has_next)
        {
            args[i] = NULL;

            if (has_next)
            {
                if (pipe(pipefd) == -1)
                {
                    errorMsg();
                    return 1;
                }
            }

            int pid = fork();
            if (pid == 0) // Child process
            {
                if (prev_pipe != -1)
                {
                    dup2(prev_pipe, STDIN_FILENO);
                    close(prev_pipe);
                }
                if (has_next)
                {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                    close(pipefd[0]); // Child doesn't need read end
                }

                execvp(args[cmd_start], &args[cmd_start]); // Use execvp instead
                exit(1);
            }
            else // Parent process
            {
                if (prev_pipe != -1)
                    close(prev_pipe);

                if (has_next)
                {
                    close(pipefd[1]);
                    prev_pipe = pipefd[0];
                }
                else
                {
                    prev_pipe = -1;
                }
            }

            cmd_start = i + 1;
        }
    }

    // Wait for all child processes
    while (wait(NULL) > 0)
        ;
    return 0;
}

int batchMode(char *argv[])
{
    FILE *batchFile = fopen(argv[1], "r");
    if (batchFile == NULL)
    {
        errorMsg();
        exit(1);
    }

    size_t buff = 256;
    char *input = malloc(buff);
    while (fgets(input, buff, batchFile) != NULL)
    {
        //  printf("DEBUG: Executing command -> %s", input);
        cleanInput(input);
    }

    free(input);
    fclose(batchFile);
    return 0;
}

int redirectIn(char *args[], int k)
{
    int file;
    char *arr[50];
    int stdinCopy = dup(STDIN_FILENO);

    file = open(args[k + 1], O_RDONLY);

    if (file == -1)
        errorMsg();

    int file2 = dup2(file, STDIN_FILENO);

    if (file2 == -1)
        errorMsg();

    int j = 0;
    for (int i = 0; i < k; i++)
    {
        arr[j++] = args[i];
    }
    arr[j] = NULL;

    intMode(arr);
    close(file);

    dup2(stdinCopy, STDIN_FILENO);
    close(stdinCopy);

    return 0;
}

int redirectOut(char *args[], int k)
{
    int file;
    char *arr[50];
    int stdoutCopy = dup(STDOUT_FILENO);

    file = open(args[k + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);

    if (file == -1)
        errorMsg();

    int file2 = dup2(file, STDOUT_FILENO);

    if (file2 == -1)
        errorMsg();

    int j = 0;
    for (int i = 0; i < k; i++)
    {
        arr[j++] = args[i];
    }
    arr[j] = NULL;
    intMode(arr);
    close(file);

    dup2(stdoutCopy, STDOUT_FILENO);
    close(stdoutCopy);

    return 0;
}

int runHistory(char *input)
{
    int num = atoi(input + 1); // Convert the number after !

    // Check if number is valid (between 1 and history_count)
    if (num <= 0 || num > history_count)
    {
        errorMsg();
        return 1;
    }
    //  printf("DEBUG: IN HISTORY COUNT: %d", history_count);

    // Get the command from history (adjust for 0-based array)
    char *historyCmd = strdup(history[num - 1]);
    //  printf("%s\n", historyCmd); // Echo the command being executed

    // Parse and execute the historical command
    cleanInput(historyCmd);
    return 0;
}

int killProcess(char *args[])
{
    if (args[1] == NULL)
    {
        errorMsg();
        return 1;
    }

    int signal = SIGTERM;
    int pidIndex = 1;

    if (args[1][0] == '-')
    {
        signal = atoi(args[1] + 1);
        pidIndex = 2;
    }

    if (args[pidIndex] == NULL)
    {
        errorMsg();
        return 1;
    }

    int pid = atoi(args[pidIndex]);
    if (pid <= 0)
    {
        errorMsg();
        return 1;
    }

    if (kill(pid, signal) == -1)
    {
        errorMsg();
        return 1;
    }

    errorMsg();
    return 0;
}
