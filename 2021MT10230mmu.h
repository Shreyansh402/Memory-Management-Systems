#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
// Include your Headers below
#include <unistd.h>
#include <string.h>
#include <errno.h>

// You are not allowed to use the function malloc and calloc directly .

// Data structure to manage the memory
typedef struct MemBlock
{
    size_t size;
    struct MemBlock *next;
    int is_free;
} MemBlock;

// Global variable to store the head of the free list
static MemBlock *head = NULL;

// Function to request memory using sbrk
void *request_memory(size_t size)
{
    // void *block = sbrk(size);
    // if (block == (void *)-1)
    // {
    //     return NULL; // sbrk failed
    // }
    void *block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL; // mmap failed
    }
    return block;
}

// Function to allocate memory using mmap
void *my_malloc(size_t size)
{
    // Your implementation of my_malloc goes here
    if (size == 0)
    {
        errno = EINVAL;
        return NULL;
    }

    MemBlock *current = head;
    MemBlock *previous = NULL;

    // First-fit memory allocation strategy
    while (current)
    {
        if (current->is_free && current->size >= size)
        {
            current->is_free = 0;
            return (void *)(current + 1); // Return memory right after the block metadata
        }
        previous = current;
        current = current->next;
    }

    // If no suitable block found, request memory from OS
    size_t total_size = sizeof(MemBlock) + size;
    MemBlock *new_block = (MemBlock *)request_memory(total_size);
    if (!new_block)
    {
        errno = ENOMEM;
        return NULL; // Memory request failed
    }

    // Initialize new memory block
    new_block->size = size;
    new_block->is_free = 0;
    new_block->next = NULL;

    // Add the new block to the free list
    if (previous)
    {
        previous->next = new_block;
    }
    else
    {
        head = new_block;
    }

    return (void *)(new_block + 1);
}

// Function to allocate and initialize memory to zero using mmap
void *my_calloc(size_t nelem, size_t size)
{
    if (nelem == 0 || size == 0)
    {
        errno = EINVAL; // Invalid size argument
        return NULL;
    }

    // Your implementation of my_calloc goes here
    size_t total_size = nelem * size;

    // Check for overflow
    if (nelem != 0 && total_size / nelem != size)
    {
        errno = ENOMEM;
        return NULL;
    }

    void *ptr = my_malloc(total_size);
    if (ptr)
    {
        memset(ptr, 0, total_size); // Initialize allocated memory to zero
    }

    return ptr;
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void *ptr)
{
    // Your implementation of my_free goes here
    if (!ptr)
    {
        errno = EINVAL; // Invalid pointer
        return;
    }

    // Get the block metadata before the memory
    MemBlock *block = (MemBlock *)ptr - 1;
    block->is_free = 1;

    // Coalesce adjacent free blocks

    MemBlock *current = head;

    while (current)
    {
        if (current->is_free && current->next && current->next->is_free)
        {
            current->size += sizeof(MemBlock) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }

    // Check if the last block in the list is free, and release memory back to the OS
    current = head;
    MemBlock *last_block = NULL;

    // Traverse to find the last block
    while (current && current->next)
    {
        last_block = current;
        current = current->next;
    }

    if (current && current->is_free)
    {
        // If the last block is free, release it back to the OS using brk()
        if (head == current)
        {
            // If it's the only block, set the head to NULL
            head = NULL;
        }
        else
        {
            // Otherwise, remove the last block from the free list
            last_block->next = NULL;
        }

        // if (((void *)current + sizeof(MemBlock) + current->size) == sbrk(0))
        // {
        //     // If the last block is at the end of the heap, release it using brk()
        //     if (brk(current) != 0)
        //     {
        //         errno = ENOMEM; // Failed to release memory
        //     }
        // }
        // Use munmap to release the memory
        if (munmap(current, sizeof(MemBlock) + current->size) != 0)
        {
            errno = ENOMEM; // Failed to release memory
        }
    }
}