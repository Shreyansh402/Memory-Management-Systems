#include <stdio.h>
#include <errno.h>
#include "2021MT10230mmu.h"

// Function to test my_malloc
void test_my_malloc()
{
    printf("Testing my_malloc...\n");

    // Allocate 1024 bytes
    void *ptr = my_malloc(1024);
    if (ptr == NULL)
    {
        printf("my_malloc failed: %s\n", strerror(errno));
    }
    else
    {
        printf("my_malloc succeeded\n");
    }

    // Free the allocated memory
    my_free(ptr);
}

// Function to test my_calloc
void test_my_calloc()
{
    printf("Testing my_calloc...\n");

    // Allocate an array of 256 elements, each 4 bytes
    void *ptr = my_calloc(256, 4);
    if (ptr == NULL)
    {
        printf("my_calloc failed: %s\n", strerror(errno));
    }
    else
    {
        printf("my_calloc succeeded\n");
    }

    // Free the allocated memory
    my_free(ptr);
}

// Function to test my_free
void test_my_free()
{
    printf("Testing my_free...\n");

    // Allocate 512 bytes
    void *ptr = my_malloc(512);
    if (ptr == NULL)
    {
        printf("my_malloc failed: %s\n", strerror(errno));
        return;
    }

    // Free the allocated memory
    my_free(ptr);
    printf("my_free succeeded\n");

    // Try to free NULL pointer
    my_free(NULL);
    if (errno == EINVAL)
    {
        printf("my_free correctly handled NULL pointer\n");
    }
    else
    {
        printf("my_free failed to handle NULL pointer\n");
    }
}

int main()
{
    // Run the tests
    test_my_malloc();
    test_my_calloc();
    test_my_free();

    return 0;
}