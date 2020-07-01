#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item {
        char name[20];
        char description[50];
        char tag[20];
        int weight;
//        struct item inventory[];
    };

char input[100];
struct item thing1 = {"boulder", "It is a really big rock yo", "rock", 100};
struct item thing2 = {"stick", "This is a small branch.", "branch", 50};
struct item room1 = {"room", "A round room with no exits.", "room", 100000};

int getInput();
int parseCommands(char command[50]);
void executeExamine(char thing[50]);

int main()
{
    printf("Welcome to my first rpg.\n");
    while (getInput() && parseCommands(input));
    printf("\nbyebye\n");

    return 0;
}

int getInput() {
    printf("\n--> ");
    return fgets(input, sizeof(input), stdin) != NULL;
}

int parseCommands(char command[]) {
    char *verb = strtok(command, " \n");
    char *noun = strtok(NULL, " \n");

    if (verb != NULL) {
        if (strcmp(verb, "quit") == 0) {
            return 0;
        }
        else if (strcmp(verb, "examine") == 0){
            executeExamine(noun);
            return 1;
        }
        else {
            printf("\nI don't know how to %s", verb);
        }
    }

    return 1;
}

void executeExamine(char thing[50]) {
    if (thing != NULL){
        if (strcmp(thing, thing1.tag) == 0) {
            printf("\nYou see a %s.", thing1.name);
            printf("\n%s", thing1.description);
        }
        else if (strcmp(thing, thing2.tag) == 0) {
            printf("\nYou see a %s.", thing2.name);
            printf("\n%s", thing2.description);
        }
        else if (strcmp(thing, room1.tag)== 0) {
            printf("\nYou are in a %s.", room1.name);
            printf("\n%s", room1.description);
            printf("\nThere is a %s and %s here.", thing1.tag, thing2.tag);
        }
        else printf("\nThere is no %s here", thing);
    }

}
