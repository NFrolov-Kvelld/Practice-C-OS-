#include <stdio.h>
#include <stdlib.h>

struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

void addAb(struct abonent *book, int *position) {
    if (*position == 99) {
        printf("%s", "No space\n");
        return;
    }
    position++;
    printf("%s", "Enter: Name>>Second name>>Mobile number\n");
    scanf("%s", &book[*position].name);
    scanf("%s",&book[*position].second_name);
    scanf("%s",&book[*position].tel);
}
void deleteAb(struct abonent *book, int *position, int deln) {
    if (position < 0) {
        printf("%s", "No abonents yet\n");
        return;
    }
    if (deln == *position) {
        for (int j = 0; j < 10; j++) {
            book[*position].name[j] = '0';
            book[*position].second_name[j] = '0';
            book[*position].tel[j] = '0';
        }
        (*position)--;
        return;
    }
    for (int i = deln; deln < *position; i++) {
        for (int j = 0;j<10;j++) {
            book[i].name[j] = book[i+1].name[j];
            book[i].second_name[j] = book[i+1].second_name[j];
            book[i].tel[j] = book[i+1].tel[j];
        }
    }
    (*position)--;
}
void findAb(struct abonent *book, int *position, char *mask) {
    for (int i = 0; i <= *position; i++) {
        for (int j = 0;j<10;j++) {
            if ( book[i].name[j] != mask[j]) {
                continue;
            }
            if (j == 9) {
                for (int k = 0;book[i].name[k]!='\0';k++) {
                    printf("%c",book[i].name[k]);
                }
                printf("\n");
                for (int k = 0;book[i].name[k]!='\0';k++) {
                    printf("%c",book[i].second_name[k]);
                }
                printf("\n");
                for (int k = 0;book[i].name[k]!='\0';k++) {
                    printf("%c",book[i].tel[k]);
                }
                printf("\n");
            }
        }
    }
}
void displayAb(struct abonent *book, int *position) {
    for (int i = 0; i <= *position; i++) {
        for (int k = 0;book[i].name[k]!='\0';k++) {
            printf("%c",book[i].name[k]);
        }
        printf("\n");
        for (int k = 0;book[i].name[k]!='\0';k++) {
            printf("%c",book[i].second_name[k]);
        }
        printf("\n");
        for (int k = 0;book[i].name[k]!='\0';k++) {
            printf("%c",book[i].tel[k]);
        }
        printf("\n");
        printf("%s", "|--------|\n");
    }
}
int main() {
    int choice;
    printf("%s","1 - Easy 1\n" "2 - Medium \n");
    scanf("%d",&choice);
    int c = 1;
    switch(choice) {
        case 1: {
            int position = -1;
            struct abonent book[100];
            while (c == 1) {
                int choice1;
                printf("%s","1 - Add\n" "2 - Delete\n" "3 - Find (name)\n" "4 - List\n" "5 - Exit\n");
                scanf("%d",&choice1);
                switch(choice1) {
                    case 1: {
                        addAb(book, &position); //TODO: почему-то не записвает, хотя функция точна копия рабочей.
                        break;
                    }
                    case 2: {
                        if (position < 0) {
                            printf("%s", "No abonents yet\n");
                            break;
                        }
                        printf("%s", "Enter: position\n");
                        unsigned int deln = 0;
                        while (1) {
                            scanf("%d",&deln);
                            if (deln > position) {
                                printf("%s", "wrong number. Enter again\n");
                            }
                            else {
                                break;
                            }
                            deleteAb(book, &position, deln);
                        }
                        case 3: {
                            printf("%s", "Enter name\n");
                            char sname[10];
                            scanf("%s",&sname);
                            findAb(book, &position, sname);
                            break;
                        }
                        case 4: {
                            displayAb(book, &position);
                        }
                        case 5: {
                            c = 0;
                            break;
                        }
                    }
                }
            }
        }
            case 2: {

        }
    }
    exit(EXIT_SUCCESS);
}