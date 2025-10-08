#include <stdio.h>
#include <stdlib.h>

void printBinary(unsigned int num) {
    for (int i = 31; i >= 0;i--) {
        printf("%d", (num >> i) & 1);
    }


}

int main()
{
int choice;
printf("%s","1 - Easy 1\n" "2 - Easy 2\n" "3 - Easy 3\n" "4 - Easy 4\n");
scanf("%d",&choice);
switch(choice) {
    case 1: {
        int num;
        char new_byte;
        printf("Enter num: ");
        scanf("%d", &num);
        printf("imiection number ");
        scanf("%hhu", &new_byte);
        unsigned char *byte = (unsigned char *)&num;
        byte[2] = new_byte;
        printf("new number: %d\n", num);
        printBinary(num);
        break;
    }
        case 2: {
        float x = 5.0;
        printf("x = %f\n", x);
        float y = 6.0;
        printf("y = %f\n", y);
        float *xp = &y; //TODO *xp = &y --> *xp = &x
        float *yp = &y;
        printf("Result: %f\n", *xp + *yp);
        break;
    }
        case 3: {
        int arr[10] = {1,2,3,4,5,6,7,8,9,10};
        int *pointer = &arr;
        for (int i = 0; i < 10; i++) {
            printf("%d\t", *pointer);
            pointer++;
        }
        break;
    }
        case 4: {  //cделано не в функции, но основной смысл есть
        char string[256];
        char substr[256];
        int check = 0;
        printf("Enter string: \n");
        scanf("%s", string);
        printf("Enter substring: \n");
        scanf("%s", substr);
        char *pointerBig = &string[0], *pointerSmall = &substr[0];
        printf("%c",*pointerBig);
        for (int i = 0;i<256;i++) {
            if (pointerBig[0] == pointerSmall[0]) {
                check = 1;
                pointerSmall++;
                if (*pointerSmall == '\0') {
                    printf("%p",*pointerBig);
                    printf("\n");
                    break;
                }
            }
            else if (check == 1) {
                check = 0;
                pointerSmall = &substr[0];
            }
            pointerBig++;
        }
        printf("%s", "NOWAY");
    }
}
return 0;
}