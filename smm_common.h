//
//  smm_common.h
//  SMMarble
//
//  Created by MIJI LEE on 2023/12/19.
//

#ifndef smm_common_h
#define smm_common_h

#include <stdio.h>
#include <stdlib.h>

#define MAX_CHARNAME                200

#define GRADUATE_CREDIT             30
#define MAX_DIE                     6
#define MAX_PLAYER                  100

//코드 내에서 사용되는 노드 타입과 학점의 최대 갯수를 명시적으로 정의
#define MAX_NODETYPE        7
#define MAX_GRADE           9

//배열들을 선언
extern const char *smmNodeName[MAX_NODETYPE];
extern const char *smmGradeName[MAX_GRADE];

#endif /* smm_common_h */