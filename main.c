//
//  main.c
//  Implementation of game initialization, board setup, player creation, and game progression
//  SMMarble
//
//  Created by MIJI LEE on 2023/12/19.
//

// Header files and constant definitions
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

// Board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;
static int player_nr;

// Player and game variables and structures
typedef struct player {
    int energy;                  // Energy
    int position;                // Current position
    char name[MAX_CHARNAME];
    int accumCredit;             // Accumulated credits
    int flag_graduate;           // Experiment flag
    int flag_experiment;		 //// ADD : Experiment Status Flag
} player_t;

static player_t *cur_player;

// Function prototypes
void generatePlayers(int player_nr, int initEnergy);
void goForward(int player, int step); // make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); // print all player status at the beginning of each turn
int rolldie(int player);
float calcAverageGrade(int player); // calculate average grade of the player
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
void actionNode(int player);
void opening(void);
void RESTAURANT(int player);
void foodChance(int player);
void graduateCheck(int player);  //check if any player is graduated
void printGraduationResults(int player);

// Main function
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
    
    // Game initialization and data loading
    //1. Import parameters ---------------------------------------------------------------------------------
    //1-1. BoardConfig 
    
 	// File open and error handling
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
		void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    // Traverse the added board nodes and print information
    for (i = 0;i<board_nr; i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    
#if 0
    //1-2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4)  //read a food parameter set
    {
        //store the parameter set
        // Assuming your food card data format is similar to board nodes
	    void* foodCardObj = smmObj_genObject(name, smmObjType_card, type, credit, energy, smmObjGrade_A0);
	    smmdb_addTail(LISTNO_FOODCARD, foodCardObj);
	    food_nr++;
}
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
    
    while  (fscanf(fp, "%s", name) == 1) {
	    // Assuming your festival card data format includes only names
	    void* festCardObj = smmObj_genObject(name, smmObjType_card, 0, 0, 0, smmObjGrade_A0);
	    smmdb_addTail(LISTNO_FESTCARD, festCardObj);
	    festival_nr++;
    }
    
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
#endif
    
    //2. Player configuration ---------------------------------------------------------------------------------
    // Get player number from user input
	do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    } while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    // Allocate memory for players and generate them
    cur_player = (player_t*)malloc(player_nr * sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    opening();
    
    //3. Game loop, SM Marble game starts ---------------------------------------------------------------------------------
    while (1) // Is anybody graduated?
    {
        int die_result;
        
        //3-1. Initial printing
        printf("\n=========================== PLAYER STATUS ===========================\n");
        printPlayerStatus(); // Print all player status at the beginning of each turn
    	
        //3-2. Die rolling (if not in experiment)        
        die_result = rolldie(turn);
        
        //3-3. Go forward
        goForward(turn, die_result);

		//3-4. Take action at the destination node of the board
        actionNode(turn);
        
        //3-6. Next turn
        turn = (turn + 1) % player_nr;

        //3-7.Check if any player has graduated
        graduateCheck(turn);
    }
}

// Generate new players
void generatePlayers(int n, int initEnergy) 
{
     int i;
     
     // 'n' time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i);
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         cur_player[i].position = 0;
         
         //set energy
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;      // Accumulated credits initialization
         cur_player[i].flag_graduate = 0;
         cur_player[i].flag_experiment = 0;  // Initialize experiment flag
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
    // Get the node data for the current player's position
    int type = smmObj_getNodeType(boardPtr); // Get the type of the node
    char *name = smmObj_getNodeName(boardPtr); //// Get the name of the node
    void *gradePtr;
    int turn = 0; //Initialize the turn variable
    
    switch (type)
    {
    //case lecture: 
        case SMMNODE_TYPE_LECTURE:
		{	
			int answer;
            if (cur_player[player].energy >= smmObj_getwNodeEnergy(boardPtr) && !smmObj_isNodeVisited(boardPtr))
			{
				// If the player has enough energy and has not visited the node
				printf("You can take the lecture %s.\n", smmObj_getNodeName(boardPtr));
				 
				 // Prompt the player to decide whether to attend or drop the lecture
	        	printf("Would you like to take the lecture? (Y: 1, N: press any key) - ");
		        scanf("%d", &answer);
		        fflush(stdin);
		        
				if (answer == 1)
       			{
       				// If attending, accumulate credits, deduct energy, and generate a random grade
            		cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

					// Grade generation
            		smmObjGrade_e randomGrade = rand() % smmObjGrade_max; // 랜덤으로 등급 선택
            		gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, smmObjGrade_A0);
            		smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            	
					// Mark the lecture as attended
	                smmObj_setNodeVisited(boardPtr, 1);
	                
	                printf("Successfully attended the lecture %s. Grade: %d\n",smmObj_getNodeName(boardPtr), randomGrade);
	        	}
	        	else
	        	{
            		// If drop a lecture
            		printf("Decided not to attend the lecture %s. Lecture dropped.\n", smmObj_getNodeName(boardPtr));
        		}
        	}
		    else
			{
			    // If not able to take, print the reason
			    if (cur_player[player].energy < smmObj_getNodeEnergy(boardPtr))
			    {
			        printf("Not able to take the lecture %s. Insufficient energy.\n", smmObj_getNodeName(boardPtr));
			    }
			    else if (smmObj_isNodeVisited(boardPtr))
			    {
			        printf("Not able to take the lecture %s. Already attended.\n", smmObj_getNodeName(boardPtr));
			    }
			}	
		}
            break;
            
    //case RESTAURANT:
        case SMMNODE_TYPE_RESTAURANT:
        	// Call the RESTAURANT function
            RESTAURANT(player);
            break;
			

	//case Laboratory:
		case SMMNODE_TYPE_LABORATORY:
	    	// Check if the player is in an experiment state
			if (cur_player[player].flag_experiment == 1)
			{
    			// Randomly set the success threshold for the experiment
    			int successThreshold = rand() % MAX_DIE + 1;
    			printf("The success threshold for the Experiment is %d.\n", successThreshold);

    			// Roll the dice
    			int dieRoll = rand() % MAX_DIE + 1;
    			printf("Dice result value: %d\n", dieRoll);

   				 // Check if the experiment is successful
    			if (dieRoll >= successThreshold){
        			// Experiment success
        			printf("Experiment successful! Ends the Experiment.\n");
        			cur_player[player].flag_experiment = 0; // End the experiment state
    			} else {
        			// Experiment is ongoing
       		 		printf("Experiment is ongoing. Energy is consumed. (remained energy : %i)\n", cur_player[player].energy);
        			int energyConsumed = smmObj_getNodeEnergy(boardPtr);
					cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
    			    
				  }
			}
			break;

	//case Home:
		case SMMNODE_TYPE_HOME:
		//In addition to the current energy as soon as it passes, as much as the supplemental energy specified
			cur_player[player].energy += REPLENISH_ENERGY_AT_HOME;
            printf("%s passed by home and replenished Energy by %d (current energy: %d)\n",
                   cur_player[player].name, REPLENISH_ENERGY_AT_HOME, cur_player[player].energy);
            break;
            
	//case GOTOLAB:
		case SMMNODE_TYPE_GOTOLAB:
            // Transition to the experiment state
            cur_player[player].flag_experiment = 1;
            printf("%s is in the Experiment State.\n", cur_player[player].name);
            
            // Move to the Laboratory
    		cur_player[player].position = SMMNODE_TYPE_LABORATORY;
    		printf("%s goes to the LABORATORY.\n", cur_player[player].name);
            break;
            
	//case Festival:
      	case SMMNODE_TYPE_FESTIVAL:
        	if (festival_nr > 0)
            {
                // Draw a random festival card
                int randomFestCard = rand() % festival_nr;
                void *festCardObj = smmdb_getData(LISTNO_FESTCARD, randomFestCard);
                
                // Print the festival card information
                printf("%s drew a Festival Card: %s\n", cur_player[player].name, smmObj_getNodeName(festCardObj));
                
                // Perform the stated mission.
                //performFestivalMission(festCardObj, player);
            }
            break;
			  
        default:
            break;
	}
}


void RESTAURANT(int player)
{
    void* boardPtr;
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position); //Retrieve node information at the current player's position using the 'smmdb_getData' function 
																		//and store this information in the 'boardPtr'
    
	// Check if the current node is a RESTAURANT and print relevant information
   	if (smmObj_getNodeType(boardPtr) == SMMNODE_TYPE_RESTAURANT) 
	{
    	// Charge the player's energy with the energy provided by the RESTAURANT
		int sumEnergy = cur_player[player].energy + smmObj_getNodeEnergy(boardPtr);
    	cur_player[player].energy = sumEnergy;
	
		printf(" Eat in %s and charge %i energies (remained energy : %i)\n", 
				smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr), sumEnergy);
    }
}

//  Function to check and handle food chance event
void foodChance(int player) {
    char c;
    void* boardPtr;
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	// Check if the current node is FOODCHANCE
    if (smmObj_getNodeType(boardPtr) == SMMNODE_TYPE_FOODCHANCE) 
	{
        printf("  ==> %s Food Replenishment Chance! Press any key to pick a food card! ", cur_player[player].name);
        c = getchar();
        fflush(stdin);
        
        // Choose a random food card index
        int randomIndex = rand() % food_nr;
        void* foodCardPtr = smmdb_getData(LISTNO_FOODCARD, randomIndex);
        
        printf("   ==> %s picks! %s replenishing %s energy.\n", 
				cur_player[player].name,
				smmObj_getNodeName(foodCardPtr),
				smmObj_getNodeCharge(foodCardPtr));
				
        cur_player[player].energy += smmObj_getNodeCharge(foodCardPtr);  // Update player's energy after picking a food card
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
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
     
     // Check if boardPtr is NULL to verify if it's a valid node
    if (boardPtr != NULL)
    {
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
	}
	
	else
    {
        printf("%s reached an invalid node.\n", cur_player[player].name);
        // Handle exceptions or errors when arriving at an invalid node
    }
}

    


// Game start message
void opening(void) {
    printf("\n\n=============================================================================\n\n");
    printf("        		     ** Welcome to Sookmyung Marble Game!! **            	       \n");
    printf("              	     Let's Graduate with a Total Credit of 30     			       \n");
    printf("\n=============================================================================\n\n\n");
} 


// Function to check if any player has graduated
void graduateCheck(int player)
{
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT)
    {
        printf("\n============================== GAME OVER ==============================\n");
        printf("%s has graduated! Congratulations!\n", cur_player[player].name);
        printGraduationResults(player);
        printf("========================================================================\n\n");

        // Free memory and exit the program
        free(cur_player);
        smmdb_freeAllLists();
        system("PAUSE");
        exit(0);
    }
}

// Function to print graduation results
void printGraduationResults(int player)
{
    // Print the player's name and graduation results
    printf("%s's Graduation Results:\n", cur_player[player].name);

    // Iterate through the grades of the player
    for (int i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
    {
        void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        printf("Lecture: %s, Credit: %d, Grade: %d\n",
               smmObj_getNodeName(gradePtr),
               smmObj_getNodeCredit(gradePtr),
               smmObj_getNodeGrade(gradePtr));
    }
    printf("\n");
}