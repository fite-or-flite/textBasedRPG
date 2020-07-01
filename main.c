//chris fite
//created 082819


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define MAXITEMS 5
#define MAXINVITEMS 10
#define MAXMONSTERS 3

//defining structs
struct item {
    char name[20];
    char description[50];
    char tag[10];
    int modifier; //attack or armor bonus
    bool wielded;
};

//reorganize fields for better initialization
struct person {
    char name[20];
    char description[50];
    char tag[10];
    int position;
    char reply[50];
    int hp;
    int xp;
    int armorClass;
    int numHitDice; //number of d4 dice to roll for damage
    struct item inventory[MAXINVITEMS];
    struct item wielded[2];
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
struct person player = {"player", "you are yourself", "you", 0/*position*/};
struct person goblin = {"goblin", "a nasty, snarling goblin licks his lips", "goblin"};
struct item coin = {"coin", "a gold coin", "coin", 0, false};
struct item rock = {"rock", "a heavy rock", "rock", 0, false};
struct item sword = {"sword", "a badass swinging sword", "sword", 1/*modifier*/, false};
struct item sword2 = {"sword2", "a big, two-handed broad sword", "sword2", 2, false};

char input[100];
int playerInvCtr = 0;
int maxHP = 25;
//declare game functions and commands
int getInput();
int parseCommands(char[]);
int diceroller(int, int);
int pickTarget(char[]);
int pickItem(char[]);
int notDeadYet(struct person, int);
int pickItemFromInv(char[]);
int checkMap(char[]);

void init();
void initRooms();
void executeGo(char[]);
void executeLook(char[]);
void executeTake(char[]);
void executeInv();
void executeTalk(int);
void executeDrop(int);
void howManyItemsInRoom();
void executeAttack(int);
void counterAttack(int);
void executeRage(int);
void executeHelp();
void displayHP();
void executeHeal(int);
void displayXP();
void removeMonster(int);
void attackSequence(int);
void executeWield(int);
void executeUnwield(int);

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
            executeTalk(pickTarget(noun));
        }
        else if (strcmp(verb, "drop") == 0) {
            executeDrop(pickItemFromInv(noun));
        }
        else if (strcmp(verb, "hmi") == 0) {
            howManyItemsInRoom();
        }
        else if (strcmp(verb, "attack") == 0) {
            attackSequence(pickTarget(noun));
        }
        else if (strcmp(verb, "help") == 0) {
            executeHelp();
        }
        else if (strcmp(verb, "rage") == 0) {
            executeRage(pickTarget(noun));
        }
        else if (strcmp(verb, "hp") == 0) {
            displayHP();
            displayXP();
        }
        else if (strcmp(verb, "heal") == 0) {
            executeHeal(25);
        }
        else if (strcmp(verb, "wield") == 0) {
            executeWield(pickItemFromInv(noun));
        }
        else if (strcmp(verb, "unwield") == 0) {
            executeUnwield(pickItemFromInv(noun));
        }
        else if (strcmp(verb, "cool") == 0) {
            printf("\nYou say, Cool.");
        }
        else {
            printf("\nI don't know how to %s", verb);
        }
    }
    return 1;
}

int diceroller(int num, int sides) {
    int rando, total = 0;
    srand(time(NULL));
    //should i have an error for wtf input?
    //simulate rolls with rando numb
    for (int i = 0; i < num; i++) {
        rando = rand() % (sides) + 1;
        //total rolls
        total = total + rando;
        }
    return total;
}

void init(){
    initRooms();
}

void initRooms() {
    //initializing player and monsters
    player.hp = maxHP;
    goblin.hp = 15;
    player.xp = 0;
    goblin.xp = 5;
    player.armorClass = 10;
    goblin.armorClass = 10;
    player.numHitDice = 2;
    goblin.numHitDice = 1;
    goblin.inventory[0] = coin;
    strcpy(goblin.reply, "the goblin growls at you");
    //adding items to rooms
    map[1].itemsPresent[0] = sword;
    map[1].itemsPresent[1] = coin;
    map[2].itemsPresent[1] = sword2;
    map[10].itemsPresent[1] = sword;
    map[10].itemsPresent[2] = rock;
    //adding monsters to rooms
    map[0].monstersPresent[0] = goblin;
    map[0].monstersPresent[1] = goblin;
    map[1].monstersPresent[0] = goblin;
    map[1].monstersPresent[1] = goblin;
    map[15].monstersPresent[0] = goblin;
    map[15].monstersPresent[1] = goblin;
    //strcpy below doesnt work?
    strcpy(map[15].monstersPresent[0].reply, "the goblin winks at you");


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
        if ((strcmp(player.inventory[i].name, "\0") != 0) &&
            (player.inventory[i].wielded == true)) {
                    printf(" (wielded)");
                }
    }
}

void executeGo(char noun[]) {
    if (noun != NULL) {
        if ((strcmp(noun, "n") == 0) && (checkMap(noun) == 1))
            player.position = player.position + 4;
        else if ((strcmp(noun, "s") == 0) && (checkMap(noun) == 1))
            player.position = player.position - 4;
        else if ((strcmp(noun, "e") == 0) && (checkMap(noun) == 1))
            player.position = player.position + 1;
        else if ((strcmp(noun, "w") == 0) && (checkMap(noun) == 1))
            player.position = player.position - 1;
        else printf("\nYou can't go there.");
        }
}

void executeLook(char noun[]){
    bool found = false;
    if (noun != NULL) {
        if (strcmp(noun, "around") == 0){
            found = true;
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
            found = true;
            printf("\n%s", player.description);
        }
        else  {
            for (int i = 0; i < MAXITEMS; i++) {
                if ((strcmp(noun, map[player.position].itemsPresent[i].tag) == 0) && (found == false)) {
                    printf("\n%s", map[player.position].itemsPresent[i].description);
                    found = true;
                }
            }
            for (int i = 0; i < MAXMONSTERS; i++) {
                if ((strcmp(noun, map[player.position].monstersPresent[i].tag) == 0) && (found == false)) {
                    printf("\n%s", map[player.position].monstersPresent[i].description);
                    found = true;
                }
            }
            if (found == false) {
                printf("\n%s isn't here.", noun);
            }

         }

    }

}

void executeTake(char noun[]) {
    bool found = false;

    if (noun != NULL) {
        if (playerInvCtr < MAXINVITEMS){
            for (int i = 0; i < MAXITEMS; i++) {
                if ((strcmp(noun, map[player.position].itemsPresent[i].tag) == 0) && (found == false)) {
                    found = true;
                    printf("\nYou take %s", map[player.position].itemsPresent[i].tag);
                    player.inventory[playerInvCtr] = map[player.position].itemsPresent[i]; //add to inventory
                    playerInvCtr++;
                    for (int c = i; c < (MAXITEMS - 1); c++) { //delete from room
                        map[player.position].itemsPresent[c] = map[player.position].itemsPresent[c + 1];
                    }
                }
            }
            if (found == false)
                printf("\n%s isn't here.", noun);
        }
        else printf("\nYou can't carry anymore.");
    }
}

void executeTalk(int talkTo) {
    printf("\n%s", map[player.position].monstersPresent[talkTo].reply);
}

void executeDrop(int toDrop) {
    int maxItemsCheck = MAXITEMS; //this is so dumb
    for (int h = (MAXITEMS - 1); h >= 0; h--) {
        if (strcmp(map[player.position].itemsPresent[h].tag, "\0") == 0)
            maxItemsCheck = h;
    }
    if (maxItemsCheck < MAXITEMS){
        if(player.inventory[toDrop].wielded == true)
            executeUnwield(toDrop);
        printf("\nYou drop %s", player.inventory[toDrop].name);
        map[player.position].itemsPresent[maxItemsCheck] = player.inventory[toDrop];
        playerInvCtr--;
        for (int c = toDrop; c < MAXINVITEMS; c++)
            player.inventory[c] = player.inventory[c + 1];
    }

    else printf("\nThere is nowhere to put %s", player.inventory[toDrop].name);
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

void executeAttack(int toAttack) {
    int attack = 0;
    int attackRoll = diceroller(1, 20);

    if (toAttack >= 0) {
        if (attackRoll > map[player.position].monstersPresent[toAttack].armorClass) {
            attack = diceroller(player.numHitDice, 4);
            printf("\nYou hit for %d", attack);
            map[player.position].monstersPresent[toAttack].hp = map[player.position].monstersPresent[toAttack].hp - attack;
        }
        else printf("\nYou missed!");
        if (notDeadYet(map[player.position].monstersPresent[toAttack], toAttack) != 1)
            counterAttack(toAttack);
    }
    if (notDeadYet(player, 0) == -1)
        printf("\noh no!");

}

void counterAttack(int toAttack) {
    int attack = 0;
    int attackRoll = 0;

    attackRoll = diceroller(1, 20);
    if (attackRoll > player.armorClass) {
        attack = diceroller(map[player.position].monstersPresent[toAttack].numHitDice, 4);
        printf("\n%s hits you for %d", map[player.position].monstersPresent[toAttack].tag, attack);
        player.hp = player.hp - attack;
    }
    else printf("\n%s missed!", map[player.position].monstersPresent[toAttack].tag);
    printf("\n");
    displayHP();
    printf("\n%s: %d", map[player.position].monstersPresent[toAttack].tag, map[player.position].monstersPresent[toAttack].hp);
}

void executeRage(int toAttack){
    if (toAttack >= 0) {
        while ((map[player.position].monstersPresent[toAttack].hp > 0) &&
               (notDeadYet(player, 4) != -1)) {
                    executeAttack(toAttack);
                    sleep(1);
        }

    if (map[player.position].monstersPresent[toAttack].hp <= 0)
            removeMonster(toAttack);
    }
}

int pickTarget(char noun[]) {
    int found = 0;
    int toAttack = 0;
    if (noun != NULL) {
        for (int j = 0; j < MAXMONSTERS; j++) {
            if ((strcmp(map[player.position].monstersPresent[j].tag, noun) == 0) && (found == 0)) {
                toAttack = j;
                found = 1;
            }
        }
        if (found == 0) {
            printf("\n%s isn't here.", noun);
            return -1;
        }
    }
    return toAttack;
}

void executeHelp() {
    printf("\nHere's some ideas of what to do: ");
    printf("\ngo <direction>");
    printf("\nlook <thing>");
    printf("\nlook around");
    printf("\ntake <thing>");
    printf("\ndrop <thing>");
    printf("\nattack <person>");
    printf("\ntalk <person>");
    printf("\nrage <person>");
    printf("\nwield <weapon>");
    printf("\nheal");
    printf("\nquit");
}

void displayHP() {
    printf("\nHP: %d", player.hp);
}

int pickItem(char noun[]) {
    int found = 0;
    int toAttack = 0;
    if (noun != NULL) {
        for (int j = 0; j < MAXITEMS; j++) {
            if ((strcmp(map[player.position].itemsPresent[j].tag, noun) == 0) && (found == 0)) {
                toAttack = j;
                found = 1;
            }
        }
        if (found == 0) {
            printf("\n%s isn't here.", noun);
            return -1;
        }
    }
    return toAttack;
}

int notDeadYet(struct person checkMe, int monsterPosition) {
    int kill = 0;
    if (strcmp(checkMe.tag, "\0") != 0) { //is this even necessary?
        if (checkMe.hp <= 0) {
            printf("\n%s is dead.", checkMe.tag);
            if (strcmp(checkMe.name, "player") == 0)
                kill = -1;
            else kill = 1;
        }
    }
    return kill;
    //-1 player death
    //0 no ones dead
    //1 monster dead
}

void executeHeal(int points) { //need to pass struct person so i can heal others
    printf("\nA glowing light surrounds you.\nYou feel revived.");
    player.hp = player.hp + points;
    if (player.hp > maxHP)
        player.hp = maxHP;
    displayHP();
}

void displayXP() {
    printf("\nXP: %d", player.xp);
}

void removeMonster(int toAttack) {
    int maxItemsCheck = 0;
    player.xp = player.xp + map[player.position].monstersPresent[toAttack].xp;
    displayXP();
    //drop all items
    for (int h = (MAXITEMS - 1); h >= 0; h--) {
            if (strcmp(map[player.position].itemsPresent[h].tag, "\0") == 0)
                maxItemsCheck = h;
    }
    while (maxItemsCheck < MAXITEMS){
        for (int i = 0; i < MAXINVITEMS; i++) {
            if (strcmp(map[player.position].monstersPresent[toAttack].inventory[i].tag, "\0") != 0)
                printf("\n%s drops %s.", map[player.position].monstersPresent[toAttack].tag, map[player.position].monstersPresent[toAttack].inventory[i].tag);
            map[player.position].itemsPresent[maxItemsCheck] = map[player.position].monstersPresent[toAttack].inventory[i];
            maxItemsCheck++;
        }
    }
    //delete from room
    for (int c = toAttack; c < (MAXMONSTERS - 1); c++) {
        map[player.position].monstersPresent[c] = map[player.position].monstersPresent[c + 1];
    }
}

void attackSequence(int toAttack) {
    executeAttack(toAttack);
    if (map[player.position].monstersPresent[toAttack].hp <= 0)
        removeMonster(toAttack);
}

void executeWield(int toWield) {
    int counter = 0;
    for (int i = 0; i < MAXINVITEMS; i++) {
        if (player.inventory[i].wielded == true)
            counter++;
    }
    if (counter == 2)
        printf("\nYou're already wielding two items.\nYou must unwield one first.");
    else {
        player.inventory[toWield].wielded = true;
        printf("\nYou wield %s", player.inventory[toWield].tag);
        player.numHitDice = player.numHitDice + player.inventory[toWield].modifier;

    }
}

int pickItemFromInv(char noun[]) {
    int found = 0;
    int toUse = 0;
    if (noun != NULL) {
        for (int j = 0; j < MAXINVITEMS; j++) {
            if ((strcmp(player.inventory[j].tag, noun) == 0) && (found == 0)) {
                toUse = j;
                found = 1;
            }
        }
        if (found == 0) {
            printf("\n%s isn't here.", noun);
            return -1;
        }
    }
    return toUse;
}

void executeUnwield(int toUnwield) {
    if (player.inventory[toUnwield].wielded == false)
        printf("\nYou aren't wielding %s", player.inventory[toUnwield].tag);
    else {
        player.inventory[toUnwield].wielded = false;
        printf("\nYou unwield %s", player.inventory[toUnwield].tag);
        player.numHitDice = player.numHitDice - player.inventory[toUnwield].modifier;
    }
}

int checkMap(char noun[]) {
    //checks if direction to go is valid
    //east wall
    if (((player.position + 1) % 4 == 0)  && (strcmp(noun, "e") == 0))
        return -1;
    //west wall
    else if (((player.position %4) == 0) && (strcmp(noun, "w") == 0))
        return -1;
    //north wall
    else if ((player.position > 11) && (strcmp(noun, "n") == 0))
        return -1;
    //south wall
    else if ((player.position < 4) && (strcmp(noun, "s") == 0))
        return -1;
    else return 1;
}
