#include <stdio.h>

int main()
{
    int arr[] = {10, 20, 30, 40, 50};
    int *ptr = arr; // Pointer to the first element of the array

    for (int i = 0; i < 5; i++)
    {
        printf("arr[%d] = %d ", i, arr[i]);
        printf("*(ptr + %d) = %d\n", i, *(ptr + i));
    }

    return 0;
}

/*** end of file ***/
