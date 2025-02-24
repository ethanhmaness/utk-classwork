#include <string.h>
#include <stdlib.h>
#include "dllist.h"
#include "fields.h"
#include "jrb.h"
#include "jval.h"
/* Struct for storing Person information */
typedef struct Person {
    char *name, sex;
    struct Person *father, *mother;
    Dllist children;
    int visited, printed;
} Person;
/* Instantiates a person */
Person *new_person() {
    Person *rv = malloc(sizeof(Person));
    rv->children = new_dllist();
    return rv;
}
/* Concatenates all fields of an inputstruct except the first into one string */
char *GetFamLineArg(IS is) {
    char *rv = malloc(256);
    for (int f = 1; f < is->NF; f++) rv = strcat(strcat(rv, is->fields[f]), " ");
    return rv;
}
/* Constructs a new person with the given name */
Person *NamedPerson(char name[]) {
    Person *p = new_person();
    p->name = name;
    return p;
}
/* Inserts Person keyed on their name and returns the new node. If the key already exists, returns that node. */
JRB InsertUniqueNamedPerson(JRB tree, Person *p) {
    JRB rv = jrb_find_str(tree, p->name);
    if (rv == NULL) rv = jrb_insert_str(tree, p->name, new_jval_v((void *)p));
    return rv;
}
/* Converts a char to a string representing the sex that the char abbreviates */
char *SexCharToStr(char sex_char)
{
    char *rv = malloc(256);
    if (sex_char == 'M') return rv = "Male";
    else if (sex_char == 'F') return rv = "Female";
    else return rv = "Unknown";
}
/* Sets the given Person's sex if not already set. Returns the Person's sex. */
char DefineSex(Person *p, char sex) { return (p->sex == 0) ? p->sex = sex : p->sex; }
/* Returns a pointer to the first person with a matching name, or NULL if not found */
Person *DllFindPerson(Dllist dll, char *name) {
    Dllist node;
    dll_traverse(node, dll) if (strcmp(name, ((Person *)node->val.v)->name) == 0) return (Person *)node->val.v;
    return NULL;
}
/* Adds child to parent's children list if they aren't already in it */
Person *AddChild(Person *parent, Person *child) {
    if (DllFindPerson(parent->children, child->name) == NULL) dll_append(parent->children, new_jval_v((void *)child));
    return parent;
}
/* Sets a Person's father or mother and adds them to the parent's children list */
Person *SetParent(Person *child, Person *parent, char sex) {
    if (sex == 'M' || sex == 'F'){
        DefineSex(parent, sex);
        if (sex == 'M') child->father = parent;
        else child->mother = parent;
        return AddChild(parent, child);
    } else return NULL;
}
/* Prints formatted info about a Person */
void PrintPerson(Person *p) {
    printf("%s\n  Sex: %s\n  Father: %s\n  Mother: %s\n", p->name, SexCharToStr(p->sex), (p->father != NULL) ? p->father->name : "Unknown", (p->mother != NULL) ? p->mother->name : "Unknown");
    if (dll_empty(p->children)) printf("  Children: None\n");
    else {
        printf("  Children:\n");
        Dllist node;
        dll_traverse(node, p->children) printf("    %s\n", ((Person *)node->val.v)->name);
    }
    printf("\n");
}
/* Prints all people in a family tree from the starting node in ancestry order. */
void PrintFamTree(JRB tree) {
    Dllist temp_node, toprint = new_dllist();
    JRB node;
    Person *p;
    jrb_traverse(node, tree) if (((Person *)node->val.v)->father == NULL && ((Person *)node->val.v)->mother == NULL) dll_append(toprint, new_jval_v((void *)(Person *)node->val.v));
    while (!dll_empty(toprint)) {
        p = (Person *)dll_first(toprint)->val.v;
        dll_delete_node(toprint->flink);
        if (p->printed == 0 && ((p->mother == NULL || p->mother->printed == 1) && (p->father == NULL || p->father->printed == 1))) {
            PrintPerson(p);
            p->printed = 1;
            dll_traverse(temp_node, p->children) dll_append(toprint, temp_node->val);
        }
    }
}
/* Returns 1 if there is a cycle in the Person's descendant tree, 0 otherwise*/
int HasCycle(Person *p) {
    if (p->visited == 1) return 0; /* I.e. we've processed this person before and he/she's ok */
    if (p->visited == 2) return 1; /* I.e. the graph is messed up */
    p->visited = 2;
    Dllist node;
    dll_traverse(node, p->children) if (HasCycle((Person *)node->val.v)) return 1;
    p->visited = 1;
    return 0;
}
/* --------- MAIN PROCESS --------- */
int main() {
    JRB people_tree = make_jrb(), temp_node, last_node;
    IS is = new_inputstruct(NULL);
    /* Read input lines */
    while (get_line(is) >= 0) {
        /* Skip empty line */
        if (is->NF == 0) continue;
        /* Add a person with the name specified by the PERSON arguments */
        if (strcmp(is->fields[0], "PERSON") == 0) last_node = InsertUniqueNamedPerson(people_tree, NamedPerson(GetFamLineArg(is)));
        /* Sets the sex of the most recently added Person */
        else if (strcmp(is->fields[0], "SEX") == 0) { if (DefineSex((Person *)last_node->val.v, GetFamLineArg(is)[0]) != GetFamLineArg(is)[0]) return fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line); }
        /* Sets the parent of the most recently added Person, creating a new Person if necessary */
        else if (strcmp(is->fields[0], "FATHER") == 0 || strcmp(is->fields[0], "MOTHER") == 0){
            char sex = (strcmp(is->fields[0], "FATHER") == 0) ? 'M' : 'F';
            Person *newparent = (Person *)InsertUniqueNamedPerson(people_tree, NamedPerson(GetFamLineArg(is)))->val.v;
            Person *currentparent = (sex == 'M') ? ((Person *)last_node->val.v)->father : ((Person *)last_node->val.v)->mother;
            if (currentparent != NULL && currentparent != newparent) return fprintf(stderr, (sex == 'M') ? "Bad input -- child with two fathers on line %d\n" : "Bad input -- child with two mothers on line %d\n", is->line);
            if (SetParent(((Person *)last_node->val.v), newparent, sex)->sex != sex) return fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
        }
        /* Defines the most recently added Person as the parent of the listed Person, creating a new Person if necessary */
        else if (strcmp(is->fields[0], "FATHER_OF") == 0 || strcmp(is->fields[0], "MOTHER_OF") == 0){
            char sex = (strcmp(is->fields[0], "FATHER_OF") == 0) ? 'M' : 'F';
            if (DefineSex(SetParent((Person *)InsertUniqueNamedPerson(people_tree, NamedPerson(GetFamLineArg(is)))->val.v, ((Person *)last_node->val.v), sex), sex) != sex) return fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
        }
    }
    /* Checks if there are any cycles, then prints tree if not */
    jrb_traverse(temp_node, people_tree) if (HasCycle(((Person *)temp_node->val.v))) return fprintf(stderr, "Bad input -- cycle in specification\n");
    PrintFamTree(people_tree);
}