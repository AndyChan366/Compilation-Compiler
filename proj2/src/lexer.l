%{
#include "proj2.h"
#include "y.tab.h"
/* TODO: declare variables here */
int yyline=1,yycolumn=1;
/* String table && index table */
char* String_table;
int* indexofid;
/* numofstring是检测到string的长度，sizeofstring是分配到String table的内存数 */
int numofstring=0;
int sizeofstring=0;
/* 同上，只不过是index table表的。 */
int numofindex=0;
int sizeofindex=0;
/* 对两个表的动态处理函数：第一个是String table，第二个是index table */
int updatetable(int l);
int updateindex(int l,char* str);
%}

/* TODO: define regular expressions */
AND   			(&&)
ASSGN   		(:=)
DECLARATIONS   		(declarations)
DOT   			(\.)
ENDDECLARATIONS   	(enddeclarations)
EQUAL   		(=)
GT   			(>)
INT   			(int)
LBRAC   		(\[)
LPAREN   		(\()
METHOD   		(method)
NE   			(!=)
OR   			(\|\|)
PROGRAM   		(program)
RBRAC   		(\])
RPAREN   		(\))
SEMI   			(;)
VAL   			(val)
WHILE   		(while)
CLASS   		(class)
COMMA   		(,)
DIVIDE   		(\/)
ELSE   			(else)
EQ   			(==)
GE   			(>=)
IF   			(if)
LBRACE   		(\{)
LE   			(<=)
LT   			(<)
MINUS   		(-)
NOT   			(!)
PLUS   			(\+)
RBRACE   		(\})
RETURN   		(return)
SCONST   		('(\\n|\\t|\\'|\\\\|[^\n\t'\\])*')
TIMES   		(\*)
VOID   			(void)
COMMENT                 (\/\*([^\*]|\*[^\/])*\*\/)
SPACES          	([ \n\t])
WRONGCOMMENT            (\/\*([^\*]|\*[^\/])*)
WRONGSTRING             ('[^\n']*)
ICONST                  ([0-9]+)
ID                      ([a-zA-Z0-9]+)
UNRECOGINZED            (.)


/* TODO: define translation rules: pattern and action */
%%

{AND} {
    yycolumn+=2;
    return (ANDnum);
}

{ASSGN} {
    yycolumn+=2;
    return (ASSGNnum);
}

{DECLARATIONS} {
    yycolumn+=12;
    return (DECLARATIONSnum);
}

{DOT} {
    yycolumn+=1;
    return (DOTnum);
}

{ENDDECLARATIONS} {
    yycolumn+=15;
    return (ENDDECLARATIONSnum);
}

{EQUAL} {
    yycolumn+=1;
    return (EQUALnum);
}

{GT} {
    yycolumn+=1;
    return (GTnum);
}

{INT} {
    yycolumn+=3;
    return (INTnum);
}

{LBRAC} {
    yycolumn+=1;
    return (LBRACnum);
}

{LPAREN} {
    yycolumn+=1;
    return (LPARENnum);
}

{METHOD} {
    yycolumn+=6;
    return (METHODnum);
}

{NE} {
    yycolumn+=2;
    return (NEnum);
}

{OR} {
    yycolumn+=2;
    return (ORnum);
}

{PROGRAM} {
    yycolumn+=7;
    return (PROGRAMnum);
}

{RBRAC} {
    yycolumn+=1;
    return (RBRACnum);
}

{RPAREN} {
    yycolumn+=1;
    return (RPARENnum);
}

{SEMI} {
    yycolumn+=1;
    return (SEMInum);
}

{VAL} {
    yycolumn+=3;
    return (VALnum);
}

{WHILE} {
    yycolumn+=5;
    return (WHILEnum);
}

{CLASS} {
    yycolumn+=5;
    return (CLASSnum);
}

{COMMA} {
    yycolumn+=1;
    return (COMMAnum);
}

{DIVIDE} {
    yycolumn+=1;
    return (DIVIDEnum);
}

{ELSE} {
    yycolumn+=4;
    return (ELSEnum);
}

{EQ} {
    yycolumn+=2;
    return (EQnum);
}

{GE} {
    yycolumn+=2;
    return (GEnum);
}

{IF} {
    yycolumn+=2;
    return (IFnum);
}

{LBRACE} {
    yycolumn+=1;
    return (LBRACEnum);
}

{LE} {
    yycolumn+=2;
    return (LEnum);
}

{LT} {
    yycolumn+=1;
    return (LTnum);
}

{MINUS} {
    yycolumn+=1;
    return (MINUSnum);
}

{NOT} {
    yycolumn+=1;
    return (NOTnum);
}

{PLUS} {
    yycolumn+=1;
    return (PLUSnum);
}

{RBRACE} {
    yycolumn+=1;
    return (RBRACEnum);
}

{RETURN} {
    yycolumn+=6;
    return (RETURNnum);
}

{SCONST} {
    /* 处理string constant的函数 */
    yycolumn+=yyleng;
    int j=0;
    /* 抠掉双引号，处理中间n-2个字符 */
    char* str=(char*)malloc((yyleng-2)*sizeof(char));
    if(str==NULL){ 
	//return ERROR;
	printf("ERROR!");
    }
    for(int i=1;i<yyleng-1;i++){
        if(yytext[i]=='\\'){
	    char c=yytext[++i];
	    if(c=='n')str[j++]='\n';
	    else if(c=='t')str[j++]='\t';
	    else if(c=='\'')str[j++]='\'';
            else str[j++]='\\';
/* 分情况讨论，如果\后面接的是一些特殊字符则代表转义，不是的话则就是本身 */
        }
/* 其他字符则正常，直接存进去即可 */
        else{
            str[j++]=yytext[i];
        }
    }
    if(!updatetable(j)){
        //return ERROR;
	printf("ERROR!");
    }/* 内存出现问题则return -1*/
    int strindex=updateindex(j,str);/* 更新index表 */
    if(strindex==-1){
        //return ERROR;
	printf("ERROR!");
    }/* 同上 */
    if(strindex>=numofindex){  /* 如果返回来的索引位置大于当前索引个数（即没有出现重复）*/
        indexofid[numofindex++]=numofstring; /* 索引加1，并将该字符串长度存入该位置 */
        for(int i=0;i<j;i++){
            String_table[numofstring++]=str[i];
        }
        String_table[numofstring++]='\0';
    }/* 从该位置将该字符串常量存入String table */
    yylval.intg=indexofid[strindex];/* yylval返回当前索引位置 */
    return (SCONSTnum);
}

{TIMES} {
    yycolumn+=1;
    return (TIMESnum);
}

{VOID} {
    yycolumn+=4;
    return (VOIDnum);
}

<<EOF>> {
    return 0;
}

{COMMENT} {
    for(int i=0; i<yyleng;i++){
        if(yytext[i]=='\n'){
	    yyline++;
            yycolumn=1;
        }
        else{
            yycolumn++;
        }
    }
    //return (COMMENTnum);
}

{SPACES} {
    if(yytext[0]=='\n'){
	yyline++;
        yycolumn=1;
    }
    else if(yytext[0]=='\t'){
        yycolumn+=4;
    }
    else{
        yycolumn++;
    }    
    //return (SPACESnum);
}

{WRONGCOMMENT} {
    for(int i=0;i<yyleng;i++){
	if(yytext[i]=='\n'){
		yyline++;
		yycolumn=1;
	}
	else if(yytext[i]=='\t'){
		yycolumn+=4;
	}
    else yycolumn++;
    }
    //return (WRONGCOMMENTnum);
}

{WRONGSTRING} {
    for(int i=0;i<yyleng;i++){
	if(yytext[i]=='\n'){
		yyline++;
		yycolumn=1;
	}
	else if(yytext[i]=='\t'){
		yycolumn+=4;
	}
    else yycolumn++;
    }
    //return (WRONGSTRINGnum);
    printf("Invalid string constant at line %d, column %d!\n",yyline,yycolumn);
    return (SCONSTnum);
}

{ICONST} {
    yylval.intg=atoi(yytext);
    return (ICONSTnum);
}

{ID} {
    yycolumn+=yyleng;
    if (!updatetable(yyleng)){
        //return ERROR;
	printf("ERROR!");
    }/* String table没有内存则return -1*/
    int judge=0;
    if(yytext[0]>='0' && yytext[0]<='9')judge=1;
    /* 先看第一位是否为数字 */
    for(int i=0;i<yyleng;i++){
        if (judge && !(yytext[i]>='0' && yytext[i]<='9')){
            judge=2;/* 第一位是但后面接的不是数字，则不是有效的identifier */
        }
    }
    if(judge==2){
	//return (WRONGIDnum);
        printf("Invalid identifier at line %d, column %d!\n",yyline,yycolumn);
	return (IDnum);
    }
    int idindex=updateindex(yyleng,yytext);/* 更新index表 */
    if(idindex==-1){
        //return ERROR;
	printf("ERROR!");
    }/* 同上 */
    if(idindex>=numofindex){/* 同SCONST的处理，可参考上面注释 */
        indexofid[numofindex++]=numofstring;
        for(int i=0;i<yyleng;i++){
            String_table[numofstring++]=yytext[i];
        }/* 将这个identifier存入到String table内 */
        String_table[numofstring++]='\0';
    }
    yylval.intg=indexofid[idindex];/* yylval返回当前索引位置 */
    return (IDnum);
}

{UNRECOGINZED} {
    yycolumn++;
    //return (UNRECOGINZEDnum);
}

%%

/* TODO: define the auxiliary functions you may use */
int yywrap()
{
    return 1;
}

/* String table的更新，输入参数为检测到的字符串长度，根据该长度开辟足够大的空间，然后再将字符串存进去*/
int updatetable(int l)
{
    if(sizeofstring==0){
        sizeofstring=16;
        String_table=(char*)malloc(16*sizeof(char));/* 先开出来一部分，初始值为16 */
    }
    while(sizeofstring<=numofstring+l){
            sizeofstring++;
    }   /* 不够再补 */
    String_table=(char*)realloc(String_table,sizeofstring*sizeof(char));
    if(String_table==NULL){
        return 0;/* 表为空的情况（有问题的情况） */
    }
    return 1;/* 一切正常 */
}
/* index table的更新，输入为字符和其长度，根据这些来开空间，最终返回的是该次处理完毕之后的索引值 */
int updateindex(int l, char* str)
{
    int i,j,flag;
    for(i=0;i<numofindex;i++){
	if(i+1<numofindex)flag=indexofid[i+1];     /* 遍历，处在中间时则长度为index表的该项对应的长度 */
	else flag=numofstring;       /* 遍历至最后一个了，则该处字符长度显然为numofstring */
        if(flag==indexofid[i]+l+1){  /* 如果长度一样，有可能是重复，接下来看是否字符串内容匹配 */
            for(j=0;j<l;j++){
                if(String_table[indexofid[i]+j]!=str[j])break;
            }          /* 说明该索引处字符不匹配，直接终止 */
            if(j==l)break;/* 说明完全匹配 */
        }
    }
    if(i==numofindex){      /* 同String table一样，可见上面注释 */
        if(sizeofindex==0){
            sizeofindex=16;
            indexofid=(int*)malloc(16*sizeof(int));
        }
        while(sizeofindex<=numofindex){
            sizeofindex++;
        }
        indexofid=(int*)realloc(indexofid,sizeofindex*sizeof(int));
        if(indexofid==NULL){
            return -1;/* 表为空的情况 */
        }
 }
    return i;/* 无问题则返回处理后的索引值 */
}
