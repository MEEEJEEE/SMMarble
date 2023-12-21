//
//  smm_object.h
//  SMMarble object
//
//  Created by MIJI LEE on 2023/12/20.
//
//선언만
#ifndef smm_object_h
#define smm_object_h

#include "smm_common.h"
#include <stdio.h>

//노드 유형 정의 
#define SMMNODE_TYPE_LECTURE            0
#define SMMNODE_TYPE_RESTAURANT         1
#define SMMNODE_TYPE_LABORATORY         2
#define SMMNODE_TYPE_HOME               3
#define SMMNODE_TYPE_GOTOLAB            4
#define SMMNODE_TYPE_FOODCHANCE         5
#define SMMNODE_TYPE_FESTIVAL           6

#define MAX_CHARNAME 200  // 적절한 값으로 수정
#define SMMNODE_TYPE_MAX                7
#define MAX_NODETYPE   				    7
#define MAX_GRADE      				    9
#define REPLENISH_ENERGY_AT_HOME 		10
#define MAX_NODE        				100
#define MAX_PLAYERS 					4
// 객체 유형을 위한 열거형
typedef enum smmObjType {
    smmObjType_board = 0,
    smmObjType_card,
    smmObjType_grade,
    smmObjType_max // 추가: 객체 유형의 최대값
} smmObjType_e;

// 객체 등급 유형을 위한 열거형
typedef enum smmObjGrade {
    smmObjGrade_Ap = 0,
    smmObjGrade_A0,
    smmObjGrade_Am,
    smmObjGrade_Bp,
    smmObjGrade_B0,
    smmObjGrade_Bm,
    smmObjGrade_Cp,
    smmObjGrade_C0,
    smmObjGrade_Cm,
    smmObjGrade_max // 추가: 객체 등급의 최대값
} smmObjGrade_e;

// 구조체 선언
typedef struct smmObject {
	char name[MAX_CHARNAME];
    smmObjType_e objType; 
    int type;
    int credit;
    int energy;
    smmObjGrade_e grade;
    int number;  // 노드 번호 추가
} smmObject_t;

smmObject_t* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade);

//element to string
char* smmObj_getNodeName(void* nodePtr) {
    smmObject_t* node = (smmObject_t*)nodePtr;
    return node->name;
}

//object generation
void smmObj_genNode(char* name, int type, int credit, int energy);
smmObjGrade_e smmObj_getNodeGrade(void* nodePtr);

#endif /* smm_object_h */


//member retrieving
//char* smmObj_getNodeName(int nodePtr);
int smmObj_getNodeType(int node_nr);
int smmObj_getNodeCredit(int node_nr);
int smmObj_getNodeEnergy(int node_nr);
int smmObj_getNodeNumber(void* nodePtr);

//element to string
char* smmObj_getTypeName(int type);


