#include <stdio.h>

int main()
{
    int var = 10;
    int *ptr;

    ptr = &var;

    printf("Value of var: %d\n", var);
    printf("Address of var: %p\n", (void *)&var);
    printf("Value of ptr: %p\n", (void *)ptr);
    printf("Value pointed to by ptr: %d\n", *ptr);

    *ptr = 20; // Changing the value of var through the pointer
    printf("New value of var: %d\n", var);

    return 0;
}

/*** end of file ***/
