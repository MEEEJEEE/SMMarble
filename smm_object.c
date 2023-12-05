//
//  smm_node.c
//  SMMarble
//
//  Created by MIJI LEE on 2023/12/04.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
       "����",
       "�Ĵ�",
       "�����",
       "��",
       "����Ƿ��̵�",
       "��������",
       "�����ð�"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}

//1. ����ü ���� ����
typedef struct smmObject {
       char name[MAX_CHARNAME];
       int type;
       int credit;
       int energy;
} smmObject_t;

//2. ����ü �迭 ���� ���� 
static smmObject_t smm_node[MAX_NODE];

static int smmObj_noNode = 0;

//3. ���� �Լ� ���� 
//object generation
void smmObj_genNode(char* name, int type, int credit, int energy)
{    
    strcpy(smm_node[smmObj_noNode].name, name);
    smm_node[smmObj_noNode].type = type;
    smm_node[smmObj_noNode].credit = credit;
    smm_node[smmObj_noNode].energy = energy;
    
    smmObj_noNode++;
}

//3. ���� �Լ� ���� 
char* smmObj_getNodeName(int node_nr)
{
    return smm_node[node_nr].name;
}

//3. ���� �Լ� ���� 
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