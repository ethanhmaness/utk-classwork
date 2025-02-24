#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <utime.h>
#include <string.h>
#include "jrb.h"

JRB inode_tree;
JRB mode_tree;
JRB mtime_tree;

void printErrAndExit(const char *str)
{
    fprintf(stderr, str);
    exit(1);
}

int tarx_read()
{
    //fread basic info
    //if not right name len return (eof or error)
    //convert nums from big endian to little endian
    size_t name_len;

    if (fread(&name_len, 4, 1, stdin) != 1)
    {
        printf("cannot read fname len\n");
        return 0;
    }

    if (name_len > 511)
    {
        printErrAndExit("invalid name len\n");
    }

    char *fname = calloc(sizeof(char) * 512, 1);
    size_t read_name_len = fread(fname, 1, name_len, stdin);
    //printf("fname: %s\n", fname); ///////////////////

    if (read_name_len != name_len)
    {
        printErrAndExit("cannot read fname\n");
    }

    unsigned long inode_num;
    if (fread(&inode_num, 8, 1, stdin) != 1)
    {
        printErrAndExit("cannot read inode\n");
    }
    //printf("inode: %lu\n", inode_num); ///////////////////

    //if inode has been seen
    //create link w link() and increment st_nlink
    JRB ptr;
    if ((ptr = jrb_find_int(inode_tree, inode_num)) != NULL)
    {
        if (link(ptr->val.s, fname) != 0)
        {
            printErrAndExit("failed to create link\n");
        }
        //printf("linked to %s\n", ptr->val.s); ///////////////////
    }
    //else
    //add file name to tree, key is inode
    //fread mode and mod time, store in tree keyed on name
    else
    {
        jrb_insert_int(inode_tree, inode_num, new_jval_s(fname));
        int mode;
        long mtime;
        if (fread(&mode, 4, 1, stdin) != 1)
        {
            printErrAndExit("cannot read file mode\n");
        }
        //printf("mode: %d\n", mode); ///////////////////
        if (fread(&mtime, 8, 1, stdin) != 1)
        {
            printErrAndExit("cannot read file mtime\n");
        }
        //printf("mtime: %ld\n", mtime); ///////////////////
        jrb_insert_str(mode_tree, fname, new_jval_i(mode));
        jrb_insert_str(mtime_tree, fname, new_jval_l(mtime));

        //if file is dir
        //mkdir(name, 0777)
        if (S_ISDIR(mode))
        {
            mkdir(fname, 0777);
            //printf("made dir %s\n", fname); ///////////////////
        } 
        //else
        //fread size and contents
        //create file with fopen(name, "w")
        //fwrite contents
        //close file
        else 
        {
            long fsize;
            if (fread(&fsize, 8, 1, stdin) != 1)
            {
                printErrAndExit("cannot read fsize\n");
            }
            //printf("read fsize: %ld\n", fsize); ///////////////////
            char *contents = malloc(fsize);
            if (fread(contents, 1, fsize, stdin) != fsize)
            {
                printErrAndExit("cannot read file contents\n");
            }
            FILE *new_file = fopen(fname, "w");
            fwrite(contents, 1, fsize, new_file);
            fclose(new_file);
        }
    }
    return 1;
}

void update_modes_and_times()
{
    //traverse inode tree
    //use name to find st_mode and st_mtime in other tree
    //use struct utimbuf set modtime to mtime from tree
    //call utime() and chmod() to update file
    JRB ptr;
    jrb_traverse(ptr, inode_tree)
    {
        char *fname = ptr->val.s;
        //printf("fname: %s\n", fname); ///////////////////
        int mode = jrb_find_str(mode_tree, fname)->val.i;
        //printf("mode: %d\n", mode); ///////////////////
        long mtime = jrb_find_str(mtime_tree, fname)->val.l;
        //printf("mtime: %ld\n", mtime); ///////////////////
        struct utimbuf *t = malloc(sizeof(struct utimbuf));
        t->actime = 0;
        t->modtime = mtime;
        if (utime(fname, t) != 0)
        {
            printErrAndExit("cannot set mod time\n");
        }
        if (chmod(fname, mode) != 0)
        {
            printErrAndExit("cannot set mode\n");
        }
    }
}

int main(int argc, char* argv[])
{
    inode_tree = make_jrb();
    mode_tree = make_jrb();
    mtime_tree = make_jrb();
    while(tarx_read());
    update_modes_and_times();
    return 0;
}
