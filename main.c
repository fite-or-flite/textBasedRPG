//chris fite
//created 110719
//edited 111119

//need to add equip/un- for armor
//idea to popuplate goblins in dungeon:
//  rand num for position
//  loop to create rand num of goblins, naming each goblinX


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define MAXITEMS 5
#define MAXINVITEMS 10
#define MAXMONSTERS 3

struct item {
    char name[15];
    char description[50];
    int attackBonus; // +numHitDice
    int defenseBonus; // +ac
    bool wielded;
};

struct person {
    char name[15];
    char description[50];
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
    int roomNum;
    char exits[5]; //nsew/0
    struct person monstersPresent[MAXMONSTERS];
    struct item itemsPresent[MAXITEMS];
};

//initialize structs for gameplay
struct room map[16];
//person: name, descr, pos, reply, hp, xp, ac, hitdice, inv[], wield[]
struct person player = {"player", "you are yourself", 0, "what?", 20, 0, 5, 1};
struct person goblin = {"goblin", "A gross looking goblin", 0, "The goblin snarls at you.", 10, 2, 5, 1};
struct person blank = {0}; //weird workaround but ok --for removemonster
//item: name, desc, attbonus, defbonus, wielded
struct item coin = {"coin", "A shiny coin with somebody's face on one side.", 0, 0, false};
struct item sword = {"sword", "A sharp looking sword.", 1, 0, false};
struct item rock = {"rock", "A medium sized rock.", 0, 0, false};
struct item broadsword = {"broadsword", "A big badass swinging sword.", 2, 0, false};
struct item shield = {"shield", "A round, wooden shield", 0, 2, false};
struct item helm = {"helm", "A round metal helmet", 0, 1, false};
struct item blankItem = {0}; //wierd work around for removeitem

char input[100];

//declare game functions and commands
int getInput();
int parseCommands(char[]);
int diceroller(int, int);
int pickTarget (char[]);
int pickItem (char[]);
int numItemsInInventory();
int numMonstersInRoom();
int numItemsInRoom();
int pickFromInventory(char[]);

void init();
void executeGo(char[]);
void executeLook(char[]);
void executeTalk(char[]);
void executeAttack(char[]);
void showHP();
void showXP();
void showInventory();
void executeHeal();
void removeMonsterFromRoom(int);
void addToInventory(char[]);
void removeFromInventory(char[]);
void executeWield(char[]);
void executeUnwield(char[]);
void executeHelp();
void showstats();

int main () {

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
        else if (strcmp(verb, "talk") == 0) {
            executeTalk(noun);
        }
        else if (strcmp(verb, "attack") == 0) {
            executeAttack(noun);
        }
        else if (strcmp(verb, "hp") == 0) {
            showHP();
            showXP();
        }
        else if (strcmp(verb, "heal") == 0) {
            executeHeal();
        }
       else if (strcmp(verb, "take") == 0) {
            addToInventory(noun);
        }
        else if (strcmp(verb, "inv") == 0) {
            showInventory();
       }
        else if (strcmp(verb, "drop") == 0) {
            removeFromInventory(noun);
        }
        else if (strcmp(verb, "wield") == 0) {
            executeWield(noun);
        }
        else if (strcmp(verb, "unwield") == 0) {
            executeUnwield(noun);
        }
        else if (strcmp(verb, "help") == 0) {
            executeHelp();
        }
        else if (strcmp(verb, "cool") == 0) {
            printf("\nYou say, Cool.");
        }
        else if (strcmp(verb, "stats") == 0) {
            showstats();
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

void init() {
    //manually adding items, monsters for now; will randomize
    for (int i = 0; i < 16; i = i + 3) {
        map[i].itemsPresent[0] = coin;
        if (i % 2 == 0)
            map[i].itemsPresent[1] = coin;
    }
    for (int j = 0; j < 16; j = j + 5) {
        map[j].monstersPresent[0] = goblin;
        if (j % 2 != 0) {
           // map[j].monstersPresent[1] = goblin;
            strcpy(map[j].monstersPresent[0].description, "This gross little guy has seen some shit.");
            strcpy(map[j].monstersPresent[0].reply, "The goblin hums to itself.");
        }
    }
    map[1].monstersPresent[2] = goblin;
    map[0].monstersPresent[2] = goblin;
    map[3].itemsPresent[2] = sword;
    map[10].itemsPresent[2] = broadsword;
    map[2].itemsPresent[2] = shield;

    //room: name, desc, roomnum, exits[], monsPres[], itemsPres[]
    //main init loop
    for (int i = 0; i < 16; i++) {
        strcpy(map[i].name, "dungeon");
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

void executeGo(char noun[]) {
    if (noun != NULL) {
        if ((strcmp(noun, "n") == 0) && (player.position < 12))
            player.position = player.position + 4;
        else if ((strcmp(noun, "s") == 0) && (player.position > 3))
            player.position = player.position - 4;
        else if ((strcmp(noun, "e") == 0) && ((player.position + 1) % 4 != 0))
            player.position = player.position + 1;
        else if ((strcmp(noun, "w") == 0) && (player.position % 4 != 0))
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
                if (strcmp(map[player.position].itemsPresent[j].name, "\0") != 0)
                    printf("\n%s", map[player.position].itemsPresent[j].name);
            }
            for (int k = 0; k < MAXMONSTERS; k++) {
                if (strcmp(map[player.position].monstersPresent[k].name, "\0") != 0)
                    printf("\n%s", map[player.position].monstersPresent[k].name);
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
            if (pickItem(noun) >= 0) {
                printf("\n%s", map[player.position].itemsPresent[pickItem(noun)].description);
                found = true;
            }
            else if (pickTarget(noun) >= 0){
                printf("\n%s", map[player.position].monstersPresent[pickTarget(noun)].description);
                found = true;
                }
            if (found == false) {
                printf("\n%s isn't here.", noun);
            }

         }

    }

}

int pickTarget (char noun[]) {
    bool found = false;
    int toAttack = 0;

    if (noun != NULL) {
        for (int i = 0; i < MAXMONSTERS; i++) {
            if ((strcmp(map[player.position].monstersPresent[i].name, noun) == 0) && (found == false)) {
                toAttack = i;
                found = true;
            }
        }
        if (found == false) {
         //   printf("\n%s is not here.", noun);
            return -1;
        }
    }
    return toAttack;
}

int pickItem (char noun[]) {
    bool found = false;
    int toUse = 0;
    if (noun != NULL) {
        for (int i = 0; i < MAXITEMS; i++) {
            if ((strcmp(map[player.position].itemsPresent[i].name, noun) == 0) && (found == false)) {
               toUse = i;
               found = true;
            }
        }
        if (found == false)
            return -1;
    }
    return toUse;
}

void executeTalk (char noun[]) {
    if (noun != NULL) {
        if (pickTarget(noun) >= 0)
            printf("\n%s", map[player.position].monstersPresent[pickTarget(noun)].reply);
        else printf("\n%s isn't here.", noun);
    }
}

void executeAttack (char noun[]) {
    int toAttack = 0; //badidea?
    int attackRoll = 0;
    int attackDamage = 0;

    if (noun != NULL) {
        if (pickTarget(noun) >= 0) {
            toAttack = pickTarget(noun);
            while ((player.hp > 0) && (map[player.position].monstersPresent[toAttack].hp > 0)) {
                //player's turn
                attackRoll = diceroller(1, 20);
                if (attackRoll > map[player.position].monstersPresent[toAttack].armorClass) {
                    attackDamage = diceroller(player.numHitDice, 4);
                    map[player.position].monstersPresent[toAttack].hp = map[player.position].monstersPresent[toAttack].hp - attackDamage;
                    printf("\nYou hit %s for %d points!", map[player.position].monstersPresent[toAttack].name, attackDamage);
                    printf("\nmonster hp: %d", map[player.position].monstersPresent[toAttack].hp);
                }
                else printf("\nYou missed!");
                //monster's turn
                if (map[player.position].monstersPresent[toAttack].hp > 0) {
                    attackRoll = diceroller(1, 20);
                    if (attackRoll > player.armorClass) {
                        attackDamage = diceroller(map[player.position].monstersPresent[toAttack].numHitDice, 4);
                        player.hp = player.hp - attackDamage;
                        printf("\n%s hit you for %d points.", map[player.position].monstersPresent[toAttack].name, attackDamage);
                        showHP();
                    }
                    else printf("\n%s missed!", map[player.position].monstersPresent[toAttack].name);
                }
            }
            if (player.hp <= 0) {
                printf("\nYou died.");
            }
            else if (map[player.position].monstersPresent[toAttack].hp <= 0) {
                printf("\n%s died!", map[player.position].monstersPresent[toAttack].name);
                player.xp = player.xp + map[player.position].monstersPresent[toAttack].xp;
                removeMonsterFromRoom(toAttack);
            }
        }
        else printf("\n%s isn't here.", noun);
    }
}

void showHP (){
    printf("\nHP: %d", player.hp);
}

void showXP () {
    printf("\nXP: %d", player.xp);
}

void showInventory() {
    int last = numItemsInInventory();
    printf("\nInventory:");
    for (int i = 0; i < last; i++) {
        printf("\n - %s", player.inventory[i].name);
        if (player.inventory[i].wielded == true)
            printf(" - wielded");
    }
}

void executeHeal() {
    printf("\nYou feel a warm glow");
    player.hp = 20;
    showHP();
}

void removeMonsterFromRoom(int toRemove) {
    //add line to get monster to drop inventory
    strcpy(map[player.position].monstersPresent[toRemove].description, "this dude ded");
    for (int i = toRemove; i < (MAXMONSTERS - 1); i++) {
            //without -1, deletes monsters correctly, but adds first item from itemspresent array
            //with -1, last item in monsterspresent wont be overwritten
        map[player.position].monstersPresent[i] = map[player.position].monstersPresent[i + 1];
    }
    //none of the below did what i wanted -- to set last element to "zero"
    //memset(&map[player.position].monstersPresent[MAXMONSTERS], 0, sizeof(map[player.position].monstersPresent[MAXMONSTERS]));
    //strcpy(map[player.position].monstersPresent[MAXMONSTERS].name, "\0");
    //map[player.position].monstersPresent[MAXMONSTERS - 1] = {};
    map[player.position].monstersPresent[MAXMONSTERS - 1] = blank;
}

int numItemsInInventory() {
    //returns first open position in inventory
    int placeToAdd = -1;
    for (int i = (MAXINVITEMS - 1); i >= 0; i--){
        if (strcmp(player.inventory[i].name, "\0") == 0)
            placeToAdd = i;
    }
    return placeToAdd;
}

int numMonstersInRoom() {
    //returns first open position in map.monstersPresent[]
    int placeToAdd = -1;
    for (int i = (MAXMONSTERS - 1); i >= 0; i--) {
        if (strcmp(map[player.position].monstersPresent[i].name, "\0") == 0)
            placeToAdd = i;
    }
    return placeToAdd;
}

int numItemsInRoom() {
    //returns first open position in map.itemsPreseent[]
    int placeToAdd = -1;
    for (int i = (MAXITEMS - 1); i >= 0; i--){
        if (strcmp(map[player.position].itemsPresent[i].name, "\0") == 0)
            placeToAdd = i;
    }
    return placeToAdd;
}

void addToInventory(char noun[]){
    int toTake = -1, placeToAdd = -1;
    if (noun != NULL) {
        if (pickItem(noun) >= 0) {
            toTake = pickItem(noun);
            placeToAdd = numItemsInInventory();
        //add to inventory
            if ((placeToAdd >= 0) && (placeToAdd < MAXINVITEMS)) {
                printf("\nYou take %s.", map[player.position].itemsPresent[toTake].name);
                player.inventory[placeToAdd] = map[player.position].itemsPresent[toTake];
                //delete from room
                for (int i = toTake; i < (MAXITEMS - 1); i++)
                    map[player.position].itemsPresent[i] = map[player.position].itemsPresent[i + 1];
                map[player.position].itemsPresent[MAXITEMS - 1] = blankItem;
                }
            else printf("\nI think you are carrying too many things.");
        }
        else printf("\n%s isn't here.", noun);
    }
    else printf("\n%s isn't here. (will this even print anything, since noun is NULL?", noun);
}

void removeFromInventory(char noun[]) {
    int toTake = -1, lastSpotInRoom = 0;
    if (noun != NULL) {
        if (pickFromInventory(noun) >= 0) {
            toTake = pickFromInventory(noun);
            lastSpotInRoom = numItemsInRoom();
            //unwield if wielded
            if (player.inventory[toTake].wielded == true)
                executeUnwield(player.inventory[toTake].name);
        //add to room
            if ((lastSpotInRoom >= 0) && (lastSpotInRoom < MAXITEMS)) {
                map[player.position].itemsPresent[lastSpotInRoom] = player.inventory[toTake];
                printf("\nDropping %s", player.inventory[toTake].name);
            //remove from inventory
                for (int j = toTake; j < MAXINVITEMS; j++)
                    player.inventory[j] = player.inventory[j + 1];
                player.inventory[MAXINVITEMS - 1] = blankItem;
            }
            else printf("\nThere's no room.");
        }
        else printf("\nYou don't have %s.", noun);

    }
    else printf("\nYou don't have %s.", noun);
}

int pickFromInventory(char noun[]) {
    int toPick = -1;
    bool found = false;
    for (int i = 0; i < MAXINVITEMS; i++) {
        if ((strcmp(player.inventory[i].name, noun) == 0) && (found == false))  {
            toPick = i;
            found = true;
        }
    }
    return toPick;
}

void executeWield(char noun[]) {
    int toWield = -1, numItemsWielded = 0;
    toWield = pickFromInventory(noun);
    if (noun != NULL) {
        //check how many items wielded
        for (int i = 0; i < MAXINVITEMS; i++){
            if (player.inventory[i].wielded == true)
                numItemsWielded++;
        }
        if (numItemsWielded < 2) {
            if ((toWield >= 0) && (player.inventory[toWield].wielded == false)) {
                player.inventory[toWield].wielded = true;
                player.numHitDice = player.numHitDice + player.inventory[toWield].attackBonus;
                player.armorClass = player.armorClass + player.inventory[toWield].defenseBonus;
                printf("\nYou wield %s", player.inventory[toWield].name);
            }
            else if (player.inventory[toWield].wielded == true)
                printf("\nYou are already wielding %s", player.inventory[toWield].name);
            else printf("\nYou don't have %s", noun);
        }
        else printf("\nYou are already wielding 2 items.");
    }
    else printf("\nYou cannot do that.");
}

void executeUnwield(char noun[]) {
    bool found = false;
    int toUnwield = -1;
    for (int i = 0; i < MAXINVITEMS; i++) {
        if ((strcmp(player.inventory[i].name, noun) == 0) && (found == false)) {
            found = true;
            toUnwield = i;
        }
    }
    if ((toUnwield >= 0) && (toUnwield < MAXINVITEMS)) {
        if (player.inventory[toUnwield].wielded == true) {
        //unwield
            player.inventory[toUnwield].wielded = false;
            //remove bonuses
            player.numHitDice = player.numHitDice - player.inventory[toUnwield].attackBonus;
            player.armorClass = player.armorClass - player.inventory[toUnwield].defenseBonus;
            printf("\nYou unwield %s", player.inventory[toUnwield].name);
        }
        else printf("\nYou're not wielding that");
    }
    else printf("\nYou cannot unwield %s", noun);
}

void executeHelp() {
//lists few commands to get started
    printf("\nHere's a few commands to get started:");
    printf("\n<look goblin> - looks at a goblin");
    printf("\n<take coin> - pick up a coin");
    printf("\n<attack goblin> - enter into combat with a goblin");
    printf("\n<drop coin> - drop coin to the ground");
    printf("\n<wield sword> - wield sword you are currently holding");
    printf("\n<equip shield> - equip a shield you are currently holding");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
}

void showstats() {
    printf("\nPlayer Stats:");
    printf("\nHP: %d", player.hp);
    printf("\nXP: %d", player.xp);
    printf("\nNumber of Hit Dice: %d", player.numHitDice);
    printf("\nArmor Class: %d", player.armorClass);
    printf("\nPosition: %d", player.position);
}
