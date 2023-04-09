/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/
bool PropogateConst(NodeList* statements);
bool ChangeNode(Node* node, Node* parent, bool right);
/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while (node != NULL){
        if (!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
    Node* node;
    while (statements != NULL) {
        node = statements->node;
        if (node->stmtCode == ASSIGN && node->right->exprCode == CONSTANT && LookupConstList(node->name) == NULL) {
            UpdateConstList(node->name, node->right->value);    
        }
        statements = statements->next;
    }
}

bool PropogateConst(NodeList* statements) {
    Node* node;
    bool madeChange = false;
    refConst* const_node;
    while (statements != NULL) {
        node = statements->node;
        if (node->stmtCode == RETURN) {
            statements = statements->next;
            continue;
        }
        else if (node->right->exprCode == VARIABLE) {
            if (ChangeNode(node->right, node, true)) { madeChange = true; }
        }
        else if (node->right->exprCode == OPERATION && node->right->opCode == FUNCTIONCALL) {
            NodeList* arg_list = node->right->arguments;
            while (arg_list != NULL) {
                if (arg_list->node->exprCode == VARIABLE) {
                    const_node = LookupConstList(arg_list->node->name);
                    if (const_node != NULL) {
                        FreeExpression(arg_list->node);
                        arg_list->node = CreateNumber(const_node->val);
                        madeChange = true;
                    }
                }
                arg_list = arg_list->next;
            }
        }
        else if (node->right->exprCode == OPERATION) {
            if (node->right->left != NULL && node->right->left->name != NULL) {
                if (ChangeNode(node->right->left, node->right, false)) { madeChange = true; }
            }
            if (node->right->right != NULL && node->right->right->name != NULL) {
                if (ChangeNode(node->right->right, node->right, true)) { madeChange = true; }
            }
        }
        statements = statements->next;
    }
    return madeChange;
}

bool ChangeNode(Node* node, Node* parent, bool right) {
    refConst* const_node = LookupConstList(node->name);
    if (const_node != NULL) {
        FreeExpression(node);
        if (right) {
            parent->right = CreateNumber(const_node->val);
        }
        else {
            parent->left = CreateNumber(const_node->val);
        }
        return true;
    }
    return false;
}

bool ConstProp(NodeList* funcdecls) {
    bool madeChange = false;
    while (funcdecls != NULL){
        TrackConst(funcdecls->node->statements);
        if (PropogateConst(funcdecls->node->statements)) {
            madeChange = true;         
        }
        FreeConstList();
        funcdecls = funcdecls->next;
    }
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
