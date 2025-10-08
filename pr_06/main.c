#include <stdio.h>
#include <string.h>

#define MAX_ABONENTS 100
#define NAME_LENGTH 10

struct abonent {
    char name[NAME_LENGTH];
    char second_name[NAME_LENGTH];
    char tel[NAME_LENGTH];
};

void add_abonent(struct abonent book[], int *position) {
    if (*position == MAX_ABONENTS - 1) {
        printf("No space\n");
        return;
    }
    (*position)++;
    printf("Enter: Name>>Second name>>Mobile number\n");
    scanf("%s", book[*position].name);
    scanf("%s", book[*position].second_name);
    scanf("%s", book[*position].tel);
}

void delete_abonent(struct abonent book[], int *position) {
    if (*position < 0) {
        printf("No abonents yet\n");
        return;
    }

    printf("Enter position to delete: ");
    unsigned int deln = 0;
    while (1) {
        scanf("%d", &deln);
        if (deln > *position) {
            printf("Wrong number. Enter again\n");
        } else {
            break;
        }
    }

    if (deln == *position) {
        memset(&book[*position], 0, sizeof(struct abonent));
    } else {
        for (int i = deln; i < *position; i++) {
            memcpy(&book[i], &book[i+1], sizeof(struct abonent));
        }
    }
    (*position)--;
}

void find_abonent(struct abonent book[], int position) {
    printf("Enter name to search: ");
    char sname[NAME_LENGTH];
    scanf("%s", sname);

    for (int i = 0; i <= position; i++) {
        if (strcmp(book[i].name, sname) == 0) {
            printf("%s\n%s\n%s\n",
                   book[i].name,
                   book[i].second_name,
                   book[i].tel);
        }
    }
}

void list_abonents(struct abonent book[], int position) {
    for (int i = 0; i <= position; i++) {
        printf("%s\n%s\n%s\n",
               book[i].name,
               book[i].second_name,
               book[i].tel);
        printf("|--------|\n");
    }
}

int main() {
    int position = -1;
    struct abonent book[MAX_ABONENTS];
    int c = 1;

    while (c == 1) {
        int choice1;
        printf("1 - Add\n2 - Delete\n3 - Find (name)\n4 - List\n5 - Exit\n");
        scanf("%d", &choice1);

        switch(choice1) {
            case 1:
                add_abonent(book, &position);
                break;
            case 2:
                delete_abonent(book, &position);
                break;
            case 3:
                find_abonent(book, position);
                break;
            case 4:
                list_abonents(book, position);
                break;
            case 5:
                c = 0;
                break;
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}