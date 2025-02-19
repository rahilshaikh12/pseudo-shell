#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define HISTORY_SIZE 10
#define PATH_COUNT 100

// Global history array to retain previous commands
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
int redirect(char *args[], int);
int processChange(char *args[], int k);
int sendInput(char *args[], int k);
int parallelProcess(char *args[], int);

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
        printf("Interactive Mode\n");
        shellLoop();
    }

    else if (argc == 2)
    {
        printf("Batch Mode\n");
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
    printf("DEBUG: %s %s", args[0], args[1]);
    if (args == NULL || args[0] == NULL)
    {
        errorMsg();
        return 1;
    }

    if (!strcmp(args[0], "exit") || !strcmp(args[0], "cd") || !strcmp(args[0], "kill") ||
        !strcmp(args[0], "history") || !strcmp(args[0], "pwd") || !strcmp(args[0], "path"))
    {
        printf("BUILTIN MODE\n");
        builtins(args);
    }
    else
    {
        printf("EXEC MODE\n");
        execCommand(args);
        for (int i = 0; i < 6 && args[i] != NULL; i++)
            printf("%s", args[i]);
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
        printf("CD");
        changeDir(args);
    }
    else if (!strcmp(args[0], "kill"))
    {
        printf("KILL");
    }
    else if (!strcmp(args[0], "history"))
    {
        printf("HISTORY\n");
    }
    else if (!strcmp(args[0], "pwd"))
    {
        printf("PWD");
        printDir();
    }
    else if (!strcmp(args[0], "path"))
    {
        printf("PATH");
        setPath(args);
    }

    return 0;
}

int shellLoop()
{
    size_t buff = 256;
    char *input = malloc(buff);
    while (true)
    {
        printf("$> ");
        getline(&input, &buff, stdin);
        cleanInput(input);
    }
    free(input);
}

int cleanInput(char *input)
{
    while (*input == ' ' | *input == '\t')
    {
        input++;
    }
    if (input[strlen(input) - 1] == '\n')
    {
        input[strlen(input) - 1] = '\0';
    }
    printf("%s\n", input);
    parseInput(input);
    printHistory(input);
    return 0;
}

int parseInput(char *input)
{
    int i = 0;
    char *args[100];

    char *tokens = strtok(input, " ");

    while (tokens != NULL)
    {
        args[i++] = tokens;
        tokens = strtok(NULL, " ");
    }
    args[i] = NULL;
    sendInput(args, i);
    return 0;
}

int printHistory(char *input)
{
    if (strcmp(input, "history") != 0)
    {
        history[history_count % HISTORY_SIZE] = strdup(input);
        if (history_count < HISTORY_SIZE)
        {
            history_count++;
        }
    }
    else
    {
        for (int i = 0; i < history_count; i++)
        {
            printf("%d %s\n", i + 1, history[i]);
        }
    }
    return 0;
}

int changeDir(char *args[])
{
    char s[100];

    if (countArgs(args) > 2)
        errorMsg();
    printf("%s\n", getcwd(s, 100));
    int cDir = chdir(args[1]);
    if (cDir != 0)
        errorMsg();

    printf("%s\n", getcwd(s, 100));
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

int setPath(char *args[])
{
    printf("%s\n", path[0]);
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
    char sign[] = "/";
    printf("DEBUG: IN EXEC");

    if (command[0] == '/' || command[0] == '.')
    {
        printf("Absolute or ");
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
        }

        wait(NULL);
        return 0;
    }

    return 0;
}

int processChange(char *args[], int k)
{

    return 0;
}

int sendInput(char *args[], int k)
{
    int indx = 0;
    int redirectCount = 0;
    int pipeCount = 0;
    int processCount = 0;
    bool isRedirect = false;
    bool isPipe = false;
    bool isProcess = false;

    for (int i = 0; i < k; i++)
    {
        if (!strcmp(args[i], "<") || !strcmp(args[i], ">"))
        {
            redirectCount++;
            isRedirect = true;

            if (redirectCount == 1)
                indx = i;
        }
        if (!strcmp(args[i], "|"))
        {
            pipeCount++;
            isPipe = true;
        }
        if (!strcmp(args[i], "&"))
        {
            processCount++;
            isProcess = true;
        }
    }

    if (redirectCount > 1)
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

    if (redirectCount == 1)
    {
        printf("REDIRECT");
        redirect(args, indx);
    }
    if (pipeCount > 0)
        printf("PIPES");
    if (processCount > 0)
    {
        printf("PROCESS");
        parallelProcess(args, k);
    }

    if (redirectCount == 0 && pipeCount == 0 && processCount == 0)
    {
        intMode(args);
    }

    return 0;
}

int redirect(char *args[], int k)
{
    int file;
    char *arr[50];
    int stdoutCopy = dup(STDOUT_FILENO);
    if (args[k + 2] == NULL)
    {
        if (!strcmp(args[k], "<"))
            printf("REDIRECT IN");
        else if (!strcmp(args[k], ">"))
        {
            printf("REDIRECT OUT");
            file = open(args[k + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (file == -1)
                errorMsg();
            int file2 = dup2(file, STDOUT_FILENO);
            if (file2 == -1)
                errorMsg();
        }
    }
    else
    {
        errorMsg();
        return 1;
    }
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

int parallelProcess(char *args[], int k)
{
    int start = 0;
    int lastForeground = 1;
    for (int i = 0; i <= k; i++)
    {
        if (i == k || strcmp(args[i], "&") == 0)
        {

            args[i] = NULL;

            int forkID = fork();

            if (forkID == 0)
            {
                execvp(args[start], &args[start]);
                perror("execvp failed");
                exit(1);
            }
            else if (forkID < 0)
            {
                perror("fork failed");
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
            lastForeground = (i == k);
        }
    }

    return 0;
}