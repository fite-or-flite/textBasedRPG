//chris fite
//created 082819

//add failsafe for going out of array and going east to go west

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXITEMS 5
#define MAXINVITEMS 10
#define MAXMONSTERS 3

//defining structs
struct item {
    char name[20];
    char description[50];
    char tag[10];
};

struct person {
    char name[20];
    char description[50];
    char tag[10];
    int position;
    struct item inventory[MAXINVITEMS];
    char reply[50];
    };

struct room {
    char name[20];
    char description[50];
    char tag[10];
    struct item itemsPresent[MAXITEMS];
    int roomNum;
    char exits[4];
    struct person monstersPresent[MAXMONSTERS];
};

//initialize structs for gameplay
struct room map[16];
struct person player = {"player", "you are yourself", "you", 0};
struct person goblin = {"goblin", "a nasty, snarling goblin licks his lips", "goblin"};
struct item coin = {"coin", "a gold coin", "coin"};
struct item rock = {"rock", "a heavy rock", "rock"};
struct item sword = {"sword", "a badass swinging sword", "sword"};

char input[100];
int playerInvCtr = 0;

//declare game functions and commands
int getInput();
int parseCommands(char[]);

void init();
void initRooms();
void executeGo(char[]);
void executeLook(char[]);
void executeTake(char[]);
void executeInv();
void executeTalk(char[]);
void executeDrop(char[]);
void howManyItemsInRoom();

int main() {
    init();

    printf("Welcome to my first rpg.\n");
    while (getInput() && parseCommands(input));
    printf("\nbyebye\n");

    return 0;
}

int getInput(){
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
        else if (strcmp(verb, "go") == 0) {
            executeGo(noun);
            executeLook("around");
        }
        else if (strcmp(verb, "look") == 0){
            executeLook(noun);
        }
        else if (strcmp(verb, "take") == 0) {
            executeTake(noun);
        }
        else if (strcmp(verb, "inv") == 0) {
            executeInv();
        }
        else if (strcmp(verb, "talk") == 0) {
            executeTalk(noun);
        }
        else if (strcmp(verb, "drop") == 0) {
            executeDrop(noun);
        }
        else if (strcmp(verb, "hmi") == 0) {
            howManyItemsInRoom();
        }
        else {
            printf("\nI don't know how to %s", verb);
        }
    }
    return 1;
}

void init(){
    initRooms();
}

void initRooms() {

    //adding items to rooms
    map[1].itemsPresent[0] = sword;
    map[1].itemsPresent[1] = coin;
    map[1].itemsPresent[2] = coin;
    map[5].itemsPresent[0] = coin;
    map[2].itemsPresent[0] = rock;
    map[2].itemsPresent[1] = coin;
    map[2].itemsPresent[2] = coin;
    map[3].itemsPresent[0] = coin;
    map[10].itemsPresent[0] = coin;
    map[10].itemsPresent[1] = sword;
    map[10].itemsPresent[2] = rock;

    //adding monsters to rooms
    strcpy(goblin.reply, "the goblin growls at you");
    map[0].monstersPresent[0] = goblin;
    map[0].monstersPresent[1] = goblin;
    strcpy(goblin.reply, "the goblin winks at you");
    map[15].monstersPresent[0] = goblin;
    map[15].monstersPresent[1] = goblin;

    //main init loop
    for (int i = 0; i < 16; i++) {
        strcpy(map[i].name, "dungeon");
        strcpy(map[i].tag, "room");
        map[i].roomNum = i;
        switch(i) {
            //corners
            case 0:
                strcpy(map[i].description, "the southwest corner of the dungeon");
                strcpy(map[i].exits, "ne  ");
                break;
            case 3:
                strcpy(map[i].description, "the southeast corner of the dungeon");
                strcpy(map[i].exits, "nw  ");
                break;
            case 12:
                strcpy(map[i].description, "the northwest corner of the dungeon");
                strcpy(map[i].exits, "se  ");
                break;
            case 15:
                strcpy(map[i].description, "the northeast corner of the dungeon");
                strcpy(map[i].exits, "sw  ");
                break;
            //west wall
            case 4:
            case 8:
                strcpy(map[i].description, "a room along the west wall");
                strcpy(map[i].exits, "nse ");
                break;
            //east wall
            case 7:
            case 11:
                strcpy(map[i].description, "a room along the east wall");
                strcpy(map[i].exits, "nsw");
                break;
            // north,
            case 13:
            case 14:
                strcpy(map[i].description, "a room along the north wall");
                strcpy(map[i].exits, "sew ");
                break;
            //south walls,
            case 1:
            case 2:
                strcpy(map[i].description, "a room along the south wall");
                strcpy(map[i].exits, "new ");
                break;
            //middle
            default:
                strcpy(map[i].description, "a dank and damp dungeon");
                strcpy(map[i].exits, "nsew");
                break;
        };
    }
}

void executeInv() {
    printf("\n\nInventory:");
    for (int i = 0; i < MAXINVITEMS; i++) {
        printf("\n%d. %s", i, player.inventory[i].name);
    }
}

//add failsafe for going out of array and going east to go west
void executeGo(char noun[]) {
    if (noun != NULL) {
        if (strcmp(noun, "n") == 0)
            player.position = player.position + 4;
        else if (strcmp(noun, "s") == 0)
            player.position = player.position - 4;
        else if (strcmp(noun, "e") == 0)
            player.position = player.position + 1;
        else if (strcmp(noun, "w") == 0)
            player.position = player.position - 1;
        else printf("\nYou can't go there.");
        }
}

void executeLook(char noun[]){
    int found = 0;
    if (noun != NULL) {
        if (strcmp(noun, "around") == 0){
            found = 1;
            printf("\nRoom %d", map[player.position].roomNum);
            printf("\n%s", map[player.position].description);
            for (int j = 0; j < MAXITEMS; j++) {
                if (strcmp(map[player.position].itemsPresent[j].tag, "\0") != 0)
                    printf("\n%s", map[player.position].itemsPresent[j].tag);
            }
            for (int k = 0; k < MAXMONSTERS; k++) {
                if (strcmp(map[player.position].monstersPresent[k].tag, "\0") != 0)
                    printf("\n%s", map[player.position].monstersPresent[k].tag);
            }
            printf("\nExits: ");
            for (int i = 0; i < 4; i++) {
                printf("%c ", map[player.position].exits[i]);
            }
        }
        else if (strcmp(noun, "you") == 0) {
            found = 1;
            printf("\n%s", player.description);
        }
        else  {
            for (int i = 0; i < MAXITEMS; i++) {
                if ((strcmp(noun, map[player.position].itemsPresent[i].tag) == 0) && (found == 0)) {
                    printf("\n%s", map[player.position].itemsPresent[i].description);
                    found = 1;
                }
            }
            for (int i = 0; i < MAXMONSTERS; i++) {
                if ((strcmp(noun, map[player.position].monstersPresent[i].tag) == 0) && (found == 0)) {
                    printf("\n%s", map[player.position].monstersPresent[i].description);
                    found = 1;
                }
            }
            if (found == 0) {
                printf("\n%s isn't here.", noun);
            }

         }

    }

}

void executeTake(char noun[]) {
    int found = 0; //prob should be bool

    if (noun != NULL) {
        if (playerInvCtr < MAXINVITEMS){
            for (int i = 0; i < MAXITEMS; i++) {
                if ((strcmp(noun, map[player.position].itemsPresent[i].tag) == 0) && (found == 0)) {
                    found = 1;
                    printf("\nYou take %s", map[player.position].itemsPresent[i].tag);
                    player.inventory[playerInvCtr] = map[player.position].itemsPresent[i]; //add to inventory
                    playerInvCtr++;
                    for (int c = i; c < (MAXITEMS - 1); c++) { //delete from room
                        map[player.position].itemsPresent[c] = map[player.position].itemsPresent[c + 1];
                    }
                }
            }
            if (found == 0)
                printf("\n%s isn't here.", noun);
        }
        else printf("\nYou can't carry anymore.");
    }
}

void executeTalk(char noun[]) {
    int found = 0;
    if (noun!= NULL) {
        for (int i = 0; i < MAXMONSTERS; i++) {
            if ((strcmp(noun, map[player.position].monstersPresent[i].tag) == 0) && (found == 0)) {
                printf("\n%s", map[player.position].monstersPresent[i].reply);
                found = 1;
            }
        }
        if (found == 0) {
            printf("\n%s isn't here.", noun);
        }
    }
}

void executeDrop(char noun[]) {
    int found = 0;
    int maxItemsCheck = MAXITEMS; //this is so dumb

    if (noun != NULL) {
        for (int h = (MAXITEMS - 1); h >= 0; h--) {
            if (strcmp(map[player.position].itemsPresent[h].tag, "\0") == 0)
                maxItemsCheck = h;
        }
        if (maxItemsCheck < MAXITEMS){
            for (int i = 0; i < MAXINVITEMS; i++) {
                if ((strcmp(noun, player.inventory[i].name) == 0) && (found == 0)) {
                    found = 1;
                    printf("\nYou drop %s", player.inventory[i].name);
                    map[player.position].itemsPresent[maxItemsCheck] = player.inventory[i];
                    playerInvCtr--;
                    for (int c = i; c < MAXINVITEMS; c++) {
                        player.inventory[c] = player.inventory[c + 1];
                    }
                }
            }
            if (found == 0) {
                    printf("\nYou don't have %s", noun);
            }

        }
        else printf("\nThere is nowhere to put %s", noun);
    }
}

void howManyItemsInRoom() {

    int hmiir = 0;
    int hmmir = 0;
    executeLook("around");
    for (int i = 0; i < MAXITEMS; i++) {
        if (strcmp(map[player.position].itemsPresent[i].tag, "\0") != 0)
            hmiir++;
    }
    for (int j = 0; j < MAXMONSTERS; j++) {
        if (strcmp(map[player.position].monstersPresent[j].tag, "\0") != 0)
            hmmir++;
    }
    printf("\nNumber of items in room : %d", hmiir);
    printf("\nNumber of monsters in room: %d", hmmir);
}


