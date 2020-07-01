//chris fite
//created 110719
//edited 120619

//fifth edition
//  now with spells!
//  and more maps
//  and spell quest

//randomize num goblins in dungeon
//idea - resistance in monsters to certain attacks, elements
//idea - room timer: rooms reset after a certain amount of time has past since you've been in it
//  set var = time() during init? <-room will only reset during inits
//  when you go to enter a room, if var / arbitraryIntervalOfTime > 1, reset room

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
    bool equipped;
};

struct person {
    char name[15];
    char description[50];
    int position;
    char reply[50];
    int hp;
    int mp;
    int xp;
    int armorClass;
    int numHitDice; //number of d4 dice to roll for damage
    struct item inventory[MAXINVITEMS];
    struct item coins[100];
};

struct room {
    char name[20];
    char description[50];
    int roomNum;
    char exits[5]; //nsew/0
    struct person monstersPresent[MAXMONSTERS];
    struct item itemsPresent[MAXITEMS];
};
struct spells {
    char name[15];
    int spellCost;
    int damage;
    char description[50];
};
//initialize structs for gameplay
struct spells spellBook[7] = {
    {"heal", 5, -10, "A warm glow surrounds "}
    };

struct room map[16];
struct room blankRoom = {0};//workaround for resetting maps
//person: name, descr, pos, reply, hp, mp, xp, ac, hitdice, inv[], ---wield[]---
struct person player = {"player", "you are yourself", 0, "what?", 20, 10, 0, 5, 1};
struct person goblin = {"goblin", "A gross looking goblin", 0, "The goblin snarls at you.", 10, 0, 2, 5, 1};
struct person troll = {"troll", "A big ugly cave troll.", 0, "The troll grunts.", 15, 0, 5, 7, 2};
struct person wizard = {"wizard", "A grey bearded wizard dude.", 0, "I can teach you a spell for 3 gold coins.", 50, 30, 50, 50, 0};
struct person townie = {"townie", "A local townsperson.", 0, "huh?", 10, 0, 0, 100, 0};
struct person boss = {"boss", "It's the boss.", 0, "The boss looks unamused.", 30, 15, 20, 100, 3};
struct person blank = {0}; //weird workaround but ok --for removemonster
//item: name, desc, attbonus, defbonus, wielded, equipped
struct item coin = {"coin", "A shiny coin with somebody's face on one side.", 0, 0, false, false};
struct item sword = {"sword", "A sharp looking sword.", 1, 0, false, false};
struct item rock = {"rock", "A medium sized rock.", 0, 0, false, false};
struct item broadsword = {"broadsword", "A big badass swinging sword.", 2, 0, false, false};
struct item shield = {"shield", "A round, wooden shield", 0, 2, false, false};
struct item helm = {"helm", "A round metal helmet", 0, 1, false, false};
struct item greatHelm = {"greathelm", "A massive helm made of goat horns and bones.", 0, 5, false, false};
struct item breastplate = {"breastplate", "A bronze breastplate with an embossed eagle", 0, 3, false, false};
struct item axe = {"axe", "A great war axe", 2, 1, false, false};
struct item dagger = {"dagger", "A nasty, curved, rusty affair.", 1, 0, false, false};
struct item potion = {"potion", "A glowing green potion in a flask.", 0, 10, false, false};
struct item plaque = {"plaque", "A tablet with runes of the four elements.", 0, 0, false, false};
struct item potion2 = {"potion", "A glowing blue potion in a flask.", 0, 10, false, false};
struct item blankItem = {0}; //wierd work around for removeitem

char input[100];
char mapType;
int maxPlayerHP = 20;
int maxPlayerMP = 10;
int playerLevel = 1;
char fourElements[5] = "zzzz"; //for secret door quest

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
int lastCoinSpot();
int price(struct item);
bool deadyet(struct person);

void forestInit();
void dungeonInit();
void townInit();
void bossRoomInit();
void executeGo(char[]);
void executeLook(char[]);
void executeTalk(char[]);
void executeAttack(char[]);
void showHP();
void showXP();
void showMP();
void showInventory();
void executeHeal();
void removeMonsterFromRoom(int);
void addToInventory(char[]);
void removeFromInventory(int);
void executeWield(char[]);
void executeUnwield(char[]);
void executeHelp();
void showstats();
void executeCast(char[], char[]);
void counterAttack(int);
void executeEquip(char[]);
void executeUnequip(char[]);
void executeRage(char[]);
void listSpells();
void addCoins();
void removeCoins();
void executeTake(char[]);
void executeDrop(char[]);
void wizardInteraction(int);
void executeGive(char[], char[]);
void shopInteraction(struct person);
void innInteraction();
void executeUse(char[]);
void executeExamine(char[]);
void bossInteraction(int);
void secretDoorInteraction(char[]);
void addXP(int);

int main () {
    printf("Welcome to my first rpg.\n");
    player.inventory[0] = potion;
    player.inventory[1] = potion2;

//easy access for testing rooms
    player.inventory[2] = sword;
    player.inventory[3] = breastplate;
    player.inventory[4] = shield;
    player.inventory[5] = sword;
    player.inventory[6] = helm;
    player.inventory[7] = dagger;

    for (int i =0; i < 50; i++)
        addCoins();
/*
//for testing boss encounter
    int openSpellSlot = 1;
    strcpy(spellBook[openSpellSlot].name, "fireball");
    spellBook[openSpellSlot].spellCost = 5;
    spellBook[openSpellSlot].damage = 5;
    strcpy(spellBook[openSpellSlot].description, "A giant fireball explodes on ");
    bossRoomInit();
*/
    townInit();
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
    char *target = strtok(NULL, " \n");

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
            showMP();
            showXP();
        }
        else if (strcmp(verb, "heal") == 0) {
            executeHeal();
        }
       else if (strcmp(verb, "take") == 0) {
            executeTake(noun);
        }
        else if (strcmp(verb, "inv") == 0) {
            showInventory();
       }
        else if (strcmp(verb, "drop") == 0) {
            executeDrop(noun);
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
        else if (strcmp(verb, "hmm") == 0) {
            printf("\nYou stroke your beard and say, hmmmmm.");
        }
        else if (strcmp(verb, "stats") == 0) {
            showstats();
        }
        else if (strcmp(verb, "cast") == 0) {
            executeCast(noun, target);
        }
        else if (strcmp(verb, "equip") == 0) {
            executeEquip(noun);
        }
        else if (strcmp(verb, "unequip") == 0) {
            executeUnequip(noun);
        }
        else if (strcmp(verb, "rage") == 0) {
            executeRage(noun);
        }
        else if (strcmp(verb, "spells") == 0) {
            listSpells();
        }
        else if (strcmp(verb, "give") == 0) {
            executeGive(noun, target);
        }
        else if (strcmp(verb, "use") == 0) {
            executeUse(noun);
        }
        else if (strcmp(verb, "examine") == 0) {
            executeExamine(noun);
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
        rando = rand() % (sides) + 1; //wtf is +1 for?
        //total rolls
        total = total + rando;
        }
    return total;
}

void forestInit() {
    mapType = 'f';
    int rando = 0;

    printf("\n...Loading...\n");
    //clear rooms
    for (int i = 0; i < 16; i++)
        map[i] = blankRoom;
    //items in room
    map[10].itemsPresent[2] = broadsword;
    map[2].itemsPresent[2] = shield;
    map[5].itemsPresent[2] = sword;
    //wizards for spell quest
    map[0].monstersPresent[2] = wizard; //2nd slot to not overwrite goblins
    strcpy(map[0].monstersPresent[2].description, "An old bearded wizard in blue robes.");
    map[5].monstersPresent[2] = wizard;
    strcpy(map[5].monstersPresent[2].description, "An old bearded wizard in red robes.");
    map[12].monstersPresent[2] = wizard;
    strcpy(map[12].monstersPresent[2].description, "An old bearded wizard in silver robes.");
    //add in some goblins
    for (int j = 0; j < 5; j++) {
        srand(time(NULL));
        rando = (rand() + 5) % 16; // +j to separate them a bit more
        map[rando].monstersPresent[0] = goblin;
        map[rando].monstersPresent[0].inventory[0] = coin;
        map[rando].monstersPresent[0].inventory[1] = coin;
        sleep(1);
    }
    //main init loop
    for (int i = 0; i < 16; i++) {
        strcpy(map[i].name, "forest");
        map[i].roomNum = i;
        switch(i) {
            //corners
            case 0:
                strcpy(map[i].description, "the southwest corner of the forest");
                strcpy(map[i].exits, "nse  ");
                break;
            case 3:
                strcpy(map[i].description, "the southeast corner of the forest");
                strcpy(map[i].exits, "nw  ");
                break;
            case 12:
                strcpy(map[i].description, "the northwest corner of the forest");
                strcpy(map[i].exits, "se  ");
                break;
            case 15:
                strcpy(map[i].description, "the northeast corner of the forest");
                strcpy(map[i].exits, "nsw  ");
                break;
            //west wall
            case 4:
            case 8:
                strcpy(map[i].description, "a clearing along the western edge");
                strcpy(map[i].exits, "nse ");
                break;
            //east wall
            case 7:
            case 11:
                strcpy(map[i].description, "a clearing along the eastern edge");
                strcpy(map[i].exits, "nsw ");
                break;
            // north,
            case 13:
            case 14:
                strcpy(map[i].description, "a clearing along the northern edge");
                strcpy(map[i].exits, "sew ");
                break;
            //south walls,
            case 1:
            case 2:
                strcpy(map[i].description, "a clearing along the southern edge");
                strcpy(map[i].exits, "new ");
                break;
            //middle
            default:
                strcpy(map[i].description, "a lovely forest clearing");
                strcpy(map[i].exits, "nsew");
                break;
        };
    }
}

void dungeonInit() {
    mapType = 'd';
    int rando = 0;
    //clear rooms
    printf("\n...Loading...\n");
    for (int i = 0; i < 16; i++)
        map[i] = blankRoom;
    //items in room
    for (int i = 0; i < 16; i = i + 3) {
        map[i].itemsPresent[0] = coin;
        if (i % 2 == 0)
            map[i].itemsPresent[1] = coin;
    }
    map[1].itemsPresent[2] = helm;
    map[2].itemsPresent[2] = shield;
    map[3].itemsPresent[2] = sword;
    map[6].itemsPresent[2] = axe;
    map[15].itemsPresent[2] = plaque;

    //add in some goblins
    for (int j = 0; j < 7; j++) {
        srand(time(NULL));
        rando = (rand() + 7) % 16; // +j to separate them a bit more
        map[rando].monstersPresent[0] = goblin;
        map[rando].monstersPresent[0].inventory[0] = coin;
        map[rando].monstersPresent[0].inventory[1] = coin;
        if (j % 3 == 0) {
            map[rando].monstersPresent[1] = goblin;
            map[rando].monstersPresent[1].inventory[0] = dagger;
            map[rando].monstersPresent[1].inventory[1] = sword;
            strcpy(map[rando].monstersPresent[1].description, "This gross little guy has seen some shit.");
            strcpy(map[rando].monstersPresent[1].reply, "The goblin hums to itself.");
        }
        sleep(1);
    }
    //wizards for wizard quest
    map[9].monstersPresent[2] = wizard;
    strcpy(map[9].monstersPresent[2].description, "An old bearded wizard in white robes.");
    map[12].monstersPresent[2] = wizard;
    strcpy(map[12].monstersPresent[2].description, "An old bearded wizard in brown robes.");
    map[3].monstersPresent[2] = wizard;
    strcpy(map[3].monstersPresent[2].description, "An old bearded wizard in black robes.");
    //they have a cave troll
    map[5].monstersPresent[2] = troll;
    map[5].monstersPresent[2].inventory[0] = breastplate;
    map[15].monstersPresent[2] = troll;
    map[15].monstersPresent[2].inventory[0] = breastplate;
    //room: name, desc, roomnum, exits[], monsPres[], itemsPres[]
    //main init loop
    for (int i = 0; i < 16; i++) {
        strcpy(map[i].name, "dungeon");
        map[i].roomNum = i;
        switch(i) {
            //corners
            case 0:
                strcpy(map[i].description, "the southwest corner of the dungeon");
                strcpy(map[i].exits, "nse ");
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

void townInit() {
    mapType = 't';
    int rando = 0;
    //clear rooms
    printf("\n...Loading...\n");
    for (int i = 0; i < 16; i++) {
        map[i] = blankRoom;
        map[i].roomNum = i;
    }
    //main init loop
    for (int i = 0; i < 16; i++ ) {
        switch(i) {
    //set up inn
    //set up shops
        case 0:
            strcpy(map[i].name, "bedroom");
            strcpy(map[i].description, "The bedroom of the inn.");
            strcpy(map[i].exits, "n   ");
            //map[i].itemsPresent[0] = ;
            break;
        case 1:
            strcpy(map[i].name, "bar");
            strcpy(map[i].description, "the bar of the inn");
            strcpy(map[i].exits, "n   ");
            map[i].monstersPresent[0] = townie;
            strcpy(map[i].monstersPresent[0].name, "barkeep");
            break;
        case 3:
            strcpy(map[i].name, "weapon shop");
            strcpy(map[i].description, "You can buy weapons here");
            strcpy(map[i].exits, "w   ");
            //setup shopkeeper
            map[i].monstersPresent[0] = townie;
            strcpy(map[i].monstersPresent[0].name, "shopkeeper");
            strcpy(map[i].monstersPresent[0].reply, "What would you like to buy?");
            strcpy(map[i].monstersPresent[0].description, "A bald, muscled man wearing a leather vest.");
            map[i].monstersPresent[0].inventory[0] = helm;
            map[i].monstersPresent[0].inventory[1] = shield;
            map[i].monstersPresent[0].inventory[2] = sword;
            map[i].monstersPresent[0].inventory[3] = dagger;
            break;
        case 4:
            strcpy(map[i].name, "inn");
            strcpy(map[i].description, "common room of the inn");
            strcpy(map[i].exits, "se  ");
            break;
        case 5:
            strcpy(map[i].name, "Inn");
            strcpy(map[i].description, "common room of the inn");
            strcpy(map[i].exits, "nsw ");
            map[i].monstersPresent[0] = townie;
            strcpy(map[i].monstersPresent[0].name, "innkeeper");
            strcpy(map[i].monstersPresent[0].reply, "I fuck goats");
            strcpy(map[i].monstersPresent[0].description, "A fat man with a dirty apron and a handlebar mustache.");
            break;
        case 12:
            strcpy(map[i].name, "quest");
            strcpy(map[i].description, "gonna put a quest here");
            strcpy(map[i].exits,"s   ");
            break;
        case 15:
            strcpy(map[i].name, "item shop");
            strcpy(map[i].description, "You can buy items here.");
            strcpy(map[i].exits, "w   ");
            //setup shopkeeper
            map[i].monstersPresent[0] = townie;
            strcpy(map[i].monstersPresent[0].name, "shopkeeper");
            strcpy(map[i].monstersPresent[0].reply, "What would you like to buy?");
            strcpy(map[i].monstersPresent[0].description, "A bespectacled old man, wearing an apron.");
            map[i].monstersPresent[0].inventory[0] = potion;
            map[i].monstersPresent[0].inventory[1] = potion2;
            break;
    //set up main road
        case 2:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "ne  ");
            break;
        case 6:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "ns  ");
            break;
        case 8:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "ne  ");
            break;
        case 9:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "sew ");
            break;
        case 10:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "nsew");
            break;
        case 11:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "w   ");
            break;
        case 14:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            strcpy(map[i].exits, "nse  ");
            break;
        default:
            strcpy(map[i].name, "Road");
            strcpy(map[i].description, "A dusty road through town.");
            break;
        }
    }
    //set up townspeople
    for (int i = 0; i < 5; i++) {
        srand(time(NULL));
        rando = (rand() + i) % 16;
        map[rando].monstersPresent[2] = townie;
        switch(i) {
            case 1:
                strcpy(map[rando].monstersPresent[2].reply, "I hear there are wizards in the forest.");
                strcpy(map[rando].monstersPresent[2].description, "A young man in overalls.");
                break;
            case 2:
                strcpy(map[rando].monstersPresent[2].reply, "If you're going to the forest, you should arm yourself.");
                strcpy(map[rando].monstersPresent[2].description, "A grizzled old man.");
                break;
            case 3:
                strcpy(map[rando].monstersPresent[2].name, "kid");
                strcpy(map[rando].monstersPresent[2].reply, "I like cake.");
                strcpy(map[rando].monstersPresent[2].description, "A fat kid.");
                break;
            case 4:
                strcpy(map[rando].monstersPresent[2].reply, "Legend has it there's a dungeon deep in the forest");
                strcpy(map[rando].monstersPresent[2].description, "An old wife.");
                break;
            case 0:
                strcpy(map[rando].monstersPresent[2].reply, "You should rest at the inn.  It's nice. Four stars.");
                strcpy(map[rando].monstersPresent[2].description, "A well-dressed man.");
                break;
            default:
                break;
        }
        sleep(1);
    }

    executeLook("around");
    printf("\n");
}

void bossRoomInit() {
    //clear rooms
    mapType = 'b';
    printf("\n...Loading...\n");
    for (int i = 0; i < 16; i++) {
        map[i] = blankRoom;
        map[i].roomNum = i;
    }
    strcpy(map[0].description, "It's the boss' room. Scary.");
    strcpy(map[0].exits, "s   ");
    map[0].monstersPresent[0] = boss;
    map[0].monstersPresent[0].inventory[0] = axe;
    map[0].monstersPresent[0].inventory[1] = greatHelm;
    map[0].monstersPresent[0].inventory[2] = coin;
    map[0].monstersPresent[0].inventory[3] = coin;
    map[0].monstersPresent[0].inventory[4] = coin;
    map[0].monstersPresent[0].inventory[5] = coin;
    map[0].monstersPresent[0].inventory[6] = coin;
    //main sequence
    printf("\n\nThe boss growls at you.");
    printf("\nSo! You have discovered my lair. \nNow what are you going to do?\n");
    bossInteraction(0);
}

void executeGo(char noun[]) {
    //set up as 4x4 square
    if (noun != NULL) {
        if (strstr(map[player.position].exits, noun) != NULL) { //check for a valid exit
        //setting up exits to other maps
            if ((strcmp(noun, "s") == 0) && (player.position == 0)) {
                switch(mapType) {
                    case 'd':
                        forestInit();
                        player.position = 15;
                        break;
                    case 'f':
                        townInit();
                        player.position = 14;//town exit
                        break;
                    case 'b':
                        dungeonInit();
                        player.position = 15;
                        break;
                    default:
                        printf("\nYou can't go that way");
                        break;
                }
            }
            else if ((strcmp(noun, "n") == 0) && (player.position == 15)) {
                if (mapType == 'f') { //highest position in forest
                    dungeonInit();
                    player.position = 0;
                }
                else if (mapType == 'd') {
                    player.position = 0;
                    bossRoomInit();
                }
                else printf("\nYou can't go that way");
            }
            else if ((strcmp(noun, "n") == 0) && (player.position == 14)) {
                if (mapType == 't') { //exit out of town
                    forestInit();
                    player.position = 0;
                }
                else printf("\nYou can't go that way");
            }
            else { //not at a map exit
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
        else printf("\nYou can't go that way.");
    }
    else printf("\nWhere do you wish to go?");
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
    int toAttack = -1;

    if (noun != NULL) {
        for (int i = 0; i < MAXMONSTERS; i++) {
            if ((strcmp(map[player.position].monstersPresent[i].name, noun) == 0) && (found == false)) {
                toAttack = i;
                found = true;
            }
        }
        if (found == false)
            return -1;
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
        if (strcmp(noun, "me") == 0) {
            printf("\n%s", player.reply);
        }
        else if ((strcmp(noun, "shopkeeper") == 0) && (pickTarget(noun) != -1)) {
            shopInteraction(map[player.position].monstersPresent[pickTarget(noun)]);
        }
        else if ((strcmp(noun, "innkeeper") == 0) && (pickTarget(noun) != -1)) {
            innInteraction();
        }
        else if ((strcmp(noun, "boss") == 0) && (pickTarget(noun) != -1)) {
            bossInteraction(pickTarget(noun));
        }
        else {
            if (pickTarget(noun) >= 0)
                printf("\n%s", map[player.position].monstersPresent[pickTarget(noun)].reply);
            else printf("\n%s isn't here.", noun);
        }
    }
}

void executeAttack (char noun[]) {
    int toAttack = -1;
    int attackRoll = 0;
    int attackDamage = 0;

    if (noun != NULL) {
        if (pickTarget(noun) >= 0) {
            toAttack = pickTarget(noun);
            //player's turn
            if (toAttack >= 0) {
                attackRoll = diceroller(1, 20);
                if (attackRoll > map[player.position].monstersPresent[toAttack].armorClass) {
                    attackDamage = diceroller(player.numHitDice, 4);
                    map[player.position].monstersPresent[toAttack].hp = map[player.position].monstersPresent[toAttack].hp - attackDamage;
                    printf("\nYou hit %s for %d points!", map[player.position].monstersPresent[toAttack].name, attackDamage);
                    printf("\nmonster hp: %d", map[player.position].monstersPresent[toAttack].hp);
                }
                else printf("\nYou missed!");
                //monster's turn
                sleep(1);
                counterAttack(toAttack);

                if (deadyet(player) == true) {
                    printf("\nYou died.");
                }
                else if (deadyet(map[player.position].monstersPresent[toAttack]) == true) {
                    printf("\n%s died!", map[player.position].monstersPresent[toAttack].name);
                    addXP(map[player.position].monstersPresent[toAttack].xp);
                    removeMonsterFromRoom(toAttack);
                }
            }
            else printf("\n%s isn't here.", noun);
        }
        else printf("\n%s isn't here.", noun);
    }
    else printf("\nWhom do you want to attack?");
}

void showHP (){
    printf("\nHP: %d / %d", player.hp, maxPlayerHP);
}

void showXP () {
    printf("\nXP: %d / %d", player.xp, maxPlayerMP);
}

void showMP() {
    printf("\nMP: %d", player.mp);
}

void showInventory() {
    printf("\nInventory:");
    printf("\nCoins: %d", lastCoinSpot());
    printf("\n-------------");
    printf("\n(slot - name)");
    for (int i = 0; i < MAXINVITEMS; i++) {
        if (strcmp(player.inventory[i].name, "\0") != 0) {
            printf("\n %d - %s", i, player.inventory[i].name);
            if (player.inventory[i].wielded == true)
                printf(" - wielded");
            if (player.inventory[i].equipped == true)
                printf(" - equipped");
        }
    }
}

void executeHeal() {
    printf("\nYou feel a warm glow");
    player.hp = maxPlayerHP;
    player.mp = maxPlayerMP;
    showHP();
    showMP();
}

void removeMonsterFromRoom(int toRemove) {
    int k = 0; // counter for monster inventory
    //unwield, unequip items,  add to room inv
    for (int j = numItemsInRoom(); j < MAXITEMS; j++) {
        if (map[player.position].monstersPresent[toRemove].inventory[k].wielded == true)
            map[player.position].monstersPresent[toRemove].inventory[k].wielded = false;
        if (map[player.position].monstersPresent[toRemove].inventory[k].equipped == true)
            map[player.position].monstersPresent[toRemove].inventory[k].equipped = false;
        if (strcmp(map[player.position].monstersPresent[toRemove].inventory[k].name, "\0") != 0)
            map[player.position].itemsPresent[j] = map[player.position].monstersPresent[toRemove].inventory[k];
        k++;
    }
    strcpy(map[player.position].monstersPresent[toRemove].description, "this dude ded");
    for (int i = toRemove; i < (MAXMONSTERS - 1); i++) {
            //without -1, deletes monsters correctly, but adds first item from itemspresent array
            //with -1, last item in monsterspresent wont be overwritten
        map[player.position].monstersPresent[i] = map[player.position].monstersPresent[i + 1];
    }
    map[player.position].monstersPresent[MAXMONSTERS - 1] = blank; //kinda crap but it works
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

void removeFromInventory(int toRemove) {
    if (toRemove >= 0) {
        for (int j = toRemove; j < MAXINVITEMS; j++)
            player.inventory[j] = player.inventory[j + 1];
        player.inventory[MAXINVITEMS - 1] = blankItem;
    }
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
    printf("\n<attack goblin> - enter into combat with a goblin");
    printf("\n<take coin> - pick up a coin");
    printf("\n<drop coin> - drop coin to the ground");
    printf("\n<wield sword> - wield sword you are currently holding");
    printf("\n<equip shield> - equip a shield you are currently holding");
    printf("\n<cast fireball goblin> - casts fireball spell on goblin");
    printf("\n<spells> - lists known spells");
    printf("\n<rage goblin> - go absolutely beserk on a goblin");
    printf("\n<inv> - lists your inventory");
    printf("\n<examine sword> - look at a sword you are currently holding");
    printf("\n<use potion> - use a potion you are currently holding");
    printf("\n");
    printf("\n");
}

void showstats() {
    printf("\nPlayer Stats:");
    printf("\nHP: %d", player.hp);
    printf("\nMP: %d", player.mp);
    printf("\nXP: %d", player.xp);
    printf("\nNumber of Hit Dice: %d", player.numHitDice);
    printf("\nArmor Class: %d", player.armorClass);
    printf("\nPosition: %d", player.position);
}

void executeCast(char spell[], char target[]) {
    int damage = 0, toPick = -1, attackRoll = 0;
    int spellToUse = -1;
    int numSpells = 7; //need a const or dynamically compute

    if (spell != NULL) {
        if (target != NULL) {
            if ((pickItem(target) >= 0) && (strcmp(target, "plaque") == 0)) {
                secretDoorInteraction(spell);
            }
            else if (strcmp(spell, "heal") == 0) {
                if (player.mp >= spellBook[0].spellCost) {
                    if (strcmp(target, "self") == 0) {
                        player.hp = player.hp - spellBook[0].damage;
                        printf("\nYou feel a warm glow surround you.");
                        if (player.hp > maxPlayerHP)
                            player.hp = maxPlayerHP;
                        showHP();
                    }
                    else {
                        if (pickTarget(target) >= 0) {
                            toPick = pickTarget(target);
                            map[player.position].monstersPresent[toPick].hp =  map[player.position].monstersPresent[toPick].hp - spellBook[0].damage;
                            printf("\nA warm glow surrounds %s",  map[player.position].monstersPresent[toPick].name);
                            printf("\n%s HP: %d",  map[player.position].monstersPresent[toPick].name,  map[player.position].monstersPresent[toPick].hp);
                        }
                        else printf("\n%s isn't here", target);
                    }
                    player.mp = player.mp - spellBook[0].spellCost;
                }
                else printf("\nYou don't have enough mana.");
            }
            else {
                if (pickTarget(target) >= 0) {
                    toPick = pickTarget(target);
                    attackRoll = diceroller(1, 20);
                    if (attackRoll >= map[player.position].monstersPresent[toPick].armorClass) {
                        for (int i = 0; i < numSpells; i++) {
                            if (strcmp(spellBook[i].name, spell) == 0)
                                spellToUse = i;
                        }
                        if (spellToUse >= 0) {
                            if (player.mp >= spellBook[spellToUse].spellCost) {
                                damage = spellBook[spellToUse].damage;
                                printf("\n%s%s", spellBook[spellToUse].description, map[player.position].monstersPresent[toPick].name);
                            //deal damage and remove if dead
                                player.mp = player.mp - spellBook[spellToUse].spellCost;
                                map[player.position].monstersPresent[toPick].hp = map[player.position].monstersPresent[toPick].hp - damage;
                                printf("\n%s hp: %d", map[player.position].monstersPresent[toPick].name, map[player.position].monstersPresent[toPick].hp);
                            //if drain is used, add hp to player
                                if (strcmp(spellBook[spellToUse].name, "drain") == 0){
                                    player.hp = player.hp + damage;
                                    showHP();
                                }
                                if (deadyet(map[player.position].monstersPresent[toPick]) == true) {
                                    printf("\n%s died!", map[player.position].monstersPresent[toPick].name);
                                    addXP(map[player.position].monstersPresent[toPick].xp);
                                    removeMonsterFromRoom(toPick);
                                }
                                else counterAttack(toPick);
                                if (deadyet(player) == true)
                                    printf("\nYou died!");
                            }
                            else printf("\nYou don't have enough mana");
                        }
                        else printf("\nYou don't know that spell.");
                    }
                    else printf("\nYour spell fizzles out.");
                }
                else printf("\n%s isn't here.", target); //picktarget returned -1
            }
        }
        else if (target == NULL)
            printf("\nCast %s on whom?", spell);
    }
    else if (spell == NULL)
        printf("\nCast what?");
}

void counterAttack(int toAttack) {
    int attackDamage = 0, attackRoll = 0;
    if (deadyet(map[player.position].monstersPresent[toAttack]) == false) {
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

void executeEquip(char noun[]) {
    int toEquip = -1;
    if (noun != NULL) {
        toEquip = pickFromInventory(noun);
        if (player.inventory[toEquip].equipped == false) {
            player.inventory[toEquip].equipped = true;
            player.armorClass = player.armorClass + player.inventory[toEquip].defenseBonus;
            printf("\nYou put on %s", player.inventory[toEquip].name);
        }
        else printf("\n%s is already equipped.", player.inventory[toEquip].name);
    }
    else printf("\nWhat do you want to equip?");
}

void executeUnequip(char noun[]) {
    int toEquip = -1;
    if (noun != NULL) {
        toEquip = pickFromInventory(noun);
        if (player.inventory[toEquip].equipped == true) {
            player.inventory[toEquip].equipped = false;
            player.armorClass = player.armorClass - player.inventory[toEquip].defenseBonus;
            printf("\nYou take off %s", player.inventory[toEquip].name);

        }
        else printf("\n%s is already unequipped.", player.inventory[toEquip].name);
    }
    else printf("\nWhat do you want to unequip?");
}

void executeRage(char noun[]) {
    int toAttack = -1;
    int attackRoll = 0;
    int attackDamage = 0;

    if (noun != NULL) {
        toAttack = pickTarget(noun);
        if (toAttack >= 0) {
            while ((deadyet(player) == false) && (deadyet(map[player.position].monstersPresent[toAttack]) == false)) {
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
                sleep(1);
                counterAttack(toAttack);
            }
            if (deadyet(player) == true) {
                printf("\nYou died.");
            }
            else if (deadyet(map[player.position].monstersPresent[toAttack]) == true) {
                printf("\n%s died!", map[player.position].monstersPresent[toAttack].name);
                addXP(map[player.position].monstersPresent[toAttack].xp);
                removeMonsterFromRoom(toAttack);
            }

        }
        else printf("\n%s isn't here.", noun);
    }
    else printf("\nOn whom do you wish to rage?");
}

bool deadyet(struct person toCheck) {
    bool isDead = false;
    if (toCheck.hp <= 0)
        isDead = true;
    return isDead;
}

void listSpells() {
    printf("\nCurrent known spells and costs:");
    for (int i = 0; i < 7; i++) {
        if (strcmp(spellBook[i].name, "\0") !=0)
            printf("\n%s: %d mp", spellBook[i].name, spellBook[i].spellCost);
    }
}

int lastCoinSpot() {
    //find last spot in coin inventory
    int lastSpot = -1;
    for (int i = 100; i >= 0; i--) {
        if (strcmp(player.coins[i].name, "\0") == 0)
            lastSpot = i;
    }
    return lastSpot;
}

void addCoins() {
    int lastSpot = lastCoinSpot();
    player.coins[lastSpot] = coin;
}

void removeCoins() {
    int lastSpot = lastCoinSpot();
    player.coins[lastSpot - 1] = blankItem;
}

void executeTake(char noun[]) {
    int toTake = -1;
    if (noun != NULL) {
        if (strcmp(noun, "coin") == 0)  {
            for (int i = 0; i < (MAXITEMS - 1); i++) {
                if (strcmp(map[player.position].itemsPresent[i].name, noun) == 0)
                    toTake = i;
            }
            if (toTake >= 0) {
                addCoins(); //add coin struct to coins[]
                //delete from room
                for (int i = toTake; i < (MAXITEMS - 1); i++)
                    map[player.position].itemsPresent[i] = map[player.position].itemsPresent[i + 1];
                map[player.position].itemsPresent[MAXITEMS - 1] = blankItem;
                printf("\nYou take a coin.");
            }
            else printf("\nYou can't do that.");
        }
        else if (strcmp(noun, "plaque") == 0)
            printf("\nYou can't take %s", noun);
        else addToInventory(noun);
    }
    else printf("\nWhat do you wish to take?");
}

void executeDrop(char noun[]) {
    int toTake = -1;
    int lastSpotInRoom = -1;
    if (noun != NULL) {
        lastSpotInRoom = numItemsInRoom();
        if (strcmp(noun, "coin") == 0) {
            toTake = lastCoinSpot();
            if  (toTake > 0) { //idk if this is still right
                //add to room, delete from coins[]
                if ((lastSpotInRoom >= 0) && (lastSpotInRoom < MAXITEMS)) {
                    map[player.position].itemsPresent[lastSpotInRoom] = player.coins[toTake - 1];
                    printf("\nDropping %s", player.coins[toTake -1].name);
                    removeCoins();
                }
                else printf("\nThere's no more room.");
            }
            else printf("\nYou don't have any coins.");
        }
        else { //this surprisingly gives correct error msg if lastcoispot == 0 -- hope still does
            toTake = pickFromInventory(noun);
            if (toTake >= 0){ //item's inventory number
                if ((lastSpotInRoom >= 0) && (lastSpotInRoom < MAXITEMS)) {
                    //add to room
                    map[player.position].itemsPresent[lastSpotInRoom] = player.inventory[toTake];
                    //unwield, unequip if nec
                    if (player.inventory[toTake].wielded == true)
                        executeUnwield(player.inventory[toTake].name);
                    if (player.inventory[toTake].equipped == true)
                        executeUnequip(player.inventory[toTake].name);
                    printf("\nDropping %s", player.inventory[toTake].name);
                    removeFromInventory(toTake);
                }
                else printf("\nThere isn't any room?");
            }
            else printf("\nYou don't have %s", noun);
        }
    }
    else printf("\nWhat do you wish to drop?");
}

void wizardInteraction(int wizNum) {
    int wizardCoinNum = 0;
    int openSpellSlot = -1;
    char *token = 0;
    char color[7];
    for (int i = 0; i < 99; i++) {
        if (strcmp(map[player.position].monstersPresent[wizNum].coins[i].name, "\0") != 0)
            wizardCoinNum++;
    }
    if (wizardCoinNum >=3) {
        printf("\nThe wizard teaches you the spell.");
        //determine available spell slot
        for (int i = 6; i >= 0; i--) {
            if (strcmp(spellBook[i].name, "\0") == 0)
                openSpellSlot = i;
        }
        if ((openSpellSlot >= 0) && (openSpellSlot < 7)) {
            //determine wizard robe color
            token = strtok(map[player.position].monstersPresent[wizNum].description, " ");
            while (token != NULL) {
                if (strcmp(token, "blue") == 0)
                    strcpy(color, "blue");
                else if (strcmp(token, "red") == 0)
                    strcpy(color, "red");
                else if (strcmp(token, "white") == 0)
                    strcpy(color, "white");
                else if (strcmp(token, "brown") == 0)
                    strcpy(color, "brown");
                else if (strcmp(token, "silver") == 0)
                    strcpy(color, "silver");
                else if (strcmp(token, "black") == 0)
                    strcpy(color, "black");
                token = strtok(NULL, " ");
            }
            if (color != NULL) {
                //distribute spell accordingly
                //water
                if (strcmp(color, "blue") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "freeze");
                    spellBook[openSpellSlot].spellCost = 4;
                    spellBook[openSpellSlot].damage = 4;
                    strcpy(spellBook[openSpellSlot].description, "Ice crystals form all over ");
                    //replace wizards description
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in blue robes.");
                }
                //fire
                else if (strcmp(color, "red") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "fireball");
                    spellBook[openSpellSlot].spellCost = 5;
                    spellBook[openSpellSlot].damage = 5;
                    strcpy(spellBook[openSpellSlot].description, "A giant fireball explodes on ");
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in red robes.");

                }
                //wine
                else if (strcmp(color, "white") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "tornado");
                    spellBook[openSpellSlot].spellCost = 3;
                    spellBook[openSpellSlot].damage = 3;
                    strcpy(spellBook[openSpellSlot].description, "Gusts of wind swirl around ");
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in white robes.");
                }
                //earth
                else if (strcmp(color, "brown") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "meteor");
                    spellBook[openSpellSlot].spellCost = 3;
                    spellBook[openSpellSlot].damage = 3;
                    strcpy(spellBook[openSpellSlot].description, "A big rock falls from above onto ");
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in brown robes.");
                }
                //lightning
                else if (strcmp(color, "silver") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "lightning");
                    spellBook[openSpellSlot].spellCost = 7;
                    spellBook[openSpellSlot].damage = 7;
                    strcpy(spellBook[openSpellSlot].description, "A lightning bolt strikes down ");
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in silver robes.");
                }
                //deat
                else if (strcmp(color, "black") == 0) {
                    strcpy(spellBook[openSpellSlot].name, "drain");
                    spellBook[openSpellSlot].spellCost = 5;
                    spellBook[openSpellSlot].damage = 5;
                    strcpy(spellBook[openSpellSlot].description, "A dark mist surrounds ");
                    strcpy(map[player.position].monstersPresent[wizNum].description, "A bearded old dude in black robes.");
                }
                else printf("\nThe wizard with that robe color isn't here.");
                listSpells();
                strcpy(map[player.position].monstersPresent[wizNum].reply, "I have nothing more to teach you.");
            }
        }
    }
    else printf("\nI'll need %d more coins to teach you.", (3 - wizardCoinNum));
}

void executeGive(char noun[], char target[]) {
    int toGiveTo = -1;
    int wizardCoinNum = 0;
    //can add pickfrominventorylater for items
    if (noun != NULL) {
        if (target != NULL) {
            toGiveTo = pickTarget(target);
            //if target = wizard {
            if (strcmp(map[player.position].monstersPresent[toGiveTo].name, "wizard") == 0) {
                //if item = coins,
                if (strcmp(noun, "coin") == 0) {
                    //addcoins(target)
                    for (int i = 0; i < 99; i++) {
                        if (strcmp(map[player.position].monstersPresent[toGiveTo].coins[i].name, "\0") != 0)
                            wizardCoinNum++;
                    }
                    map[player.position].monstersPresent[toGiveTo].coins[wizardCoinNum] = coin;
                    //removecoin (player)
                    removeCoins();
                    printf("\nYou give a coin to the wizard");
                    wizardInteraction(toGiveTo);
                }
                else printf("\nThe wizard doesn't want that.");
            }
        //else if target != wizard, target doesn't want that
            else printf("\n%s doesn't want %s", target, noun);

        }
        else printf("\nWhat do you want to give %s?", target);

    }
    else printf("\nTo whom do you wish to give what?");
}

int price(struct item thing) {
   int toReturn = 0;
   if (strcmp(thing.name, "potion") == 0) { //exception to keep potion price down
        toReturn = thing.defenseBonus;
   }
   else {
       if (thing.attackBonus >= thing.defenseBonus)
            toReturn = 5 * thing.attackBonus;
       else if (thing.attackBonus < thing.defenseBonus)
            toReturn = 5 * thing.defenseBonus;
    }
   return toReturn;
}

void shopInteraction(struct person shopkeeper) {
    char garbage, input = 'a';
    int sellPrice = 0, choice = -1, placeToAdd = -1;
    //greeting. show inventory, prices
    printf("\nThe shopkeeper smiles at you.\nGreetings, traveler. Do you see anything you like?");
    printf("\n---For Sale---");
    for (int i = 0; i < MAXINVITEMS; i++) {
        if (strcmp(shopkeeper.inventory[i].name, "\0") !=0) {
            printf("\n%d) %s: ", i, shopkeeper.inventory[i].name);
            sellPrice = price(shopkeeper.inventory[i]);
            printf("%d coins", sellPrice);
        }
    }
    //while response isn't to leave convo, continue buy/sell
    while (input != 'n') {
        printf("\nWould you like to <b>uy, <s>ell, or <n>evermind? ");
        scanf("%c%c", &input, &garbage); //dumb scanf is dumb
        if (input == 'b') {
            //execute buy
            printf("\nWhich item number?");
            scanf("%d%c", &choice, &garbage);
            if ((choice >= 0) && (choice < MAXINVITEMS)) {
                if (lastCoinSpot() >= price(shopkeeper.inventory[choice])) {
                    sellPrice = price(shopkeeper.inventory[choice]);
                    //add item to inventory
                    placeToAdd = numItemsInInventory();
                    if ((placeToAdd >= 0) && (placeToAdd < MAXINVITEMS)) {
                        printf("\nYou purchase %s", shopkeeper.inventory[choice].name);
                        player.inventory[placeToAdd] = shopkeeper.inventory[choice];
                        //deduct price
                        for (int i = 0; i < sellPrice; i++)
                            removeCoins();
                    }
                    else printf("\nYou can't carry anymore.");
                }
                else printf("\nYou don't have enough coins.");
            }
            else printf("\nThat is not for sale.");
        }
        else if (input == 's') {
            //execute sell
            showInventory();
            printf("\nWhich item in your inventory? ");
            scanf("%d%c", &choice, &garbage);
            if ((choice >= 0) && (choice < MAXINVITEMS)) {
                sellPrice = (price(player.inventory[choice]) / 5);
                if (sellPrice > 0) { //we'll see if everything ends up being worthless
                    //remove from inventory
                    printf("\nYou sell %s for %d coins.", player.inventory[choice].name, sellPrice);
                    for (int i = choice; i < (MAXINVITEMS - 1); i++)
                        player.inventory[i] = player.inventory[i + 1];
                    player.inventory[MAXINVITEMS - 1] = blankItem;
                    //add coins
                    for (int i = 0; i < sellPrice; i++)
                        addCoins();
                }
                else printf("\n%s isn't worth anything.", player.inventory[choice].name);
            }
            else printf("\nI don't understand that choice.");
        }
        else if (input == 'n') {
            break;
        }
        else printf("\nWhat?"); //some other input
    }
    printf("\nThanks for stopping by.");
}

void innInteraction() {
    char choice = 'a', garbage;
    int roomCost = 3;
    while (choice != 'n') {
        printf("\nWelcome to the Dew Drop Inn. Won't you stay a while?");
        printf("\n%d coins for a room to rest in.", roomCost);
        printf("\n<p>ay or <n>evermind? ");
        scanf("%c%c", &choice, &garbage);
        if (choice == 'p') {
            if (lastCoinSpot() >= roomCost) {
                printf("\nYou take the room.\n\n...zzzzz...\n");
                sleep(1);
                printf("\nYou awake feeling rested.");
                player.position = 0;
                executeHeal();
                for (int i = 0; i < roomCost; i++)
                    removeCoins();
                executeLook("around");
            }
            else printf("\nYou don't have enough money.");
            break;
        }
        else if (choice == 'n')
            printf("\nWell, if you change your mind, you know where to find us.");
        else printf("\nI don't understand that choice.");
    }
}

void executeUse(char noun[]) {
    int toUse = -1;
    char *token;
    if (noun != NULL) {
        if (strcmp(noun, "potion") == 0) {
            if (pickFromInventory(noun) >= 0) {
                toUse = pickFromInventory(noun);
                token = strtok(player.inventory[toUse].description, " ");
                while (token != NULL) {
                    if (strcmp(token, "green") == 0) {
                        printf("\nYou drink the potion and feel refreshed.");
                        player.hp = player.hp + player.inventory[toUse].defenseBonus;
                        if (player.hp > maxPlayerHP)
                            player.hp = maxPlayerHP;
                        showHP();
                        break;
                    }
                    else if (strcmp(token, "blue") == 0) {
                        printf("\nYou drink the potion and feel refreshed.");
                        player.mp = player.mp + player.inventory[toUse].defenseBonus;
                        if (player.mp > maxPlayerMP)
                            player.mp = maxPlayerMP;
                        showMP();
                        break;
                    }
                    token = strtok(NULL, " ");
                }
                //remove from inventory
                removeFromInventory(toUse);
            }
            else printf("\nYou don't have %s", noun);
        }
        else printf("\nYou can't use %s", noun);
    }
    else printf("\nWhat do you want to use?");
}

void executeExamine(char noun[]) {
    int toUse = -1;
    if (noun != NULL) {
        toUse = pickFromInventory(noun);
        if (toUse>= 0) {
            printf("\n%s", player.inventory[toUse].description);
        }
        else printf("\nYou don't have %s", noun);
    }
    else printf("\nWhat would you like to examine?");
}

void bossInteraction(int boss) {
    bool keepFighting = true;
    char choice, garbage, spellChoice[15], useChoice[15];
    while (keepFighting == true) {
        map[player.position].monstersPresent[boss].armorClass = 100;
        printf("\nWould you like to (a)ttack, (r)age, (u)se item, or (c)ast a spell? ");
        scanf("%c%c", &choice, &garbage);
        switch (choice) {
            case 'a':
                executeAttack(map[player.position].monstersPresent[boss].name);
                break;
            case 'r':
                executeRage(map[player.position].monstersPresent[boss].name);
                break;
            case 'u':
                showInventory();
                printf("\nWhich item (name) would you like to use? ");
                fgets(useChoice, sizeof(useChoice), stdin);
                useChoice[strcspn(useChoice, "\n")] = 0;
                executeUse(useChoice);
                break;
            case 'c':
                map[player.position].monstersPresent[boss].armorClass = 10;
                listSpells();
                printf("\nWhich spell would you like to cast? ");
                fgets(spellChoice, sizeof(spellChoice), stdin);
                spellChoice[strcspn(spellChoice, "\n")] = 0;
                if (strcmp(spellChoice, "heal") == 0) //doesn't let you heal boss
                    executeCast(spellChoice, "self");
                else executeCast(spellChoice, map[player.position].monstersPresent[boss].name);
                break;
            default:
                printf("\nYou can't do that.");
                break;
        }
        printf("\n%s", map[player.position].monstersPresent[boss].reply);
        if ((deadyet(player) == true) || (deadyet(map[player.position].monstersPresent[boss]) == true))
            keepFighting = false;
    }
    if (deadyet(player) == true)
        printf("\nAw man, he killed you.");
    else if (deadyet(map[player.position].monstersPresent[boss]) == true)
        printf("\nYou did it!");
}

void secretDoorInteraction(char spell[]) {
    int spellToUse = -1;
    if (spell != NULL) {
        for (int i = 0; i < 7; i++) { //determine spell number
            if (strcmp(spellBook[i].name, spell) == 0)
                spellToUse = i;
        }
        if (spellToUse >= 0) { //maybe add some strcat'd descr's later?
            if (player.mp >= spellBook[spellToUse].spellCost) {
                printf("\n%splaque.", spellBook[spellToUse].description);
                if (strcmp(spellBook[spellToUse].name, "fireball") == 0) {
                    fourElements[0] = 'f';
                    printf("\nOne of the runes on the plaque glows red.");
                }
                else if (strcmp(spellBook[spellToUse].name, "meteor") == 0) {
                    fourElements[1] = 'e';
                    printf("\nOne of the runes on the plaque glows green.");
                }
                else if (strcmp(spellBook[spellToUse].name, "freeze") == 0) {
                    fourElements[2] = 'w';
                    printf("\nOne of the runes on the plaque glows blue.");
                }
                else if (strcmp(spellBook[spellToUse].name, "tornado") == 0) {
                    fourElements[3] = 'a';
                    printf("\nOne of the runes on the plaque glows white.");
                }
                else printf("\nThe spell does nothing.");
                player.mp = player.mp - spellBook[spellToUse].spellCost;
            }
            else printf("\nYou don't have enough mana.");
        }
        else printf("\nYou don't know that spell.");
        //check to see if all spells have been used
        if (strcmp(fourElements, "fewa") == 0) {
            //door unlocks
            printf("\n\nEach symbol on the plaque glows.");
            printf("\nThere is a great rumbling, as a door opens in the back of the room.\n");
            strcpy(map[player.position].exits, "nsw ");
            strcpy(plaque.description, "A small stone tablet with glowing runes.");
            //show unlocked exits
            executeLook("around");
        }
    }

    else printf("\nWhat do you want to cast on what?");
}

void addXP(int toAdd) {
    player.xp = player.xp + toAdd;
    int addToHP = 0;
    int addToMP = 0;
    //level up every 50xp -- should change to sliding scale
    if (player.xp >= (playerLevel * 50)) {
        printf("\n---Level up!---\n");
        addToHP = maxPlayerHP / 10;
        maxPlayerHP = maxPlayerHP + addToHP;
        player.hp = player.hp + addToHP;
        addToMP = maxPlayerMP / 10;
        maxPlayerMP = maxPlayerMP + addToMP;
        player.mp = player.mp + addToMP;
        playerLevel++;
        showHP();
        showMP();
    }
    showXP();
}
