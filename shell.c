#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define HISTORY_SIZE 10

// Global history array to retain previous commands
char *history[HISTORY_SIZE];
int history_count = 0; // Number of stored commands

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

void errorMsg()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char *argv[])
{

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
    // exit, cd, kill, history, pwd, and path
    if (!strcmp(args[0], "exit") | !strcmp(args[0], "cd") | !strcmp(args[0], "kill") | !strcmp(args[0], "history") | !strcmp(args[0], "pwd") | !strcmp(args[0], "path"))
    {
        printf("BUILTIN MODE\n");
        builtins(args);
    }
    else
    {
        printf("EXEC MODE");
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
    intMode(args);
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