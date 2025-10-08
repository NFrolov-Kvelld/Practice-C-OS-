#include <stdio.h>

void printBinary(unsigned int num) {
    for (int i = 31; i >= 0;i--) {
        printf("%d", (num >> i) & 1);
    }


}

void printNegativeBinary(int num) { //работает и для положительных, прикольно
    num = num * (-1);
    num = ~num;
    num = num + 1;


    for (int i = 31; i >= 0;i--) {
        printf("%d", (num >> i) & 1);
    }


}

int countBinary(unsigned int num) {
    int count = 0;
    for (int i = 31; i >= 0;i--) {
        if (((num >> i) & 1) == 1 ) {
            count++;
        }
    }
    return count;

}
//0 0000 0000 0001 0010 0000 0000 0000 0101 /// 00 0000 0000 0001 0010 0000 0000 0000 0101 // 0000 0000 0001 0010 0000 0000 0000 0101 // 0 0000 0000 0000 1001 0000 0000 0000 0101
void iniection(unsigned int num, unsigned int drop) {
    for (int i = 31; i >= 23;i--) {
        printf("%d", (num >> i) & 1);
    }
    for (int i = 7; i >= 0;i--) {
        printf("%d", (drop >> i) & 1);
    }
    for (int i = 15; i >= 0;i--) {
        printf("%d", (num >> i) & 1);
    }
}

int paritet(unsigned int num) {
    int count = 0;
    for (int i = 31; i >= 0;i--) {
        if (((num >> i) & 1) == 1 ) {
            count++;
        }
    }
    if (count%2 == 0) {
        return 1;
    }
    return 0;
}
void control(unsigned int num, unsigned int position) {
    if (position > 31) {
        printf("Error: position > 31\n");
        return;
    }
    printf("%s", "1 - Setup\n" "2 - Drop\n" "3 - Invertion\n");
    int choice;
    while (1) {
        scanf("%d", &choice);
        if (choice >= 1 && choice <= 3) {
            break;
        }
        printf("%s", "Incorrect number. Again\n");
    }
    switch (choice) {
        case 1: {
            printBinary(num);
            num = num | (1 << position);
            printf("%s","\n");
            printBinary(num);
            break;
        }
            case 2: {
            printBinary(num);
            num = num & ~(1 << position);
            printf("%s","\n");
            printBinary(num);
            break;
        }
        case 3: {
            printBinary(num);
            if ((num >> position) & 1 == 1) {
                num = num & ~(1 << position);
            }
            else {
                num = num | (1 << position);
            }
            printf("%s","\n");
            printBinary(num);
            break;
        }
    }
}

int main() {
    int choice;
    printf("%s", "1 - Easy 1\n" "2 - Easy 2\n" "3 - Easy 3\n" "4 - Easy 4\n" "5 - Medium 1\n" "6 - Medium 2\n" "7 - Medium 3\n");
    while (1) {
        scanf("%d", &choice);
        if (choice >= 1 && choice <= 7) {
            printf("%s","Enter number(s): ");
            break;
        }
        printf("%s", "Incorrect number. Again\n");
    }
    switch (choice) {
        case 1: {
            unsigned int number;
            scanf("%d", &number);
            printBinary(number);
            break;
        }
            case 2: {
            int number;
            scanf("%d", &number);
            printNegativeBinary(number);
            break;
        }
            case 3: {
            unsigned int number;
            scanf("%d", &number);
            printBinary(number);
            printf("\n");
            printf("%d", countBinary(number));
            break;
        }
            case 4: {
            unsigned int number, iniect;
            scanf("%d", &number);
            scanf("%d", &iniect);
            iniection(number, iniect);
            break;
        }
            case 5: {
            unsigned int number;
            scanf("%d", &number);
            printBinary(number);
            printf("\n");
            printf("%d", paritet(number));
            break;
        }
            case 6: {
            unsigned int number, position;
            scanf("%d", &number);
            scanf("%d", &position);
            control(number, position);
            break;
        }
            case 7: {
           printf("%s", "Surrender. Weakness.\n");
        }
    }
    return 0;
}