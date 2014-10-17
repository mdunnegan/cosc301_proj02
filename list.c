#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

void list_insert(const char *str, struct node**head){
        struct node *new = malloc(sizeof(struct node));
        strcpy(new->name, str);
        if (head==NULL){
                new->next = NULL;
                *head = new;
                return;
        }
        else{
                new->next = *head;
                *head = new;
                return;
        }
}

void list_print(struct node *list) {
        //cycle through each element and print it out...
        while (list != NULL) {
                printf("%s\n", list->name);
                list = list->next;
        }
}

void list_delete(struct node *list) {
        //cycle through each element and delete it.
        while (list != NULL) {
                struct node *tmp = list;
                list = list->next;
                free(tmp);
        }
}