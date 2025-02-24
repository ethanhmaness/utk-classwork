#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include "jrb.h"
#include "dllist.h"

void PrintDirTar(char *);
void PrintFileTar(char *);

struct JtarData
{
    JRB inodes;
    struct stat *statbuf;
    char *root_path;
} Jtar;

/* Returns a new string containing the contents of a and b concatenated */
char *StrCpyCat(char *a, char *b)
{
    char *rv = malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
    strcpy(rv, a);
    strcat(rv, b);
    return rv;
}

/* Takes a path and an absolute parent directory to make absolute file path */
char *AbsolutePath (char *path, char *abs_parent)
{
    char *base = malloc(sizeof(char) * 256);
    strcpy(base, path);
    basename(base);
    char *temp = malloc(sizeof(char) * 256);
    strcpy(temp, abs_parent);
    strcat(temp, "/");
    strcat(temp, base);
    free(base);
    return temp;
}

/* Print the file name length, file name, and inode */
void PrintBaseInfo (char *path)
{
    char *full_path = StrCpyCat(Jtar.root_path, path);
    stat(full_path, Jtar.statbuf);
    size_t namelen = strlen(path);
    fwrite(&namelen, 4, 1, stdout);
    for(int i = 0; i < strlen(path); i++)
    {
        printf("%c", path[i]);
    }
    unsigned long inode_num = Jtar.statbuf->st_ino;
    fwrite(&inode_num, 8, 1, stdout);
}

/* Compares the inodes stored at void * locations in Jvals */
int CompareInodeJval(Jval a, Jval b)
{
    return *(int *)(a.v) - *(int *)(b.v);
}

/* 
 * Checks if the inode of the given file or directory has been seen
 * If not, prints the info and marks as seen
 */
void PrintInodeInfoIfUnique (char *path, int is_file)
{
    char *full_path = StrCpyCat(Jtar.root_path, path);
    stat(full_path, Jtar.statbuf);
    unsigned long *inode = malloc(sizeof(long));
    *inode = Jtar.statbuf->st_ino;

    //Check if inode is already in tree
    Jval inode_jval = new_jval_v(inode);
    if (jrb_find_gen(Jtar.inodes, inode_jval, CompareInodeJval) == NULL)
    {
        //Insert into tree
        jrb_insert_gen(Jtar.inodes, inode_jval, inode_jval, CompareInodeJval);

        //Print mode and modification time
        unsigned int mode = Jtar.statbuf->st_mode;
        fwrite(&mode, 4, 1, stdout);
        long modtime = Jtar.statbuf->st_mtime;
        fwrite(&modtime, 8, 1, stdout);
        
        //If file, print size and contents
        if (!S_ISDIR(Jtar.statbuf->st_mode))
        {
            long size = Jtar.statbuf->st_size;
            fwrite(&size, 8, 1, stdout);
            FILE *f = fopen(full_path, "rb");
            char byte;
            for (long i = 0; i < Jtar.statbuf->st_size; i++)
            {
                fread(&byte, 1, 1, f);
                printf("%c",  byte);
            }
            fclose(f);
        }
    }
    else
    {
        free(inode);
    }
}

/* 
 * General recursive tar data print procedure
 * If path isn't found, throws error
 * If path is file, calls PrintFileTar
 * If path is directory, calls PrintDirTar
 */
int GenPrintTar (char *path)
{
    char *full_path = StrCpyCat(Jtar.root_path, path);
    if (stat(full_path, Jtar.statbuf))
    {
        printf("Cannot open %s\n", path);
        return 1;
    }

    if (S_ISDIR(Jtar.statbuf->st_mode))
    {
        PrintDirTar(path);
    }
    else
    {
        PrintFileTar(path);
    }
    return 0;
}

/* Recursive tar data print procedure for directories */
void PrintDirTar (char *path)
{
    PrintBaseInfo(path);
    PrintInodeInfoIfUnique(path, 0);

    char *full_path = StrCpyCat(Jtar.root_path, path);
    DIR *d = opendir(full_path);
    Dllist children = new_dllist();
    struct dirent *d_entry = NULL;
    if (d)
    {
        while ((d_entry = readdir(d)))
        {
            if (strcmp(d_entry->d_name, ".") && strcmp(d_entry->d_name, ".."))
            {
                dll_append(children, new_jval_s(AbsolutePath(d_entry->d_name, path)));
            }
        }
    }
    closedir(d);
    free(d_entry);
    Dllist node;
    dll_traverse(node, children)
    {
        GenPrintTar(node->val.s);
    }
}

/* Recursive tar data print procedure for files*/
void PrintFileTar (char *path)
{
    PrintBaseInfo(path);
    PrintInodeInfoIfUnique(path, 1);
}

int main (int argc, char* argv[])
{

    /* Invalid call */
    if (argc != 2)
    {
        printf("Usage: tarc <directory>\n");
        exit(1);
    }

    Jtar.statbuf = malloc(sizeof(struct stat));
    Jtar.inodes = make_jrb();
    Jtar.root_path = malloc(sizeof(char) * 256);
    strcpy(Jtar.root_path, argv[1]);
    dirname(Jtar.root_path);
    strcat(Jtar.root_path, "/");

    /* Recursive print */
    GenPrintTar(basename(argv[1]));

}
