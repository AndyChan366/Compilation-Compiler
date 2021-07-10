%{ /* definition */
#include "proj2.h"
#include <stdio.h>
%}

%token <intg> ANDnum ASSGNnum DECLARATIONSnum DOTnum ENDDECLARATIONSnum EQUALnum GTnum IDnum INTnum LBRACnum LPARENnum METHODnum NEnum ORnum PROGRAMnum RBRACnum RPARENnum SEMInum VALnum WHILEnum CLASSnum COMMAnum DIVIDEnum ELSEnum EQnum GEnum ICONSTnum IFnum LBRACEnum LEnum LTnum MINUSnum NOTnum PLUSnum RBRACEnum RETURNnum SCONSTnum TIMESnum VOIDnum

%type <tptr> Program ProgramLST ClassDecl ClassBody ClassBodyLST Decls DeclsLST FieldDecl FieldType FieldDeclLST VariableDeclId VariableInitializer ArrayInitializer ArrayInitializerLST ArrayCreationExpression ArrayCreationExpressionLST MethodDecl RorVArg FormalParameterList FormalParameterListRST Block Type TypeRST StatementList StatementListLST Statement AssignmentStatement MethodCallStatement MethodCallStatementRST ReturnStatement IfStatement IfStatementLST WhileStatement Expression SimpleExpression Term Factor UnsignedConstant Variable VariableRST ExpressionIndex

/* 其中LST代表left subtree,RST代表right subtree.（根据附录所给图和树可知有一定的递归关系所以这样命名） */

%% /* yacc specification */
Program : PROGRAMnum IDnum SEMInum ProgramLST
        { $$ = MakeTree(ProgramOp,$4,MakeLeaf(IDNode,$2)); printtree($$,0); }
        ;
ProgramLST : ClassDecl
        { $$ = MakeTree(ClassOp,NullExp(),$1); }
        | ProgramLST ClassDecl
        { $$ = MakeTree(ClassOp,$1,$2); }
        ;
ClassDecl : CLASSnum IDnum ClassBody
        { $$ = MakeTree(ClassDefOp,$3,MakeLeaf(IDNode,$2)); }
        ;
ClassBody : LBRACEnum Decls RBRACEnum
        { $$ = MakeTree(BodyOp,$2,NullExp()); }
        | LBRACEnum ClassBodyLST RBRACEnum
        { $$ = $2; }
        ;
ClassBodyLST : 
        { $$ = NullExp(); }
        | Decls MethodDecl
        { $$ = MakeTree(BodyOp,$1,$2); }
        | ClassBodyLST MethodDecl
        { $$ = MakeTree(BodyOp,$1,$2); }
        ;
Decls : DECLARATIONSnum DeclsLST ENDDECLARATIONSnum
        { $$ = $2; }
        ;
DeclsLST : 
        { $$ = NullExp(); }
        | DeclsLST FieldDecl
        { $$ = MakeTree(BodyOp,$1,$2); }
        ;
FieldDecl : FieldType FieldDeclLST SEMInum
        { $$ = $2; }
        ;
FieldType : Type
        { globalvariable = $1; }
        ;
FieldDeclLST : VariableDeclId
        { $$ = MakeTree(DeclOp,NullExp(),MakeTree(CommaOp,$1,MakeTree(CommaOp,globalvariable,NullExp()))); }
        | FieldDeclLST COMMAnum VariableDeclId
        { $$ = MakeTree(DeclOp,$1,MakeTree(CommaOp,$3,MakeTree(CommaOp,globalvariable,NullExp()))); }
        | VariableDeclId EQUALnum VariableInitializer
        { $$ = MakeTree(DeclOp,NullExp(),MakeTree(CommaOp,$1,MakeTree(CommaOp,globalvariable,$3))); }
        | FieldDeclLST COMMAnum VariableDeclId EQUALnum VariableInitializer
        { $$ = MakeTree(DeclOp,$1,MakeTree(CommaOp,$3,MakeTree(CommaOp,globalvariable,$5))); }
        ;
VariableDeclId : IDnum
        { $$ = MakeLeaf(IDNode,$1); }
        | VariableDeclId LBRACnum RBRACnum
        { $$ = $1; }
        ;
VariableInitializer : Expression
        { $$ = $1; }
        | ArrayInitializer
        { $$ = $1; }
        | ArrayCreationExpression
        { $$ = $1; }
        ;
ArrayInitializer : LBRACEnum ArrayInitializerLST RBRACEnum
        { $$ = MakeTree(ArrayTypeOp,$2,globalvariable); }
        ;
ArrayInitializerLST : VariableInitializer
        { $$ = MakeTree(CommaOp,NullExp(),$1); }
        | ArrayInitializerLST COMMAnum VariableInitializer
        { $$ = MakeTree(CommaOp,$1,$3); }
        ;
ArrayCreationExpression : INTnum ArrayCreationExpressionLST
        { $$ = MakeTree(ArrayTypeOp,$2,INTEGERT()); }
        ;
ArrayCreationExpressionLST : LBRACnum Expression RBRACnum
        { $$ = MakeTree(BoundOp,NullExp(),$2); }
        | ArrayCreationExpressionLST LBRACnum Expression RBRACnum
        { $$ = MakeTree(BoundOp,$1,$3); }
        ;
MethodDecl : 
        METHODnum Type IDnum LPARENnum FormalParameterList RPARENnum Block
        { $$ = MakeTree(MethodOp,MakeTree(HeadOp,MakeLeaf(IDNode,$3),MakeTree(SpecOp,$5,$2)),$7); }
        | METHODnum VOIDnum IDnum LPARENnum FormalParameterList RPARENnum Block
        { $$ = MakeTree(MethodOp,MakeTree(HeadOp,MakeLeaf(IDNode,$3),MakeTree(SpecOp,$5,NullExp())),$7); }
        ;
/* 为了能够正确parse,需将参数列表里的参数存到一个container内（见proj2.c）,VArgTypeOp代表"val int y"形;RArgTypeOp代表"int x"形 */
RorVArg : 
        { store(RArgTypeOp); }
        | VALnum
        { store(VArgTypeOp); }
        ;
FormalParameterList : 
	{ $$ = NullExp(); }
	| RorVArg INTnum IDnum FormalParameterListRST
        { $$ = MakeTree(container[--right],MakeTree(CommaOp,MakeLeaf(IDNode,$3),INTEGERT()),$4); }
        ;
/* 用递归形式来定义参数列表的grammar,在构建parse tree时需要从后往前得到container内存储的参数（因为是reverse of rightmost）,因此从后往前递归提取出来之前存的参数 */
FormalParameterListRST :
        { $$ = NullExp(); }
	| SEMInum RorVArg INTnum IDnum FormalParameterListRST
        { $$ = MakeTree(container[--right],MakeTree(CommaOp,MakeLeaf(IDNode,$4),INTEGERT()),$5); }
        | COMMAnum IDnum FormalParameterListRST
        { $$ = MakeTree(container[--right],MakeTree(CommaOp,MakeLeaf(IDNode,$2),INTEGERT()),$3); }
        ;
Block : StatementList
        { $$ = MakeTree(BodyOp,NullExp(),$1); }
        | Decls StatementList
        { $$ = MakeTree(BodyOp,$1,$2); }
        ;
Type : IDnum TypeRST
        { $$ = MakeTree(TypeIdOp,MakeLeaf(IDNode,$1),$2); }
        | INTnum TypeRST
        { $$ = MakeTree(TypeIdOp,INTEGERT(),$2); }
        ;
TypeRST : 
        { $$ = NullExp(); }
        | LBRACnum RBRACnum TypeRST 
        { $$ = MakeTree(IndexOp,NullExp(),$3); }
        | LBRACnum RBRACnum DOTnum Type
        { $$ = MakeTree(FieldOp,$4,NullExp()); }
        ;
StatementList : LBRACEnum StatementListLST RBRACEnum
        {
            if(IsNull($2)){
                $$ = MakeTree(StmtOp,$2,$2);
            }
            else{
                $$ = $2; 
            }
        }
	/*LBRACEnum StatementListLST RBRACEnum
	{ $$ = $2; }
	| LBRACEnum RBRACEnum
	{ $$ = MakeTree(StmtOp,NullExp(),NullExp()); }*/
        ;
StatementListLST : Statement
        {
             if(IsNull($1)){
                 $$ = $1;
             }
             else{
                 $$ = MakeTree(StmtOp,NullExp(),$1);
             }
        }
        | StatementListLST SEMInum Statement
        {
             if(IsNull($3)){
                 $$ = $1;
             }
             else{
                 $$ = MakeTree(StmtOp,$1,$3);
             }
        }
	/*Statement
        {
             if(IsNull($1)){
                 $$ = $1;
             }
             else{
                 $$ = MakeTree(StmtOp,NullExp(),$1);
             }
        }
	| StatementListLST SEMInum
	{ $$ = $1; }
	| StatementListLST SEMInum Statement
	{ $$ = MakeTree(StmtOp,$1,$3); }*/
        ;
Statement : 
        { $$ = NullExp(); }
        | AssignmentStatement
        { $$ = $1; }
        | MethodCallStatement
        { $$ = $1; }
        | ReturnStatement
        { $$ = $1; }
        | IfStatement
        { $$ = $1; }
        | WhileStatement
        { $$ = $1; } 
        ;
AssignmentStatement : Variable ASSGNnum Expression
        { $$ = MakeTree(AssignOp,MakeTree(AssignOp,NullExp(),$1),$3); }
        ;
MethodCallStatement : Variable LPARENnum RPARENnum
        { $$ = MakeTree(RoutineCallOp,$1,NullExp()); }
        | Variable LPARENnum MethodCallStatementRST RPARENnum
        { $$ = MakeTree(RoutineCallOp,$1,$3); }
        ;
MethodCallStatementRST : Expression
        { $$ = MakeTree(CommaOp,$1,NullExp()); }
        | Expression COMMAnum MethodCallStatementRST
        { $$ = MakeTree(CommaOp,$1,$3); }
        ;
ReturnStatement : RETURNnum
        { $$ = MakeTree(ReturnOp,NullExp(),NullExp()); }
        | RETURNnum Expression
        { $$ = MakeTree(ReturnOp,$2,NullExp()); }
        ;
IfStatement : IfStatementLST
        { $$ = $1; }
        | IfStatementLST ELSEnum StatementList
        { $$ = MakeTree(IfElseOp,$1,$3); }
        ;
IfStatementLST : IFnum Expression StatementList
        { $$ = MakeTree(IfElseOp,NullExp(),MakeTree(CommaOp,$2,$3)); }
        | IfStatementLST ELSEnum IFnum Expression StatementList
        { $$ = MakeTree(IfElseOp,$1,MakeTree(CommaOp,$4,$5)); }
        ;
WhileStatement : WHILEnum Expression StatementList
        { $$ = MakeTree(LoopOp,$2,$3); }
        ;
Expression : SimpleExpression
        | SimpleExpression LTnum SimpleExpression
        { $$ = MakeTree(LTOp,$1,$3); }
        | SimpleExpression LEnum SimpleExpression
        { $$ = MakeTree(LEOp,$1,$3); }
        | SimpleExpression EQnum SimpleExpression
        { $$ = MakeTree(EQOp,$1,$3); }
        | SimpleExpression NEnum SimpleExpression
        { $$ = MakeTree(NEOp,$1,$3); }
        | SimpleExpression GEnum SimpleExpression
        { $$ = MakeTree(GEOp,$1,$3); }
        | SimpleExpression GTnum SimpleExpression
        { $$ = MakeTree(GTOp,$1,$3); }
        ;
SimpleExpression : Term
        { $$ = $1; }
        | PLUSnum Term
        { $$ = $2; }
        | MINUSnum Term
        { $$ = MakeTree(UnaryNegOp,$2,NullExp()); }
        | SimpleExpression PLUSnum Term
        { $$ = MakeTree(AddOp,$1,$3); }
        | SimpleExpression MINUSnum Term
        { $$ = MakeTree(SubOp,$1,$3); }
        | SimpleExpression ORnum Term
        { $$ = MakeTree(OrOp,$1,$3); }
        ;
Term : Factor
        { $$ = $1; }
        | Term TIMESnum Factor
        { $$ = MakeTree(MultOp,$1,$3); }
        | Term DIVIDEnum Factor
        { $$ = MakeTree(DivOp,$1,$3); }
        | Term ANDnum Factor
        { $$ = MakeTree(AndOp,$1,$3); }
        ;
Factor : UnsignedConstant
        { $$ = $1; }
        | Variable
        { $$ = $1; }
        | MethodCallStatement
        { $$ = $1; }
        | LPARENnum Expression RPARENnum
        { $$ = $2; }
        | NOTnum Factor
        { $$ = MakeTree(NotOp,$2,NullExp()); }
        ;
UnsignedConstant : ICONSTnum
        { $$ = MakeLeaf(NUMNode,$1); }
        | SCONSTnum
        { $$ = MakeLeaf(STRINGNode,$1); }
        ;
Variable : IDnum VariableRST
        { $$ = MakeTree(VarOp,MakeLeaf(IDNode,$1),$2); }
        ;
VariableRST : 
        { $$ = NullExp(); }
        | LBRACnum ExpressionIndex RBRACnum VariableRST
        { $$ = MakeTree(SelectOp,$2,$4); }
        | DOTnum IDnum VariableRST
        { $$ = MakeTree(SelectOp,MakeTree(FieldOp,MakeLeaf(IDNode,$2),NullExp()),$3); }
        ;
ExpressionIndex : Expression
        { $$ = MakeTree(IndexOp,$1,NullExp()); }
        | Expression COMMAnum ExpressionIndex
        { $$ = MakeTree(IndexOp,$1,$3); }
        ;


%%
int yycolumn, yyline;
FILE *treelst;

main() {
   treelst = stdout;
   yyparse();
   /*for(int i=0;i<length;i++){
        printf("%d\n",container[i]);
   }*/
}

yyerror(char *str) { 
    printf("yyerror: %s at line %d!\n",str,yyline); 
}
