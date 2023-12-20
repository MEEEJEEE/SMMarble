//
//smm_object.c
//  smm_node.c
//  SMMarble
//
//  Created by MIJI LEE on 2023/12/20.
//
//정의
#include "smm_common.h"
#include "smm_object.h"

#ifndef smm_object.c  //헤더가디언: 중복 포함 방지, 순환 포함 방지
#define smm_object_c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*#define MAX_CHARNAME    50

// 추가: 상수 정의
#define REPLENISH_ENERGY_AT_HOME 10

// 추가: 파일에서 읽은 데이터에 대한 에러 처리를 위한 매크로
#define READ_SUCCESS 4

// smm_object.c 파일 내에서 smmObj_noNode 변수 선언
int smmObj_noNode = 0;
//smmObject_t smm_node[MAX_NODE];
// 추가: 파일에서 읽은 데이터에 대한 에러 처리를 위한 열거형
typedef enum {
    FILE_READ_SUCCESS = 4,
    FILE_READ_FAILURE = 0
} FileReadResult;
*/

// smm_object.c 파일 상단에 전역 변수로 선언되어 있는지 확인
int smmObj_noNode = 0;
smmObject_t smm_node[MAX_NODE];
// 노드의 개수를 반환하는 함수 선언 추가
int getSmmObjNodeCount(void){
	return smmObj_noNode;
}
//노드의 이름을 반환

#endif /* smm_object_h */


const char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
    "lecture",
	"restaurant",
	"laboratory",
	"home",
	"gotolab",
	"foodchance",
	"festival"
};

/*
// 1. 구조체 혈식 정의
typedef struct smmObject {
	char name[MAX_CHARNAME];
    smmObjType_e objType; 
    int type;
    int credit;
    int energy;
    smmObjGrade_e grade;
    int number;  // 노드 번호 추가
} smmObject_t;


//노드 배열 및 노드 개수 변수 정의
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;
*/

//2.관련 함수 변경
//object generation 함수 수정
smmObject_t* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjType_e objtype, smmObjGrade_e grade)
{   

	//먼저 노드 배열이 가득 차 있는지 확인하고, 가득 찼다면 배열이 꽉 찼다는 오류 메시지를 출력하고 프로그램을 종료 
	if (getSmmObjNodeCount() >= MAX_NODE) {
		// 오류 반환
        printf("Error: Node array is full.\n");
        exit(EXIT_FAILURE); // 또는 오류 처리에 맞게 NULL 또는 다른 값을 반환
    } 
    
    // malloc 함수를 사용하여 smmObject_t 구조체의 크기만큼 동적으로 메모리를 할당 
    smmObject_t* ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    if (ptr == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    ptr->number = getSmmObjNodeCount();  //노드 번호 할당 추가
    smm_node[getSmmObjNodeCount()] = *ptr;  // 노드 배열에 할당된 노드 추가
    
    // 메모리를 해제하지 않으면 메모리 누수가 발생함
    // 사용이 끝난 메모리는 반드시 해제되어야 함
    // free 함수를 사용하여 동적으로 할당된 메모리를 해제
    //free(ptr);
    
	return ptr;
}

//3.관련 함수 변경 
// 노드의 이름을 반환


char* smmObj_getGradeName(smmObjGrade_e grade)
{
	return smmGradeName[grade];
}

//4.관련 함수 변경- 각 함수는 노드의 타입, 크레딧, 에너지 값을 반환
int smmObj_getNodeType(int node_nr)
{
    return smm_node[node_nr].type;
}

int smmObj_getNodeCredit(int node_nr)
{
    return smm_node[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
    return smm_node[node_nr].energy;
}

// 노드의 번호를 반환
int smmObj_getNodeNumber(void* nodePtr)
{
	smmObject_t* node = (smmObject_t*)nodePtr;
    return node->number;
}
