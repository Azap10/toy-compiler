/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp->name);
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
size_t VarExists(char* name) {
   refVar *node;
   node = head;
   while (node != NULL) {
       if (!strcmp(name, node->name)) {
           return node->line;
       }
       node = node->next;
    }
    return 0;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name, size_t stmtCounter) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = malloc(sizeof(name));
    strcpy(node->name, name);
    node->line = stmtCounter;
    node->next = NULL;
    if (head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if (node == NULL) {
        printf("\nList is empty"); 
        return;
    }
    while (node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node, size_t stmtCounter) {
      if (node->right != NULL && node->right->exprCode == VARIABLE) {
          if (!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name, stmtCounter);
          }
      }
      if (node->left != NULL && node->left->exprCode == VARIABLE) {
          if (!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name, stmtCounter);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
    NodeList* statements = funcNode->statements;
    Node *node;
    size_t stmtCounter = 0;
    while (statements != NULL) {
        node = statements->node;
        if (node->stmtCode == RETURN || node->right->exprCode == VARIABLE) {
            UpdateRef(node, stmtCounter);
        }
        else if (node->right->exprCode == OPERATION && node->right->opCode == FUNCTIONCALL) {
            NodeList* arg_list = node->right->arguments;
            while (arg_list != NULL) {
                node = arg_list->node;
                if (node->exprCode == VARIABLE && !VarExists(node->name)) {
                    UpdateRefVarList(node->name, stmtCounter);
                }
                arg_list = arg_list->next;
            }
        }
        else if (node->right->exprCode == OPERATION) {
            UpdateRef(node->right, stmtCounter);
        }
        stmtCounter++;
        statements = statements->next;
    }
}

/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    NodeList *prev, *tmp, *first;
    if (statements == NULL) {
        return NULL;
    }
    size_t stmtCounter = 0;
    NodeList* tmp_statement;
    first = statements;
    prev = first;
    statements = prev->next;
    while (statements != NULL) {
        if (statements->node->stmtCode == ASSIGN && !VarExists(statements->node->name)) {
            tmp = statements->next;
            tmp_statement = statements;
            FreeStatement(statements->node);
            free(tmp_statement);
            prev->next = tmp;
            change = 1;
        }
        prev = prev->next;
        stmtCounter++;
        statements = prev->next;
    }
    if (first->node->stmtCode == ASSIGN && !VarExists(first->node->name)) {
        tmp = first->next;
        tmp_statement = first;
        FreeStatement(first->node);
        free(tmp_statement);
        return tmp;
        change = 1;
    }
    return first;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* funcdecls) {
    bool madeChange = false;
    while (funcdecls != NULL) {
        TrackRef(funcdecls->node);
        funcdecls->node->statements = RemoveDead(funcdecls->node->statements);
        FreeList();
        funcdecls = funcdecls->next;
    }
    if (change == 1) madeChange=true;
    change = 0;
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
