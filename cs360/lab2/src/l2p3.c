#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "jrb.h"
#include "dllist.h"

/* Stores an ip and a list of names, representing a machine */
typedef struct Machine
{
    Dllist names;
    unsigned char *ip;
} Machine;

/* Prints four unsigned chars from the given address */
char *IPToString(unsigned char *ip)
{
    char *rv = malloc(sizeof(char) * 16);
    sprintf(rv, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return rv;
}

/* Compresses four unsigned chars from the given address into a single int */
int IPToInt(unsigned char *ip)
{
    return (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
}

/* Creates a new string with the contents of the old string, up to a dot */
char *StringUntil(char *str, const char *delim)
{
    char *rv = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(rv, str);
    rv[strcspn(rv, ".")] = '\0';
    return rv;
}

/* Reads in a machine from a memory buffer at f from a given offset, increasing offset accordingly */
Machine *ReadMachine(unsigned char* f, int *offset)
{
    if(*(f + *offset) == '\0') return NULL;

    int num_names;
    unsigned char buffer[4];
    char *name;
    char c;

    Machine *m;
    unsigned char* ip = malloc(sizeof(unsigned char) * 4);
    Dllist names = new_dllist();

    memcpy(ip, f + *offset, 4);
    *offset += 4;

    memcpy(buffer, f + *offset, 4);
    *offset += 4;
    num_names = IPToInt(buffer);

    for (int i = 0; i < num_names; i++)
    {
        name = malloc(sizeof(char) * 128);
        strcpy(name, "");
        do
        {
            memcpy(&c, f + *offset, 1);
            (*offset)++;
            strcat(name, &c);
        } while (c != '\0');

        dll_append(names, new_jval_s(name));

        if (strchr(name, '.') != NULL)
        {
            dll_append(names, new_jval_s(StringUntil(name, ".")));
        }
    }

    m = malloc(sizeof(Machine));
    m->ip = ip;
    m->names = names;

    return m;
}

/* Reads all machines from a memory buffer */
JRB ReadAllMachines(unsigned char* f)
{
    JRB machine_tree = make_jrb();
    Machine *m;
    Dllist node;
    int *i = malloc(sizeof(int));
    *i = 0;
    while (m = ReadMachine(f, i))
    {
        dll_traverse(node, m->names)
        {
            jrb_insert_str(machine_tree, node->val.s, new_jval_v(m));
        }
    }
    /* help idk why theres still a single machine leaking omfg */
    if(m != NULL)
        {
            free(m->ip);
            dll_traverse(node, m->names)
            {
                free(node->val.s);
            }
            free_dllist(m->names);
            free(m->names);
        }
        free(m);
    free(i);
    return machine_tree;
}

/* Reads in memory from "converted" to a tree of machines and allows for searching of the tree by name */
int main()
{
    int f = open("converted", O_RDONLY);
    unsigned char data[350000] = {'\0'};
    read(f, data, 350000);

    JRB machine_tree = ReadAllMachines(data);
    printf("Hosts all read in\n\n");
    
    Machine *m;
    int num_matches;
    Dllist dll_node;
    JRB jrb_node;
    char name[128];

    while(printf("Enter host name: "), fgets(name, 128, stdin))
    {
        name[strcspn(name, "\n")] = '\0';
        if (strlen(name) == 0) continue;
        num_matches = 0;
        jrb_traverse(jrb_node, machine_tree){
            if (strcmp(jrb_node->key.s, name) == 0)
            {
                num_matches++;
                m = (Machine *)jrb_node->val.v;
                char *ipstr = IPToString(m->ip);
                printf("%s: ", ipstr);
                free(ipstr);
                dll_traverse(dll_node, m->names)
                {
                    printf("%s ", dll_node->val.s);
                }
                printf("\n\n");
            }
        }
        if (num_matches == 0)
        {
            printf("no key %s\n\n", name);
        }
    }

    close(f);

    JRB machine_tree_nodupe = make_jrb();

    jrb_traverse(jrb_node, machine_tree)
    {
        m = (Machine *)jrb_node->val.v;
        if (jrb_find_int(machine_tree_nodupe, IPToInt(m->ip)) == NULL)
        {
            jrb_insert_int(machine_tree_nodupe, IPToInt(m->ip), new_jval_v(m));
        }
    }

    jrb_traverse(jrb_node, machine_tree_nodupe)
    {
        m = (Machine *)jrb_node->val.v;
        dll_traverse(dll_node, m->names)
        {
            free(dll_node->val.s);
        }
        free_dllist(m->names);
        free(m->ip);
        free(m);
    }

    jrb_free_tree(machine_tree);
    jrb_free_tree(machine_tree_nodupe);

    return 0;
}