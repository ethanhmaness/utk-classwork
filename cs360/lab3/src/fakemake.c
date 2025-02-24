#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fields.h"
#include "dllist.h"

/* Prints a string to stderr and exits */
void ExitWithPrintErr(const char *str)
{
    fprintf(stderr, str),
    exit(1);
}

/* Returns a new string with the contents of str up to the first period */
char *CopyFilenameWithoutExtension(char *str)
{
    return strncpy(calloc(1, 256 * sizeof(char)), str, strcspn(str, "."));
}

/* Runs command, exits on failure */
void RunCommandExitOnFail(char *cmd, char *errmsg)
{
    printf("%s\n", cmd);

    if (system(cmd) != 0)
        ExitWithPrintErr(errmsg);
}

void FreeStringDllist(Dllist d)
{
    Dllist node;

    dll_traverse(node, d)
        free(node->val.s);

    free_dllist(d);
}

int main(int argc, char** argv)
{

/*
 * READING INPUT
 */

    /* Read in description file name or default to 'fmakefile' if unspecified */
    IS is = new_inputstruct( (argc < 2) ? "fmakefile" : argv[1] );

    /* Vars for storing fakemake  info */
    Dllist  source_files = new_dllist(),
            header_files = new_dllist(),
            flags = new_dllist(),
            libraries = new_dllist();

    char    *executable_name = NULL,
            *err = calloc(1, 256 * sizeof(char));

    /*
     * Read in each line, parsing and storing the relevant information.
     */
    while(get_line(is) >= 0)
    {
    
        /* List of C source files -- append */
        if (strcmp(is->fields[0], "C") == 0)
            for (int i = 1; i < is->NF; i++)
                dll_append(source_files, new_jval_s(strdup(is->fields[i])));

        /* List of C header files -- append */
        else if (strcmp(is->fields[0], "H") == 0) 
            for (int i = 1; i < is->NF; i++)
                dll_append(header_files, new_jval_s(strdup(is->fields[i])));

        /* Executable file name -- set (cannot be set twice) */
        else if (strcmp(is->fields[0], "E") == 0) 
        {
            if (executable_name == NULL)
                executable_name = strdup(is->fields[1]);

            else
                sprintf(err, "fmakefile (%d) cannot have more than one E line\n", is->line),
                ExitWithPrintErr(err);
        }

        /* List of flags -- append */
        else if (strcmp(is->fields[0], "F") == 0) 
            for (int i = 1; i < is->NF; i++)
                dll_append(flags, new_jval_s(strdup(is->fields[i])));

        /* List of libraries -- append */
        else if (strcmp(is->fields[0], "L") == 0) 
            for (int i = 1; i < is->NF; i++)
                dll_append(libraries, new_jval_s(strdup(is->fields[i])));

    }

    /* error if no executable name is found */
    if (executable_name == NULL)
        ExitWithPrintErr("No executable specified\n");

    /* Free memory from inputstruct */
    jettison_inputstruct(is);

/*
 * COMPILING FILES
 */

    /* vars for determining compilation actions */
    struct stat *filestat = malloc(sizeof(struct stat));

    Dllist  node,
            object_files = new_dllist(),
            source_files_to_compile = new_dllist();

    time_t  time,
            latest_h_mod = 0,
            latest_o_mod = 0;

    char    *filename,
            *command = calloc(1, 256 * sizeof(char)),
            *flagstr = calloc(1, 256 * sizeof(char)),
            *objstr = calloc(1, 256 * sizeof(char)),
            *libstr = calloc(1, 256 * sizeof(char));

    /* Check that header files exist and determine latest header modification time value */
    dll_traverse(node, header_files)
        if (stat(node->val.s, filestat) != 0 && filestat->st_mtime > latest_h_mod)
            latest_h_mod = filestat->st_mtime;

    /* Populate object file names list */
    dll_traverse(node, source_files)
        filename = strcat(CopyFilenameWithoutExtension(node->val.s), ".o"),
        dll_append(object_files, new_jval_s(filename));

    /* If an object file doesn't exist, or is older than its source file, the source must be recompiled */
    dll_traverse(node, object_files)
    {
        stat(filename = strcat(CopyFilenameWithoutExtension(node->val.s), ".c"), filestat),
        time = filestat->st_mtime;

        if (stat(node->val.s, filestat) != 0 || filestat->st_mtime < time)
            dll_append(source_files_to_compile, new_jval_s(filename));

        else
            free(filename);
    }

    /* Build flag list string */
    dll_traverse(node, flags)
        strcat(flagstr, " "),
        strcat(flagstr, node->val.s);

    /* Build object list string */
    dll_traverse(node, object_files)
        strcat(objstr, " "),
        strcat(objstr, node->val.s);

    /* Build library list string */
    dll_traverse(node, libraries)
        strcat(libstr, " "),
        strcat(libstr, node->val.s);

    /* Recompile necessary source files */
    dll_traverse(node, source_files_to_compile)
    {
        if (stat(node->val.s, filestat) != 0)
            sprintf(err, "fmakefile: %s: No such file or directory\n", node->val.s),
            ExitWithPrintErr(err);
        
        sprintf(command, "gcc -c%s %s", flagstr, node->val.s),
        RunCommandExitOnFail(command, "Command failed.  Exiting\n");
    }

    /* Determine time of last object file modification */
    dll_traverse(node, object_files)
        if (stat(node->val.s, filestat) == 0 && filestat->st_mtime > latest_o_mod)
            latest_o_mod = filestat->st_mtime;

    /* Remake executable if any source code was compiled or no executable younger than the last object mod already exists */
    if ((!dll_empty(source_files_to_compile)) || stat(executable_name, filestat) != 0 || filestat->st_mtime < latest_o_mod)
        sprintf(command, "gcc -o %s%s%s%s", executable_name, flagstr, objstr, libstr),
        RunCommandExitOnFail(command, "Command failed.  Fakemake exiting\n");
    
    else
        printf("%s up to date\n", executable_name);

    /* Free all memory */

    FreeStringDllist(source_files),
    FreeStringDllist(header_files),
    FreeStringDllist(flags),
    FreeStringDllist(libraries),
    FreeStringDllist(source_files_to_compile),
    FreeStringDllist(object_files),
    free(executable_name),
    free(filestat),
    free(command),
    free(flagstr),
    free(objstr),
    free(libstr),
    free(err);

    return 0;
}