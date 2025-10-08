#include <stdio.h>
#include <stdlib.h>

int main()
{
int choice;
printf("%s","1 - Easy 1\n" "2 - Easy 2\n" "3 - Easy 3\n" "4 - Easy 4\n");
scanf("%d",&choice);
switch(choice) {
case 1: {
    const int size;
    printf("%s","Enter the size of the matrix\n");
    scanf("%d",&size);
    for(int i=1;i<=size*size;i++) {
        printf("%d",i);
        printf("%s","\t");
        if (i % size == 0) {
            printf("\n");
        }
    }
    break;
}
        case 2: {
    const int size;
    printf("%s","Enter the size of the array\n");
    scanf("%d",&size);
    int arr[size];
    for (int i =0;i<size;i++) {
        scanf("%d",&arr[i]);
    }
    for (int i = size-1; i >= 0; i--) {
        printf("%d",arr[i]);
        printf("%s","\t");
    }
    break;
}
        case 3: {
    int size;
    printf("%s","Enter the size of the matrix\n");
    scanf("%d",&size);
    int arr[size][size];
    int diag = 1;
    for (int i = 1; i<=size; i++) {
        for (int j = 0; j<size - i; j++) {
            arr[i-1][j] = 0;
            printf("%d", 0 );
            printf("%s","\t");
        }
        for (int j=size - i - 1;j<size-1;j++) {
            arr[i-1][j] = 0;
            printf("%d", 1 );
            printf("%s","\t");
        }
        printf("%s", "\n");
    }
    break;
}
        case 4: {
    const int size;
    printf("%s","Enter the size of the matrix\n");
    scanf("%d",&size);
    int **arr = (int **)calloc(size, sizeof(int *)); //почему-то при не динамическом массиве выдает мусор в конце
    for (int i = 0; i < size; i++) {
        arr[i] = (int *)calloc(size, sizeof(int));
    }
    int num = 1;
    int left = 0, right = size - 1,top = 0, bottom = size - 1;
    while (left <= right && top <= bottom) {
        for (int i = left; i <= right; i++) {
            arr[top][i] = num++;
        }
        top++;
        for (int i = top; i <= bottom; i++) {
            arr[i][right] = num++;
        }
        right--;
        if (top <= bottom) {
            for (int i = right; i >= left; i--) {
                arr[bottom][i] = num++;
            }
            bottom--;
        }
        if (left <= right) {
            for (int i = bottom; i >= top; i--) {
                arr[i][left] = num++;
            }
            left++;
        }
    }
    for (int i = 0; i<=size; i++) {
        for (int j = 0; j<size; j++) {
            printf("%d",arr[i][j]);
            printf("%s","\t");
        }
        printf("\n");
    }
    break;
}
        case 5: {}
}
return 0;
}