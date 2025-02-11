#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int intMode();
void errorMsg();
int builtins(char *hello);

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
        intMode();
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

int intMode()
{
    size_t bufferSize = 100;
    char *input = malloc(bufferSize);
    while (true)
    {
        printf("$> ");
        getline(&input, &bufferSize, stdin);
        builtins(input);
    }
    free(input);
}

int builtins(char *input)
{
    // remove newline char
    input[strlen(input) - 1] = '\0';
    printf("%lu\n", strlen(input));
    // char *token = strtok(input, " ");
    // printf("%s\n", token + 1);

    // exit, cd, kill, history, pwd, and path
    printf("Input from shell: %s\n", input);
    return 0;
}
