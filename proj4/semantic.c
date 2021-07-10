#include "proj2.h"
#include "proj3.h"
/* 用来作语义检查 */
typedef struct{
    tree type;   /* type tree的type */
    int dim;     /* type tree的dim */
} INFO;
/* 存放相关信息 */
INFO storeinfo(tree type,int dim){
    INFO info;
    info.type = type;
    info.dim = dim;
    return info;
}
extern char* String_table;
void do_semantic(tree);
void traverse(tree);
void classdefop(tree);
void bodyopinclass(tree);
void bodyopindecl(tree);
void methodop(tree);
void declop(tree);
INFO varinit(tree);
INFO commaopofarray(tree, tree);
INFO boundopofarray(tree);
void stmtlist(tree);
void assignstmt(tree);
INFO methodcallstmt(tree, tree);
void returnstmt(tree);
void ifstmt(tree);
void whilestmt(tree);
INFO expression(tree);
INFO simplexpression(tree);
INFO term(tree);
INFO factor(tree);
INFO varop(tree);
int typeidop(tree);
int countargnum(tree);
int globalmethodindex;
int globalvariableindex;
bool multimain = false;

/* 从根节点向左递归遍历，然后遍历classdefop */
void traverse(tree node){
    if(!IsNull(node->LeftC->LeftC)){
		traverse(node->LeftC);
    }
    classdefop(node->LeftC->RightC);
}

void classdefop(tree node){
    int nSymInd = IDtoSTNode(&node->RightC, DECLARE, 0);     /* 对应的IDNode */
    if(nSymInd != 0){
        SetAttr(nSymInd, KIND_ATTR, CLASS);
        if(!IsNull(node->LeftC)){
            OpenBlock();
            bodyopinclass(node->LeftC);    /* 遍历body */
            CloseBlock();
        }
    }
}

/* 根据proj2附录所给classbody语法树: */
void bodyopinclass(tree node){
    if(node->RightC->NodeOpType == DeclOp){
        bodyopindecl(node);
        return;
    }
    if(!IsNull(node->LeftC)){
        bodyopinclass(node->LeftC);
    }
    if(node->RightC != NullExp()){
        methodop(node->RightC);
    }
}

/* 根据proj2附录所给decls语法树: */
void bodyopindecl(tree node){
    if(!IsNull(node->LeftC)){
        bodyopindecl(node->LeftC);
    }
    declop(node->RightC);
}

void methodop(tree node){
    int nStrInd = IntVal(node->LeftC->LeftC);
    if(strcmp(String_table + nStrInd,"main") == 0){
        if(multimain){
            error_msg(RE_MAIN, CONTINUE, 0, 0);
            return;     /* 说明main函数先前已被定义 */
        }
        else{
            multimain = true;
        }
    }
    int nSymInd = IDtoSTNode(&node->LeftC->LeftC, DECLARE, 0);    /* 对应的IDNode */
    if(nSymInd != 0){          
        tree typenode = node->LeftC->RightC->RightC;
        if(typenode == NullExp()){
            SetAttr(nSymInd, KIND_ATTR, PROCE);       /* 说明这个方法是main函数，设置对应属性 */
        }
        else{
            SetAttr(nSymInd, KIND_ATTR, FUNC);        /* 说明这个方法是其他函数，设置对应属性 */
            SetAttr(nSymInd, TYPE_ATTR, typenode->LeftC);
            int dim = typeidop(typenode);
            if(dim != 0){
                SetAttr(nSymInd, DIMEN_ATTR, dim);
            }
        }
        globalmethodindex = nSymInd;
        OpenBlock();
        SetAttr(nSymInd, ARGNUM_ATTR, countargnum(node->LeftC->RightC->LeftC));  /* 统计方法所需参数个数 */
        if(node->RightC->LeftC != NullExp()){
            bodyopindecl(node->RightC->LeftC);
        }
        stmtlist(node->RightC->RightC);
        SetAttr(nSymInd, TREE_ATTR, node);
        CloseBlock();
    }
}

void declop(tree node){
    if(!IsNull(node->LeftC)){
        declop(node->LeftC);
    }
    int nSymInd = IDtoSTNode(&node->RightC->LeftC, DECLARE, 0);       /* 对应的IDNode */
    if(nSymInd != 0){     
        tree typenode = node->RightC->RightC->LeftC;         /* 类型的子树（可见proj2附录fielddecl语法树） */
        int dim = typeidop(typenode);
        SetAttr(nSymInd, TYPE_ATTR, typenode->LeftC);
        if(node->RightC->RightC->RightC != NullExp()){             /* 说明已经初始化了 */
            INFO info = varinit(node->RightC->RightC->RightC);
            if(!(info.type == typenode->LeftC && info.dim == dim)){               /* 说明此时为字符串赋值了数字，报语义错误 */
                error_msg(STRING_ASSIGN, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);
            }
        }
        if(dim == 0){
            SetAttr(nSymInd, KIND_ATTR, VAR);          /* dim为0，说明只是个值变量 */
        }
        else{
            SetAttr(nSymInd, KIND_ATTR, ARR);          /* 不为0，说明是一个数组 */
            SetAttr(nSymInd, DIMEN_ATTR, dim);          /* 设置数组维度属性 */
        }
    }
}

/* 变量初始化，见VariableInitializer语法树 */
INFO varinit(tree node){
    if(node->NodeOpType == ArrayTypeOp){             /* 如果是数组类型 */
        if(node->RightC->NodeOpType == TypeIdOp){    /* 数组初始化 */
            return commaopofarray(node->LeftC, node->RightC->LeftC);
        }
        return boundopofarray(node->LeftC);           /* 添加数组表达式 */
    }
    return expression(node);        /* 是表达式类型 */
}

/* ArrayInitializer */
INFO commaopofarray(tree node, tree type){
    if(node->LeftC == NullExp()){
        INFO info = varinit(node->RightC);
        if(info.dim != -1) info.dim++;
        return info;
    }
    INFO info1 = commaopofarray(node->LeftC, type);   /* 递归左子树 */
    if(info1.dim != -1){
        INFO info = varinit(node->RightC);
        if(info.type == info1.type && info.dim == info1.dim - 1){
            return info1;
        }
    }
    info1.dim = -1;
    return info1;
}

/* ArrayCreationExpression */
INFO boundopofarray(tree node){
    INFO info = storeinfo(INTEGERT(),0);
    if(node->LeftC != NullExp()){
        info.dim = boundopofarray(node->LeftC).dim;
    }
    INFO info1 = expression(node->RightC);
    if(!(info1.type == INTEGERT() && info1.dim == 0)){  /* 索引不是整型，报语义错误 */
        error_msg(OPER_MIS, CONTINUE, IndexOp, 0);
    }
    ++info.dim;
    return info;
}

/* 状态语句，对不同情况讨论即可 */
void stmtlist(tree node){
    if(node->LeftC != NullExp()){
        stmtlist(node->LeftC);
    }
    switch(node->RightC->NodeOpType){
        case AssignOp: assignstmt(node->RightC); break;
        case RoutineCallOp: methodcallstmt(node->RightC, PROCE); break;
        case ReturnOp: returnstmt(node->RightC); break;
        case IfElseOp: ifstmt(node->RightC); break;
        case LoopOp: whilestmt(node->RightC);
    }
}

/* 赋值语句，要判断等号左侧和右侧是否类型、维度匹配的语义问题 */
void assignstmt(tree node){
    INFO infoofleftC = varop(node->LeftC->RightC);
    INFO infoofrightC = expression(node->RightC);
    if(infoofleftC.dim != -1 && infoofrightC.dim != -1){
    	if(infoofleftC.type != infoofrightC.type){
            error_msg(STRING_ASSIGN, CONTINUE, GetAttr(node->LeftC->RightC->LeftC->IntVal, NAME_ATTR), 0);
        }
    	else if(infoofleftC.dim != infoofrightC.dim){
            error_msg(INDX_MIS, CONTINUE, GetAttr(node->LeftC->RightC->LeftC->IntVal, NAME_ATTR), 0);
        }
    }
}

INFO methodcallstmt(tree node, tree method){
    INFO info = varop(node->LeftC);
    if(info.dim == -1){
        return info;             /* 证明没有问题，可以直接返回 */
    }
    int nSymInd = globalvariableindex;          /* 全局变量，可以通过其获取method的相应符号表索引，可以通过其直接获取属性 */
    int variable = GetAttr(nSymInd, KIND_ATTR);
    int vartype,varmissmth,temp,flag=0;
    flag = (info.dim==0?0:1);                         /* 根据proj2附录语法树可知dim>0 */
    vartype = (variable != FUNC && variable != PROCE);      /* 必须为FUNC、PROCE之一 */
    varmissmth = (variable == PROCE && method == FUNC);    /* 变量和函数返回值不匹配 */
    temp = vartype||varmissmth||flag;
    if(temp){         /* temp=1证明有语义错误 */
    	if(method == PROCE){
    		error_msg(PROCE_MISMATCH, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);
    	}
    	else if(method == FUNC){
    		error_msg(FUNC_MISMATCH, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);
    	}
    }
    tree child = node->RightC;
    for (int i = 0; i < GetAttr(nSymInd, ARGNUM_ATTR); ++i){
        if(child == NullExp()){
            error_msg(ARGUMENTS_NUM2, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);  /* 说明调用时的参数小于定义时的参数 */
            break;
        }
        INFO info1 = expression(child->LeftC);
        if(!GetAttr(nSymInd, PREDE_ATTR)){
            if(!(info1.dim == 0 && info1.type == GetAttr(nSymInd + i + 1, TYPE_ATTR))){
                error_msg(ARGU_MIS, CONTINUE, GetAttr(nSymInd, NAME_ATTR), i + 1);      /* 第i个参数类型维度不匹配 */
            }
            if(GetAttr(nSymInd + i + 1, KIND_ATTR) == REF_ARG && child->LeftC->NodeOpType != VarOp){    /* 引用参数不能为值或表达式 */
            	if(child->LeftC->NodeKind == EXPRNode){
            		error_msg(EXPR_VAR, CONTINUE, GetAttr(nSymInd, NAME_ATTR), i + 1);
            	}
                else error_msg(CONSTANT_VAR, CONTINUE, GetAttr(nSymInd, NAME_ATTR), i + 1);
            }
        }
        child = child->RightC;
    }
    if(child != NullExp()){
        error_msg(ARGUMENTS_NUM2, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);  /* 说明调用时的参数大于定义时的参数 */
    }
    return info;
}

void returnstmt(tree node){
    tree type = GetAttr(globalmethodindex, TYPE_ATTR);
    int dim = GetAttr(globalmethodindex, DIMEN_ATTR);   /* 在methodop里用一个全局变量保存了索引，调用其直接获得该方法的属性*/
    if((node->LeftC == NullExp() && type != NullExp()) || (node->LeftC != NullExp() && type == NullExp())){
        error_msg(RETN_MIS, CONTINUE, GetAttr(globalmethodindex, NAME_ATTR), 0);  /* method的返回值和最终返回的值应均为void，但类型不匹配，报语义错误 */
    }
    else{               /* 如果method和最终的返回值不为void,即为一个表达式 */
        INFO infoofrightC = expression(node->LeftC);
        if(!(type == infoofrightC.type && dim == infoofrightC.dim)){
            error_msg(RETN_MIS, CONTINUE, GetAttr(globalmethodindex, NAME_ATTR), 0);   /* 比较类型和维度，有一个不符合就报语义错误 */
        }
    }
}

void ifstmt(tree node){
    if(node->LeftC != NullExp()){
        ifstmt(node->LeftC);            /* 嵌套，递归 */
    }
    if(node->RightC->NodeOpType == CommaOp){
        INFO info = expression(node->RightC->LeftC);
        if(!(info.type == INTEGERT() && info.dim == 0)){
            error_msg(OPER_MIS, CONTINUE, IfElseOp, 0);     /* 说明if语句内有错误 */
        }
        stmtlist(node->RightC->RightC);
    }
    else{
        stmtlist(node->RightC);
    }
}

void whilestmt(tree node){
    INFO info = expression(node->LeftC);
    if(!(info.type == INTEGERT() && info.dim == 0)){
        error_msg(OPER_MIS, CONTINUE, LoopOp, 0);         /* 说明while语句有错误 */
    }
    stmtlist(node->RightC);
}

INFO expression(tree node){
    if(node->NodeOpType == EQOp || node->NodeOpType == NEOp){
        simplexpression(node->LeftC);
        simplexpression(node->RightC);
        return storeinfo(INTEGERT(),0);
    }
    if(node->NodeOpType == LTOp || node->NodeOpType == LEOp || node->NodeOpType == GEOp || node->NodeOpType == GTOp){
        INFO infoofleftC = simplexpression(node->LeftC);
        INFO infoofrightC = simplexpression(node->RightC);
        if(!(infoofleftC.type == INTEGERT() && infoofleftC.dim == 0 && infoofrightC.type == INTEGERT() && infoofrightC.dim == 0)){
            error_msg(OPER_MIS, CONTINUE, node->NodeOpType, 0);          /* 说明是除整型外的其他类型之间进行大小比较，报语义错误 */
            return storeinfo(INTEGERT(),0);
        }
        return infoofleftC;
    }
    return simplexpression(node);
}

INFO simplexpression(tree node){
    if(node->NodeOpType == UnaryNegOp){
    	INFO info = simplexpression(node->LeftC);
    	if(!(info.type == INTEGERT() && info.dim == 0)){
    		error_msg(OPER_MIS, CONTINUE, UnaryNegOp, 0);
    	}
        return storeinfo(INTEGERT(),0);
    }
    if(node->NodeOpType == AddOp || node->NodeOpType == SubOp || node->NodeOpType == OrOp){
        INFO infoofleftC = simplexpression(node->LeftC);
        INFO infoofrightC = term(node->RightC);
        if(!(infoofleftC.type == INTEGERT() && infoofleftC.dim == 0 && infoofrightC.type == INTEGERT() && infoofrightC.dim == 0)){
            error_msg(OPER_MIS, CONTINUE, node->NodeOpType, 0);         /* 说明是除整型外的其他类型之间进行加减，报语义错误 */
        }
        return storeinfo(INTEGERT(),0);
    }
    return term(node);
}

INFO term(tree node){
    if(node->NodeOpType == MultOp || node->NodeOpType == DivOp || node->NodeOpType == AndOp){
        INFO infoofleftC = term(node->LeftC);
        INFO infoofrightC = factor(node->RightC);
        if(!(infoofleftC.type == INTEGERT() && infoofleftC.dim == 0 && infoofrightC.type == INTEGERT() && infoofrightC.dim == 0)){
            error_msg(OPER_MIS, CONTINUE, node->NodeOpType, 0);         /* 说明是除整型外的其他类型之间进行乘除，报语义错误 */
            return storeinfo(INTEGERT(),0);
        }
        return infoofleftC;
    }
    return factor(node);
}

INFO factor(tree node){
	switch(node->NodeKind){
        case NUMNode: return storeinfo(INTEGERT(),0);
        case STRINGNode: return storeinfo(STRINGT(),0);
    }
    switch(node->NodeOpType){
        case VarOp: return varop(node);
        case RoutineCallOp: return methodcallstmt(node, FUNC);
        case NotOp: {
        	INFO info = factor(node->LeftC);
        	if(!(info.type == INTEGERT() && info.dim == 0)){
        		error_msg(OPER_MIS, CONTINUE, NotOp, 0);   /* Not操作符的操作数不是整型，报语义错误 */
        	}
        	return storeinfo(INTEGERT(),0);
        }
    }
    return expression(node);
}

int typeidop(tree node){
    int dim = 0;
    int nSymInd;
    if(node->LeftC == INTEGERT() || (nSymInd = IDtoSTNode(&node->LeftC, VARIABLE, 0)) != 0){         /* 对应的IDNode为整型或者为ID */
        tree child = node->RightC;
        while(child != NullExp()){
            if(child->NodeOpType == FieldOp){
                error_msg(REC_TYPE_MIS, CONTINUE, GetAttr(nSymInd, NAME_ATTR), 0);
                break;
            }
            child = child->RightC;
            ++dim;    /* 递归遍历右子树，每遍历一次维度+1 */
        }
    }
    return dim;
}

INFO varop(tree node){
	globalvariableindex = IDtoSTNode(&node->LeftC, VARIABLE, 0);
    if(globalvariableindex == 0){
        return storeinfo(INTEGERT(),0);
    }
    INFO info;
    info.type = GetAttr(globalvariableindex, TYPE_ATTR);
    info.dim = GetAttr(globalvariableindex, DIMEN_ATTR);
    tree child = node->RightC;
    while(child != NullExp()){
    	if(child->LeftC->NodeOpType == FieldOp){      /* 检查FieldOp.是否有使用错误 */
        	if(info.dim != 0){          
	            error_msg(TYPE_MIS, CONTINUE, GetAttr(globalvariableindex, NAME_ATTR), 0);  /* 数组不能用.，报错 */
	            info.dim = -1;
	            return info;
        	}
        	else if(GetAttr(globalvariableindex, KIND_ATTR) == CLASS){      /* 一个类使用.时 */
	        	globalvariableindex = IDtoSTNode(&child->LeftC->LeftC, FIELD, globalvariableindex);
	            if(globalvariableindex == 0){
	            	info.dim = -1;
	                return info;
	            }
	            info.type = GetAttr(globalvariableindex, TYPE_ATTR);
	            info.dim = GetAttr(globalvariableindex, DIMEN_ATTR);
	        }
	        else if(GetAttr(globalvariableindex, KIND_ATTR) == VAR){      /* 一个变量使用.时 */
	        	int temp = globalvariableindex;
	        	tree typenode = ((tree)GetAttr(temp, TYPE_ATTR))->LeftC;
	        	int intval = ((tree)GetAttr(temp, TYPE_ATTR))->IntVal;
	        	globalvariableindex = IDtoSTNode(&child->LeftC->LeftC, FIELD, intval);
	            if(typenode== INTEGERT() || globalvariableindex == 0){
	                info.dim = -1;
	                return info;
	            }
	            info.type = GetAttr(globalvariableindex, TYPE_ATTR);
	            info.dim = GetAttr(globalvariableindex, DIMEN_ATTR);
	        }
	    }
        if(child->LeftC->NodeOpType == IndexOp){        /* 检查IndexOp[]是否有使用错误 */
            if(GetAttr(globalvariableindex, KIND_ATTR) != ARR){
                error_msg(TYPE_MIS, CONTINUE, GetAttr(globalvariableindex, NAME_ATTR), 0);  /* []只能用于数组，不是数组则报错 */
                info.dim = -1;
                return info;
            }
            if(info.dim <= 0){
                error_msg(INDX_MIS, CONTINUE, GetAttr(globalvariableindex, NAME_ATTR), 0);   /* 说明索引数大于数组维度，报错 */
                info.dim = -1;
                return info;
            }
            INFO index = expression(child->LeftC->LeftC);
            if(!(index.type == INTEGERT() && index.dim == 0)){
                error_msg(OPER_MIS, CONTINUE, 0, 0);         /* 索引位置不是整型，报错 */
            }
            info.dim--;
        }
        child = child->RightC;
    }
    return info;
}

/* 将IDNode转为STNode的函数：针对不同情况查找索引的方式不同(根据proj3.c)；然后把新的叶子节点添加进去即可 */
int IDtoSTNode(tree* node, int cond, int pointer){
    int nSymInd, nStrInd;
    nStrInd = (*node)->IntVal;
    switch(cond){
        case DECLARE: nSymInd = InsertEntry(nStrInd); break;
        case VARIABLE: nSymInd = LookUp(nStrInd); break;
        default:
            if((nSymInd = LookUpField(pointer, nStrInd)) == 0){
                error_msg(FIELD_MIS, CONTINUE, nStrInd, 0);
            }
    }
    if(nSymInd != 0){
        *node = MakeLeaf(STNode, nSymInd);
    }
    return nSymInd;
}

/* 递归统计参数个数 */
int countargnum(tree node){
    if(node == NullExp()){
        return 0;
    }
    int nSymInd = IDtoSTNode(&node->LeftC->LeftC, DECLARE, 0);     /* 对应的IDNode */
    if(nSymInd != 0){
    	SetAttr(nSymInd, TYPE_ATTR, node->LeftC->RightC);
    	if(node->NodeOpType == RArgTypeOp){
    		SetAttr(nSymInd, KIND_ATTR, REF_ARG);	/* 引用传递 */
    	}
    	else{
    		SetAttr(nSymInd, KIND_ATTR, VALUE_ARG);	/* 值传递 */
    	}
    }
    return countargnum(node->RightC) + 1;    /* 递归右子树即可 */
}

void do_semantic(tree parseTree){
    STInit();           	                 // Initialize the symbol table
    traverse(parseTree);                     // Traverse tree
    STPrint();          	                 // Print the symbol table
}
