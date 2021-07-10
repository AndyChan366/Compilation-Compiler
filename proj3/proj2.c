/* %A% ********************************************************/
/*
proj2.c

This file consists of 4 parts
	a. the data structure of a tree node
	b. the tree operation functions, from "CopyTree"
           to "SetRightChild"
	c. the tree printing function

The functions in this file are contributed by Chunmin Qiao and
Aggelos Varvitsiotis.
*/

/*	define syntax tree node and pointer type	*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj2.h"
#include "proj3.h"
ILTree dummy = { DUMMYNode, 0, 0, 0, 0 };
ILTree integer = { INTEGERTNode, 0, 0, 0, 0 };
ILTree string = { STRINGNode, 0, 0, 0, 0 };
/********************************************************
* 	This function return true if the node is 	*
*	DUMMYNode, false otherwise.			*
********************************************************/
int IsNull(T)
        tree T;
{
    return ( NodeKind(T) == DUMMYNode );
}
/* "int" & "string" */
tree INTEGERT()
{
    return &integer;
}
tree STRINGT()
{
    return &string;
}
/********************************************************
*	This function return a DUMMYNode to the caller  *
*	Note: All the dummy nodes are corresponding to  *
*	the save memory location.  So any attampt to    *
*	use it for the other purposes will cause        *
*	trouble                                         *
********************************************************/
tree NullExp()
{
    return (&dummy);
}

/***********************************************************
*	This function will create a leafnode with it	   *
*	NodeKind and IntVal to be Kind and N, respectively *
***********************************************************/
tree MakeLeaf(Kind, N)
        int Kind, N;
{
    tree 	p;

    p = (tree)malloc(sizeof(ILTree));
    p->NodeKind = Kind;
    p->IntVal = N;
    return (p);
}

/***********************************************************
* 	This function create a interior node of NodeOptype *
*	with children to be Left and Right, respectively,  *
***********************************************************/
tree MakeTree(NodeOp, Left, Right)
        int NodeOp;
        tree Left, Right;
{
    tree 	p;

    p = (tree)malloc(sizeof(ILTree));
    p->NodeKind = EXPRNode;
    p->NodeOpType = NodeOp;
    p->LeftC = Left;
    p->RightC = Right;
    return (p);
}

/*********************************************************
*	This function returns leftchild of the treenode  *
*********************************************************/
tree LeftChild(T)
        tree T;
{
    if (NodeKind(T) != EXPRNode)
        return (NullExp());
    return (T->LeftC);
}

/*********************************************************
*	This function returns rightchild of the treenode *
*********************************************************/
tree RightChild(T)
        tree T;
{
    if (NodeKind(T) != EXPRNode)
        return (NullExp());
    return (T->RightC);
}

/********************************************************
*	This function makes subtree T1 to be the        *
*	leftmost child of the tree T2, return T2	*
********************************************************/
tree MkLeftC(T1, T2)
        tree T1, T2;
{
    tree p,q;

    if (IsNull(T2)) return (T1);
    p = T2;
    q = LeftChild(p);
    /* replace the leftmost DUMMYNode */
    while ( !IsNull(q) )
    {
        p = q;
        q = LeftChild(p);
    }
    p->LeftC = T1;
    return(T2);
}

/********************************************************
*	This function makes subtree T1 to be the        *
*	rightmost child of the tree T2, return T2	*
********************************************************/
tree MkRightC(T1, T2)
        tree T1, T2;
{
    tree p,q;

    if (IsNull(T2)) return (T1);
    p = T2;
    q = RightChild(p);
    /* replace the rightmost DUMMYNode */
    while ( !IsNull(q) )
    {
        p = q;
        q = RightChild(p);
    }
    p->RightC = T1;
    return(T2);
}

/********************************************************
*	This function returns NodeOpType of a node	*
********************************************************/
int NodeOp(T)
        tree T;
{
    if (NodeKind(T) != EXPRNode)
    {
        printf("NodeOP(): This node must be an EXPRNode!\n");
        return 0;
    }
    return (T->NodeOpType);
}

/********************************************************
*	This function returns NodeKind of a node 	*
********************************************************/
int NodeKind(T)
        tree T;
{
    return (T->NodeKind);
}


/********************************************************
*	This function returns IntVal of a leafnode	*
********************************************************/
int IntVal(T)
        tree T;
{
    if ( NodeKind(T) == EXPRNode )
    {
        printf("IntVal(): This node must be a leaf node!\n");
        return(-1);
    }
    return (T->IntVal);
}
/* judge a node is DUMMYNode */

/********************************************************
*	This function sets the Target Node to be	*
*	Source Node (only for Non Dummy Target Node)	*
********************************************************/
void SetNode(Target, Source)
        tree Target, Source;
{
    if ((Target->NodeKind = Source->NodeKind) != EXPRNode)
    {
        Target->IntVal = Source->IntVal;
        Target->LeftC = NullExp();
        Target->RightC = NullExp();
    }
    else
    {
        Target->NodeOpType = Source->NodeOpType;
        Target->LeftC = Source->LeftC;
        Target->RightC = Source->RightC;
    }
}

/********************************************************
*	This function sets the NodeOpType  to be	*
*	to be NewOp (only for Interior EXPRNode)	*
********************************************************/
void SetNodeOp(T, Op)
        tree T;
        int Op;
{
    if (NodeKind(T) != EXPRNode)
        printf("SetNodeOp(): This node must be an EXPRNode!\n");
    else
        T->NodeOpType = Op;
}

/********************************************************
*	This function sets the tree root and all its	*
*	left subtree root to be a NewOp node, used only *
*	in construct a Record Component subtree.	*
*	Name Changed by Hui-Jung Chang, Oct.30, 1992	*
********************************************************/
void SetLeftTreeOp(T, Op)
        tree T;
        int Op;
{
    tree 	p;

    p = T;
    do
    {
        SetNodeOp(p, Op);
        p = LeftChild(p);
    }
    while ( !IsNull(p));
}


/********************************************************
*	This function sets the tree root and all its	*
*	right subtree root to be a NewOp node, used	*
*	only in construct a Procedure or function call	*
*	subtree with arguments				*
*	Added by Hui-Jung Chang	, Oct.30, 1992		*
********************************************************/
void SetRightTreeOp(T, Op)
        tree T;
        int Op;
{
    tree 	p;

    p = T;
    do
    {
        SetNodeOp(p, Op);
        p = RightChild(p);
    }
    while ( !IsNull(p));
}


/****************************************************************
* 	This function sets the LeftChild of T to be NewC	*
****************************************************************/
void SetLeftChild(T, NewC)
        tree T, NewC;
{
    if (NodeKind(T) != EXPRNode)
        printf("SetLeftChild(): This node must be an EXPRNode!\n");
    else
        T->LeftC = NewC;
}

/****************************************************************
* 	This function sets the RightChild of T to be NewC	*
****************************************************************/
void SetRightChild(T, NewC)
        tree T, NewC;
{
    if (NodeKind(T) != EXPRNode)
        printf("SetRightChild(): This node must be an EXPRNode!\n");
    else
        T->RightC = NewC;
}

/*****************************************************************/
/*	This is syntax tree printer, "treelst" is the output file
	pointer.

   	call printtree with the root node pointer and the depth level
	(could be 0 if you do not want the root to be indent)

   	writing "getname()" and "getstring()" is your responsibility
	^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 */


extern FILE *treelst;

char *opnodenames [] =
        {
                "ProgramOp", "BodyOp", "DeclOp", "CommaOp", "ArrayTypeOp", "TypeIdOp",
                "BoundOp", "RecompOp",
                "ToOp", "DownToOp", "ConstantIdOp", "ProceOp", "FuncOp",
                "HeadOp", "RArgTypeOp", "VArgTypeOp", "StmtOp", "IfElseOp",
                "LoopOp", "SpecOp", "RoutineCallOp", "AssignOp", "ReturnOp",
                "AddOp", "SubOp", "MultOp", "DivOp",
                "LTOp", "GTOp", "EQOp", "NEOp", "LEOp", "GEOp", "AndOp", "OrOp",
                "UnaryNegOp", "NotOp", "VarOp", "SelectOp", "IndexOp", "FieldOp",
                "SubrangeOp", "ExitOp", "ClassOp", "MethodOp", "ClassDefOp"
        };

static int crosses [162];

void indent (x)
        int x;
{
    register int i;

    for (i = 0; i < x; i++)
    {
        fprintf (treelst,"%s", crosses [i]? "| " : "  ");
    }
    fprintf (treelst,"%s", x ? "+-" : "R-");
    if (x)
        crosses [x] = (crosses [x] + 1) % 2;
}

void zerocrosses ()
{
    register int i;
    for (i = 0; i < 162; i++)
        crosses [i] = 0;
}

extern char* String_table;

char* getname(int i)/*return ID name or String, i is the index of the string table, passed through yylval*/
{
    return( String_table+i );/*return string table indexed at i*/
}

char* getstring(int i)
{
    return( String_table+i );/*return string table indexed at i*/
}


void printtree (nd, depth)
        tree nd;
        int depth;
{
    int id, indx;

    if (!depth)
    {
        zerocrosses ();
        fprintf (treelst,"************* SYNTAX TREE PRINTOUT ***********\n\n");
    }
    if (IsNull (nd))
    {
        indent (depth);
        fprintf (treelst,"[DUMMYnode]\n");
        return;
    }
    if (NodeKind (nd) == EXPRNode)
        printtree (RightChild (nd), depth + 1);
    indent (depth);
    switch (NodeKind (nd))
    {
        case IDNode:
            indx = IntVal(nd);
            if (indx >= 0)
            {
                id = indx; /*GetAttr(indx, NAME_ATTR); */
                fprintf (treelst,"[IDNode,%d,\"%s\"]\n", IntVal(nd),
                         getname(id));
            }
            else
                fprintf (treelst,"[IDNode,%d,\"%s\"]\n", indx, "err");
            break;

        case STNode:
            indx = IntVal(nd);
            if (indx > 0)
            {
                id=GetAttr(indx, NAME_ATTR);
                fprintf (treelst,"[STNode,%d,\"%s\"]\n", IntVal(nd),
                         getname(id));
            }
            else
                fprintf (treelst,"[IDNode,%d,\"%s\"]\n", indx, "err");
            break;

        case INTEGERTNode:
            fprintf (treelst,"[INTEGERTNode]\n");
            break;

        case NUMNode:   fprintf (treelst,"[NUMNode,%d]\n", IntVal (nd));
            break;

        case CHARNode:  if (isprint (IntVal (nd)))
                fprintf (treelst,"[CHARNode,%d,\'%c\']\n",
                         IntVal (nd), IntVal (nd));
            else
                fprintf (treelst,"[CHARNode,%d,\'\\%o\']\n",
                         IntVal (nd), IntVal (nd));
            break;

        case STRINGNode:fprintf (treelst,"[STRINGNode,%d,\"%s\"]\n", IntVal (nd),
                                 getstring(IntVal(nd)));
            break;

        case EXPRNode:  fprintf (treelst,"[%s]\n",
                                 opnodenames [NodeOp(nd) - ProgramOp]);
            break;

        default:	    fprintf (treelst,"INVALID!!!\n");
            break;
    }
    if (NodeKind (nd) == EXPRNode)
        printtree (LeftChild (nd), depth + 1);
}

int loc_str(char* str) {
    int i=0;
    while(i<sizeofstring){
        if(strcmp((char*)&String_table[i],str)==0)
            return i;               /* 比较，完全相同则返回所在位置*/
        else
            i+=strlen((char*)&String_table[i])+1; /* 如果当前对应的token不是所要找的token，则向前移动当前token长度（即指到下一个token） */
    }
    if(i>=sizeofstring)return -1;
}

/* 将参数放进去以便后续构建parse tree,就是维护一个container,不够大时就分配内存，然后将参数放进来 */
void store(int RorVArgop){
    if(length==0){
        right=0;
        length=1;
        container=(int*)malloc(sizeof(int));
    }
    while(length<right+1){
        length++;
    }
    container=(int*)realloc(container,length*sizeof(int));
    if(container==NULL){
        return;
    }/* 出现问题终止 */
    container[right++]=RorVArgop;/* 将参数存进来 */
}
