//
//  main.c
//게임의 초기설정, 보드구성,플레이어 생성,게임 진행등 구현 
//  SMMarble
//
//  Created by MIJI LEE on 2023/12/19.
//

//헤더 파일 및 상수 정의
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define MAX_PLAYER     100

// board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

//플레이어 및 게임 변수 및 구조체
typedef struct player {
    int energy;
    int position;
    char name[MAX_CHARNAME];
    int accumCredit;
    int flag_graduate;
} player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
int isGraduated(void); //check if any player is graduated
 //print grade history of the player
void generatePlayers(int player_nr, int initEnergy);
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
int rolldie(int player);
float calcAverageGrade(int player); //calculate average grade of the player
//smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
void actionNode(int player);
void opening(void);
void cafeteria(int player);
void foodChance(int player);
void graduateCheck(int player);
void printGraduationResults(void);


int main(int argc, const char * argv[]) 
{
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    //게임 초기 설정 및 데이터 로딩
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    #if 0
    //1-2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //1-3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr * sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    //3. Game loop, SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int die_result;
        
        
        //3-1. initial printing
        printf("\n=========================== PLAYER STATUS ===========================\n");
         
        printPlayerStatus(); //[Error] too many arguments to function 'printPlayerStatus' 수정
    		
    	printf("============================== PLAYER STATUS ===========================\n\n");
        
        //3-2. die rolling (if not in experiment)        
        die_result = rolldie(turn);
        
        //3-3. go forward
        goForward(turn, die_result);

		//3-4. take action at the destination node of the board
        actionNode(turn);
        
        cafeteria(turn);
        foodChance(turn);
        
        
        //3-5. next turn
        turn = (turn + 1) % player_nr;
        
        //3-6.Check if any player has graduated
        graduateCheck(turn);
    }
    
//////////////////////////////////////////////////////////////////////////////////////////   
// Generate new players
void generatePlayers(int n, int initEnergy) 
{
     int i;
     
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i);
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0; //누적 학점 초기화
         cur_player[i].flag_graduate = 0;
     }
}

void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}
// Print player status
void printPlayerStatus(void)
{
     int i;
     
     for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
}


// Action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    //int type = smmObj_getNodeType( cur_player[player].position );
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;
    
    switch (type)
    {
        //case lecture: 
        case SMMNODE_TYPE_LECTURE:
			if(cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
			{ 
            	cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            	cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
            
            	//grade generation
            	gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, smmObjGrade_A0);
            	smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        	}
            break;
/*        // case  CAFETERIA:
        case SMMNODE_TYPE_CAFETERIA
void CAFETERIA(int player)
{
    void* boardPtr;
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int sumEnergy = cur_player[player].energy + smmObj_getNodeEnergy(boardPtr);
    
    // Check if the current node is a cafeteria and print relevant information
    if (smmObj_getNodeType(boardPtr) == SMMNODE_TYPE_CAFETERIA) {
        printf("    Eat in %s and charge %i energies (remained energy : %i)\n", smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr), sumEnergy);
    }
}
*/

	//case Laboratory:
	//case Home:
	//case Experiment:

// 플레이어가 게임 보드에서 음식 이벤트에 참여하도록 하는 기능
void foodChance(int player) {
    char c;
    void* boardPtr;
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);

    if (smmObj_getNodeType(boardPtr) == SMMNODE_TYPE_FOOD) {
        printf("  ==> %s Food Replenishment Chance! Press any key to pick a food card! ", cur_player[player].name);
        c = getchar();
        fflush(stdin);
        int randomIndex = rand() % food_nr;  // Choose a random food card index
        void* foodCardPtr = smmdb_getData(LISTNO_FOODCARD, randomIndex);
        int sumEnergy = cur_player[player].energy + smmObj_getNodeCharge(foodCardPtr);

        printf("   ==> %s picks! %s and charges %i (remained energy : %i)\n", cur_player[player].name, smmObj_getNodeName(foodCardPtr), smmObj_getNodeCharge(foodCardPtr), sumEnergy);
        cur_player[player].energy += smmObj_getNodeCharge(foodCardPtr);  // Update player's energy after picking a food card
    }
}

	//case Festival:
      
        default:
            break;
    }
}


// Roll a die
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
	while (getchar() != '\n');
	
    if (c == 'g')
        printGrades(player);
#endif
    // Generate a random number between 1 and 6
    return (rand() % 6) + 1;
}



void goForward(int player, int step)
{
     void *boardPtr;
     cur_player[player].position += step;
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr);
}

    free(cur_player);
    system("PAUSE");
    return 0;
}

// Game start message
void opening(void) {
    printf("\n\n=============================================================================\n\n");
    printf("             **Sookmyung Marble** Let's Graduate!! (total credit : 30)             \n");
    printf("\n=============================================================================\n\n\n");
} 


// Function to check if any player has graduated
void graduateCheck(int player)
{
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT)
    {
        printf("\n============================== GAME OVER ==============================\n");
        printf("%s has graduated! Congratulations!\n", cur_player[player].name);
        printGraduationResults();
        printf("========================================================================\n\n");

        // Free memory and exit the program
        free(cur_player);
        smmdb_freeAllLists();
        system("PAUSE");
        exit(0);
    }
}

// Function to print graduation results
void printGraduationResults(void)
{
    int i;
    for (i = 0; i < player_nr; i++)
    {
        if (cur_player[i].flag_graduate)
        {
            printf("%s's Graduation Results:\n", cur_player[i].name);
            printGrades(i); // Assuming this function prints the grades of the player
            printf("\n");
        }
    }
}