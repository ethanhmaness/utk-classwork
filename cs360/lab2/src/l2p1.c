#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jrb.h"
#include "dllist.h"

typedef struct Machine
{
    Dllist names;
    unsigned char *ip;
} Machine;

Machine *new_machine()
{
    Machine *m = malloc(sizeof(Machine));
    m->names = new_dllist();
    m->ip = malloc(sizeof(unsigned char) * 4);
    return m;
}

char *IPToString(unsigned char *ip)
{
    char *rv = malloc(sizeof(char) * 16);
    sprintf(rv, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return rv;
}

int IPToInt(unsigned char *ip)
{
    return (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
}

char *StringUntil(char *str, const char *delim)
{
    char *rv = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(rv, str);
    rv[strcspn(rv, ".")] = '\0';
    return rv;
}

Machine *ReadMachine(FILE *f)
{
    int num_names;
    unsigned char buffer[4];
    char *name;
    char c;

    Machine *m;
    unsigned char* ip = malloc(sizeof(unsigned char) * 4);
    Dllist names = new_dllist();

    if (!fread(ip, 1, 4, f)) return free(ip), free(names), NULL;

    if (!fread(buffer, 1, 4, f)) return free(ip), free(names), NULL;
    num_names = IPToInt(buffer);

    for (int i = 0; i < num_names; i++)
    {
        name = malloc(sizeof(char) * 128);
        strcpy(name, "");
        do
        {
            if (!fread(&c, sizeof(char), 1, f)) return free(ip), free(names), NULL;
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

JRB ReadAllMachines(FILE *f)
{
    JRB machine_tree = make_jrb();
    Machine *m;
    Dllist node;
    while (m = ReadMachine(f), m != NULL)
    {
        dll_traverse(node, m->names)
        {
            jrb_insert_str(machine_tree, node->val.s, new_jval_v(m));
        }
    }
    free(m);
    return machine_tree;
}

int main()
{
    FILE *f = fopen("converted", "rb");
    JRB machine_tree = ReadAllMachines(f);
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
                printf("%s: ", IPToString(m->ip));
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

    fclose(f);

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