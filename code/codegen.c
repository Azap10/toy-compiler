/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"
#include "Expression.h"

int argCounter;
int lastUsedOffset;
int stackFrameBytes;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;
char* argument_registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if (!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if (varHead == NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }
    varLast = NULL;
    varList = NULL;
}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (varList->isConst == true) {
            if (varList->value == val) return varList->location;
        }
        else {
            if (!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if (!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if (varList == NULL) printf("NULL varlist");
       while (varList != NULL) {
           if (!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if (regHead == NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while (regList != NULL) {
        if (regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            if (!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if (noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            // registers available
            if (!noAcc) return 1;
            if (noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (!strcmp(regList->regName, name)) {
           if (regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    return;
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
void PopArgFromStack(NodeList* arguments) {
    size_t register_index = argCounter - 1;
    while (arguments != NULL) {
        fprintf(fptr, "\npopq %%%s", argument_registers[register_index]); 
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE 
      FIRST ARGUMENT OF A FUNCTION STORED.
     ************************************************************************
     */ 
        register_index--;
        arguments = arguments->next;
    }
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
int PushArgOnStack(NodeList* arguments) {
    argCounter = 0;
    char location[10];
    while (arguments != NULL) {
        fprintf(fptr, "\npushq %%%s", argument_registers[argCounter]); 
        sprintf(location, "-%d(%%rbp)", stackFrameBytes);
        AddVarInfo(arguments->node->name, location, arguments->node->value, false);
        argCounter++;
        stackFrameBytes += 8;
        arguments = arguments->next;
    }
    return argCounter;
}

/*
 ************************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    char location[10];
    while (statements != NULL) {
        Node* stmtNodeRight = statements->node->right;
        if (statements->node->stmtCode == RETURN) {
            if (statements->node->left->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $0x%x, %%rax", (u_int) statements->node->left->value);                         
            }
            else if (statements->node->left->exprCode == VARIABLE){
                fprintf(fptr, "\nmovq %s, %%rax", LookUpVarInfo(statements->node->left->name, 0xDEADBEEF));                         
            }
            break;
        }
        else if (stmtNodeRight->exprCode == OPERATION && stmtNodeRight->opCode == FUNCTIONCALL) {
            NodeList* arg_list = stmtNodeRight->arguments;
            Node* arg_node;
            size_t arg_idx = 0;
            while (arg_list != NULL) {
                arg_node = arg_list->node;
                if (arg_node->exprCode == CONSTANT) {
                    fprintf(fptr, "\nmovq $0x%x, %%%s", (u_int) arg_node->value, argument_registers[arg_idx]);
                }                
                else if (arg_node->exprCode == VARIABLE) {
                    fprintf(fptr, "\nmovq %s, %%%s", LookUpVarInfo(arg_node->name, 0xDEADBEEF), argument_registers[arg_idx]);
                }
                arg_idx++;
                arg_list = arg_list->next;
            }
            fprintf(fptr, "\ncallq %s", stmtNodeRight->left->name);
            sprintf(location, "-%d(%%rbp)", stackFrameBytes);
        }
        else if (stmtNodeRight->exprCode == OPERATION &&
                (stmtNodeRight->opCode == NEGATE || stmtNodeRight->opCode == BSHR || stmtNodeRight->opCode == BSHL || stmtNodeRight->opCode == DIVIDE || stmtNodeRight->opCode == SUBTRACT)) {
            if (stmtNodeRight->left->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $0x%x, %%rax", (u_int) stmtNodeRight->left->value);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rax", LookUpVarInfo(stmtNodeRight->left->name, 0xDEADBEEF));
            }
            if (stmtNodeRight->right != NULL && stmtNodeRight->right->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $0x%x, %%r8", (u_int) stmtNodeRight->right->value);
            }
            else if (stmtNodeRight->right != NULL) {
                fprintf(fptr, "\nmovq %s, %%r8", LookUpVarInfo(stmtNodeRight->right->name, 0xDEADBEEF));
            }
            switch (stmtNodeRight->opCode) {
                case SUBTRACT:
                    fprintf(fptr, "\nsubq %%r8, %%rax");
                    break;
                case NEGATE:
                    fprintf(fptr, "\nnegq %%rax");
                    break;
                case DIVIDE:
                    fprintf(fptr, "\ncqto");
                    fprintf(fptr, "\nidivq %%r8");
                    break;
                case BSHR:
                    fprintf(fptr, "\nsarq $%d, %%rax", (int) stmtNodeRight->right->value);
                    break;
                case BSHL:
                    fprintf(fptr, "\nsalq $%d, %%rax", (int) stmtNodeRight->right->value);
                    break;
                default:
                    break;
            }
        }
        else if (stmtNodeRight->exprCode == OPERATION) {
            if (stmtNodeRight->left->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $0x%x, %%r8", (u_int) stmtNodeRight->left->value);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%r8", LookUpVarInfo(stmtNodeRight->left->name, 0xDEADBEEF));
            }
            if (stmtNodeRight->right->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $0x%x, %%rax", (u_int) stmtNodeRight->right->value);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rax", LookUpVarInfo(stmtNodeRight->right->name, 0xDEADBEEF));
            }
            switch (stmtNodeRight->opCode) {
                case MULTIPLY:
                    fprintf(fptr, "\nimulq %%r8, %%rax");
                    break;
                case ADD:
                    fprintf(fptr, "\naddq %%r8, %%rax");
                    break;
                case BOR:
                    fprintf(fptr, "\norq %%r8, %%rax");
                    break;
                case BAND:
                    fprintf(fptr, "\nandq %%r8, %%rax");
                    break;
                case BXOR:
                    fprintf(fptr, "\nxorq %%r8, %%rax");
                    break;
                default:
                    break;
            } 
        }
        else if (stmtNodeRight->exprCode == VARIABLE) {
            fprintf(fptr, "\nmovq %s, %%rax", LookUpVarInfo(stmtNodeRight->name, 0xDEADBEEF));
        }
        else if (stmtNodeRight->exprCode == CONSTANT) {
            fprintf(fptr, "\nmovq $0x%x, %%rax", (u_int) stmtNodeRight->value);
        }
        sprintf(location, "-%d(%%rbp)", stackFrameBytes);
        UpdateVarInfo(statements->node->name, location, 0xDEADBEEF, false);
        fprintf(fptr, "\npushq %%rax");
        stackFrameBytes += 8;
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    // TODO: something?
    if (fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    Node* node;
    CreateRegList();
    while (worklist != NULL) {
        node = worklist->node; 
        InitAsm(node->name);
        stackFrameBytes += 8;
        PushArgOnStack(node->arguments);
        // fprintf(fptr, "\nsubq $0x%x, %%rsp", argCounter * 8);
        ProcessStatements(node->statements);
        stackFrameBytes -= 8;
        fprintf(fptr, "\naddq $0x%x, %%rsp", stackFrameBytes);
        RetAsm();
        stackFrameBytes = 0;
        FreeVarList();
        worklist = worklist->next; 
    }
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/
