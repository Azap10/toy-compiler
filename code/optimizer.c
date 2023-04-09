#include "optimizer.h"
#include "Expression.h"

void Optimizer(NodeList *funcdecls) {
    bool madeChange = true;
    while (madeChange) {
        madeChange = false;
        if (ConstantFolding(funcdecls) || ConstProp(funcdecls) || DeadAssign(funcdecls)) {
            madeChange = true;
        }
        // PrintFunctionDecl(funcdecls->node);
        // PrintFunctionDecl(funcdecls->next->node);
    }
    return;
}
