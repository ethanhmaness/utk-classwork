#include "mymalloc.h"

// C
#include <stdio.h>
#include <unistd.h>

/*
 * ------------------------------------------------
 *                  Internal stuff                 
 * ------------------------------------------------
 */

/*
 * Free list node
 * size includes this 8 bytes of metadata
 * User gets space at this + 8
 */
typedef struct __flnode
{
    unsigned int size;
    struct __flnode *next;
} flnode;

/* Pointer to first free list node */
flnode *head = NULL;

/* 
 * creates an flnode at the given memory addr with the given size and next initialized to NULL
 */
flnode *new_flnode_at(void *addr, unsigned int size)
{
    flnode *rv = addr;
    rv->size = size,
    rv->next = NULL;
    return rv;
}

/* 
 * allocs a single flnode with sbrk
 * should only be used with large sizes
 */
flnode *flnode_alloc(size_t size)
{
    return new_flnode_at((void *)sbrk(size), size);
}

/*
 * Adds a new block of memory to the heap with an flnode of size 'size' at the front
 */
flnode *new_block(size_t size)
{
    // If bigger than minimum sbrk size, alloc a single flnode
    if (size > 8192) return flnode_alloc(size);

    // Otherwise, alloc two, then free the second one and return the first
    flnode *a = new_flnode_at((void *)sbrk(8192), size);
    flnode *b = new_flnode_at((void *)a + size, 8192 - size);
    my_free((void *)b + 8);
    return a;
}

/* 
 * ------------------------------------------------
 *                Library procedures               
 * ------------------------------------------------
 */

/*
 * Returns a pointer to a block of memory in the heap for the user
 * Behind the scenes, this will find a large enough free block in the heap and remove it from the list of free blocks
 * If no block is found, a new block is created by extending the heap
 */
void* my_malloc(size_t size) {

    // calc total node size

    int padding = (8 - (size % 8)) % 8;
    size_t total_bytes = size + padding + 8;

    // search for existing sufficiently large block

    flnode *n_found;
    for (n_found = head; n_found != NULL && n_found->size < total_bytes; n_found = n_found->next);

    // if no block found make new one
    
    if (n_found == NULL)
    {
        n_found = new_block(total_bytes);
        return (void *)n_found + 8;
    } 

    // if block is too big, split and return latter part

    if (n_found->size > total_bytes + 8)
    {
        n_found->size -= total_bytes;
        flnode *n_new = new_flnode_at((void *)n_found + n_found->size, total_bytes);
        return (void *)n_new + 8;
    }

    // otherwise remove existing block from free list and return

    if (n_found == head) head = n_found->next;
    else
    {
        flnode *n_prev;
        for (n_prev = head; n_prev->next != n_found; n_prev = n_prev->next);
        n_prev->next = n_found->next;
    }
    n_found->next = NULL;
    return (void *)n_found + 8;
}

/*
 * Links the block of memory at ptr into the free memory list using the prefixed metadata
 */
void my_free(void* ptr) {

    // flnode info is stored in 8 byte prefix to ptr
    flnode *n = ptr - 8;

    // if no free memory list exists, start it
    if (!head) head = n;

    // if this memory is before the list, prepend it
    else if (n < head)
    {
        n->next = head,
        head = n;
    }

    // otherwise, find the correct location to insert this memory
    else
    {
        flnode *n_prev;

        for (n_prev = head; n_prev->next != NULL && n_prev->next < n; n_prev = n_prev->next);

        n->next = n_prev->next,
        n_prev->next = n;
    }
}

/*
 * Gets the first free list node
 */
void* free_list_begin() {
    return head;
}

/*
 * Gets the free list node after 'node'
 */
void* free_list_next(void* node) {
    return ((flnode *)node)->next;
}

/*
 * Defragments the free list nodes by finding adjacent nodes and merging them together
 */
void coalesce_free_list() {

    flnode *n;

    // for each node in the list, consume all adjacent nodes
    for (n = head; n != NULL; n = n->next)
    {
        while (n->next == (void *)n + n->size)
        {
            n->size += n->next->size,
            n->next = n->next->next;
        }
    }
}
