#pragma once
/*
*************************************************************************************
     DEADASSIGN.H : DECLARATION OF FUNCTIONS ADDED IN DEADASSIGN.C HERE
**************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Expression.h"

/*
*****************************I*******************************************************
 THIS FLAG VARIBALE WILL BE USEFUL TO ITERATE UNTILL THERE ARE NO UPDATES
 i.e. ALL DEAD CODE IS REMOVED.
**************************************************************************************
*/
extern int change;

/*
*************************************************************************************
  THIS STRUCTURE CAN BE USED TO STORE THE REFERENCED VARIABLES
**************************************************************************************
*/

typedef struct _refVar {
    char* name;
    size_t line;
    struct _refVar* next;
} refVar;

extern refVar *last, *head;


/*
***********************************************************************
  FUNCTION DECLARATIONS
************************************************************************
*/

void init();
void FreeList();
void TrackRef(Node* funcNode);
size_t VarExists(char* name);
void UpdateRefVarList(char* name, size_t stmtCounter);
void PrintRefVarList();
void UpdateRef(Node* node, size_t stmtCounter);
// void TrackExpr(Node* funcNode);
NodeList* RemoveDead(NodeList* statements); 
bool DeadAssign(NodeList* funcdecls);


/*
***********************************************************************
  ADD DECLARATIONS OF ANY FUNCTIONS YOU ADD BELOW THIS LINE
************************************************************************
*/
