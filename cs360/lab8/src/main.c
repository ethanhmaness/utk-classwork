#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "dllist.h"
#include "jval.h"
#include "fields.h"

// Returns an integer indicating a particular command token type
int token_type (char *s)
{
    if (strcmp(s, "<") == 0) return 1;
    if (strcmp(s, ">") == 0) return 2;
    if (strcmp(s, ">>") == 0) return 3;
    if (strcmp(s, "&") == 0) return 4;
    return 0;
}

void perror_and_exit(char *errstr)
{
    perror(errstr);
    exit(1);
}

// stores info about a command call
typedef struct command
{
    Dllist args;
    char    *name,
            *inputfname,
            *outputfname,
            append_output,
            input_from_pipe,
            output_to_pipe,
            await;
} *Command;

// constructs a default Command
Command new_command ()
{
    Command new_cmd = malloc(sizeof(struct command));
    new_cmd->name = NULL,
    new_cmd->args = new_dllist(),
    new_cmd->inputfname = NULL,
    new_cmd->outputfname = NULL,
    new_cmd->append_output = 0,
    new_cmd->input_from_pipe = 0,
    new_cmd->output_to_pipe = 0,
    new_cmd->await = 1;
    return new_cmd;
}

// gets the args to exec for a command
char **cmd_exec_str (Command cmd)
{
    size_t  argc = 1,
            i = 0;
    Dllist ptr;
    dll_traverse(ptr, cmd->args)
    {
        argc++;
    }
    char **argv = malloc(sizeof(char *) * (argc + 1));
    argv[i] = cmd->name;
    dll_traverse(ptr, cmd->args)
    {
        i++,
        argv[i] = ptr->val.s;
    }
    argv[i+1] = NULL;
    return argv;
}

// parses a list of strings into a command
Command cmd_from_str_list (Dllist str_list)
{
    Command cmd = new_command();

    // if no strings in list return default Command
    if (dll_empty(str_list))
    {
        return cmd;
    }

    // set name to first string
    Dllist l_ptr = dll_first(str_list);
    cmd->name = l_ptr->val.s;

    // read args until redirect token found or end of list
    int token_t;
    for (l_ptr = dll_next(l_ptr); l_ptr != dll_nil(str_list) && (token_t = token_type(l_ptr->val.s)) == 0; l_ptr = dll_next(l_ptr))
    {
        dll_append(cmd->args, new_jval_s(l_ptr->val.s));
    }

    for (; l_ptr != dll_nil(str_list); l_ptr = dll_next(l_ptr))
    {
        // get token
        token_t = token_type(l_ptr->val.s);

        // if no await flag token
        if (token_t == 4)
        {
            cmd->await = 0;
            break;
        }

        // get arg for redirect
        l_ptr = dll_next(l_ptr);

        // if arg found
        if (l_ptr != dll_nil(str_list))
        {
            // if redirect input
            if (token_t == 1 && cmd->inputfname == NULL)
            {
                // get filename for input
                cmd->inputfname = l_ptr->val.s;
            }
            // if redirect output
            else if ((token_t == 2 || token_t == 3) && cmd->outputfname == NULL)
            {
                // get filename for output
                cmd->outputfname = l_ptr->val.s,
                // flag append vs overwrite
                cmd->append_output = (token_t == 3);
            }
        }
    }

    return cmd;
}

// stores strings from adjacent memory in a list
Dllist str_list_from_str_arr (char **strs, int num_strs)
{
    Dllist str_list = new_dllist();
    for (int i = 0; i < num_strs; i++)
    {
        dll_append(str_list, new_jval_s(strdup(strs[i])));
    }
    return str_list;
}

// Takes a list of strings and splits it into a list of lists of strings, divided by any strings that match delim
Dllist str_list_split (Dllist str_list, char *delim)
{
    Dllist  str_list_list = new_dllist(),
            new_str_list = new_dllist(),
            ptr;

    // reads through list of strings
    dll_traverse(ptr, str_list)
    {
        // if string matches delim, put current list into lists list and start new list
        if (strcmp(ptr->val.s, delim) == 0)
        {
            dll_append(str_list_list, new_jval_v(new_str_list)),
            new_str_list = new_dllist();
        }
        // if string does not match delim, add to current list
        else
        {
            dll_append(new_str_list, new_jval_s(ptr->val.s));
        }
    }

    dll_append(str_list_list, new_jval_v(new_str_list));

    return str_list_list;
}

// takes a list of commands and marks their piping flags, assuming they are in piping order
Dllist cmd_list_set_pipe (Dllist cmd_list)
{
    Dllist ptr;
    Command cmd;
    dll_traverse(ptr, cmd_list)
    {
        cmd = (Command) ptr->val.v;
        if (ptr != dll_first(cmd_list))
        {
            cmd->input_from_pipe = 1;
        }
        if (ptr != dll_last(cmd_list))
        {
            cmd->output_to_pipe = 1;
        }
    }
    return cmd_list;
}

// takes a list of lists of strings and creates a list of consecutively piped commands,
// assuming each string list contains strings for a command call
Dllist piped_cmd_list_from_str_list_list (Dllist str_list_list)
{
    Dllist  cmd_list = new_dllist(),
            ptr;
    dll_traverse(ptr, str_list_list)
    {
        dll_append(cmd_list, new_jval_v(cmd_from_str_list((Dllist) ptr->val.v)));
    }
    return cmd_list_set_pipe(cmd_list);
}

int main (int argc, char** argv) {

    char *prompt = calloc(128, 1);

    // prompt is argument or default
    strcpy(prompt, (argc == 2) ? argv[1] : "jsh1: ");

    // for reading commands
    IS is = new_inputstruct(NULL);

    // while not eof or ctrl-d
    while (get_line(is) != -1)
    {
        // skip empty line
        if (is->NF == 0) continue;

        // exit on exit
        if (strcmp(is->fields[0], "exit") == 0) break;

        // construct command list from input strings
        Dllist  line_str_list = str_list_from_str_arr(is->fields, is->NF),
                cmd_str_list_list = str_list_split(line_str_list, "|"),
                cmd_list = piped_cmd_list_from_str_list_list(cmd_str_list_list),
                ptr;

        int status,
            forkval,
            waitpid,
            prevpipeout = -1,
            pipefd[2] = {-1, -1};

        // for each command
        dll_traverse(ptr, cmd_list)
        {
            Command cmd = (Command) ptr->val.v;

            // move previous pipe read fd
            if (pipefd[0] != -1)
            {
                prevpipeout = pipefd[0],
                pipefd[0] = -1;              
            }

            // pipe if not the last command
            if (cmd->output_to_pipe)
            {
                if (pipe(pipefd) < 0)
                {
                    perror_and_exit("pipe");
                }
            }

            // fork, and if child then execute command
            if (forkval = fork(), forkval == 0)
            {
                // no need for new pipe read fd
                if (pipefd[0] != -1)
                {
                    close(pipefd[0]);
                }

                int inputf = -1;
                int outputf = -1;

                // if redirecting input to file, open dup and close
                if (cmd->inputfname != NULL)
                {
                    inputf = open(cmd->inputfname, O_RDONLY);
                    if (inputf < 0)
                    {
                        perror_and_exit(cmd->inputfname);
                    }
                    if (dup2(inputf, 0) != 0)
                    {
                        perror_and_exit(cmd->inputfname);
                    }
                    close(inputf);
                }
                // if input from prev pipe, dup
                else if (cmd->input_from_pipe)
                {
                    if (dup2(prevpipeout, 0) != 0)
                    {
                        perror_and_exit("pipe");
                    }
                    close(prevpipeout);
                }

                // if redirected output to file, open dupe and close
                if (cmd->outputfname != NULL)
                {
                    outputf = open(cmd->outputfname, O_WRONLY | ((cmd->append_output) ? O_APPEND : O_TRUNC) | O_CREAT, 0644);
                    if (outputf < 0)
                    {
                        perror_and_exit(cmd->outputfname);
                    }
                    if (dup2(outputf, 1) != 1)
                    {
                        perror_and_exit(cmd->outputfname);
                    }
                    close(outputf);
                }
                // if output to pipe, dup
                else if (cmd->output_to_pipe)
                {
                    if (dup2(pipefd[1], 1) != 1)
                    {
                        perror_and_exit("pipe");
                    }
                }

                // close pipe write fd
                close(pipefd[1]),

                // exec command and exit anyways if fail
                execvp(cmd->name, cmd_exec_str(cmd)),
                perror_and_exit(cmd->name);
            }
            // if bad fork, exit
            else if (forkval == -1)
            {
                exit(1);
            }

            // close unneeded fds
            if (prevpipeout != -1)
            {
                close(prevpipeout);   
            }
            if (pipefd[1] != -1)
            {
                close(pipefd[1]);
            }

            // wait for child to finish
            if (cmd->await)
            {
                while (waitpid = wait(&status), waitpid != forkval);
            }


        }

        // free all of the memory
        dll_traverse(ptr, line_str_list)
        {
            free(ptr->val.s);
        }
        
        dll_traverse(ptr, cmd_list)
        {
            free_dllist(((Command) ptr->val.v)->args),
            free((Command) ptr->val.v);
        }
        dll_traverse(ptr, cmd_str_list_list)
        {
            free_dllist(ptr->val.v);
        }
        free_dllist(cmd_list),
        free_dllist(line_str_list),
        free_dllist(cmd_str_list_list);

        if (prevpipeout != -1)
        {
            close(prevpipeout);
        }
    }

    // free even more memorys
    free(prompt),
    jettison_inputstruct(is);

    return 0;
}
