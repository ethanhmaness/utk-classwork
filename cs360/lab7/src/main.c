#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "fields.h"

// Returns an integer indicating a particular token type
int tokentype(char *s)
{
    if (strcmp(s, "<") == 0) return 1;
    if (strcmp(s, ">") == 0) return 2;
    if (strcmp(s, ">>") == 0) return 3;
    if (strcmp(s, "&") == 0) return 4;
    return 0;
}

int main(int argc, char** argv) {

    char *prompt = calloc(128, 1);

    // prompt is argument or default
    strcpy(prompt, (argc == 2) ? argv[1] : "jsh1: ");

    // vars
    IS is = new_inputstruct(NULL);
    char **args;
    char *inputfname;
    char *outputfname;
    int inputf;
    int outputf;
    int token_t;
    int is_append;
    int findex;
    int status;
    int await;
    int forkval;
    int waitpid;

    // while not eof or ctrl-d
    while (get_line(is) != -1)
    {

        // skip empty line
        if (is->NF == 0) continue;

        // exit on exit
        if (strcmp(is->fields[0], "exit") == 0) break;

        // create command args array
        args = malloc(sizeof(char *) * (is->NF + 1));

        // default to wait for command to finish
        await = 1;

        // init vals
        inputfname = NULL;
        outputfname = NULL;
        // inputf = NULL;
        // outputf = NULL;
        findex = 0;
        is_append = 0;

        // while reading command args
        while (findex < is->NF && (token_t = tokentype(is->fields[findex])) == 0)
        {
            args[findex] = is->fields[findex];
            findex++;
        }

        args[findex] = NULL;

        // redirection handling
        while (findex < is->NF)
        {
            // get token and skip
            token_t = tokentype(is->fields[findex]);
            findex++;

            // if redirect input
            if (token_t == 1)
            {
                // get filename for input
                inputfname = malloc(sizeof(char) * 256);
                strcpy(inputfname, is->fields[findex]);
            }

            // if redirect output
            else if (token_t == 2 || token_t == 3)
            {
                // get filename for output
                outputfname = malloc(sizeof(char) * 256);
                strcpy(outputfname, is->fields[findex]);
                // flag append vs overwrite
                is_append = (token_t == 3);
            }

            // if no-wait token
            else if (token_t == 4)
            {
                await = 0;
                break;
            }

            // if unknown
            else
            {
                perror("invalid token\n");
                exit(1);
            }

            findex++;
        }

        // fork, and if child then execute command
        if (forkval = fork(), forkval == 0)
        {

            // redirect input
            if (inputfname != NULL)
            {
                inputf = open(inputfname, O_RDONLY);
                if (inputf < 0)
                {
                    perror(inputfname);
                    exit(1);
                }
                if (dup2(inputf, 0) != 0)
                {
                    perror(inputfname);
                    exit(1);
                }
                close(inputf);
            }

            // redirect output
            if (outputfname != NULL)
            {
                outputf = open(outputfname, O_WRONLY | ((is_append) ? O_APPEND : O_TRUNC) | O_CREAT, 0644);
                if (outputf < 0)
                {
                    perror(outputfname);
                    exit(1);
                }
                if (dup2(outputf, 1) != 1)
                {
                    perror(outputfname);
                    exit(1);
                }
                close(outputf);
            }

            execvp(is->fields[0], args);
            perror(is->fields[0]);
            exit(1);
        }
        // if bad fork, exit
        else if (forkval == -1)
        {
            exit(1);
        }
        // wait for child to finish
        else if (await)
        {
            while (waitpid = wait(&status), waitpid != forkval);
        }
    }

    return 0;
}
