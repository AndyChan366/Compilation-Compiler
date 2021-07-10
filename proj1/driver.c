#include "token.h"
#include <stdio.h>
#include <stdlib.h>

/* TODO: handle the tokens, and output the result */

// yylex() is in lex.yy.c
extern int yylex();
extern int yyleng, yyline, yycolumn, yylval;
extern char* String_table;
extern int* indexofid;
extern int numofstring,sizeofstring,numofindex,sizeofindex;

int main (int argc, char* argv[]) {
    int lexRtn = -1;			// return value of yylex
    printf("Line\t\tColumn\t\tToken\t\tIndex in String table\n");
    /* recognize each token type */
    while (1) {
        lexRtn = yylex();
        switch (lexRtn) {
            case ANDnum:{
                printf("%d\t\t%d\t\tANDnum\n", yyline, yycolumn);
                break;
            }
            case ASSGNnum:{
                printf("%d\t\t%d\t\tASSGNnum\n", yyline, yycolumn);
                break;
            }
            case DECLARATIONSnum:{
                printf("%d\t\t%d\t\tDECLARATIONSnum\n", yyline, yycolumn);
                break;
            }
            case DOTnum:{
                printf("%d\t\t%d\t\tDOTnum\n", yyline, yycolumn);
                break;
            }
            case ENDDECLARATIONSnum:{
                printf("%d\t\t%d\t\tENDDECLARATIONSnum\n", yyline, yycolumn);
                break;
            }
            case EQUALnum:{
                printf("%d\t\t%d\t\tEQUALnum\n", yyline, yycolumn);
                break;
            }
            case GTnum:{
                printf("%d\t\t%d\t\tGTnum\n", yyline, yycolumn);
                break;
            }
            case IDnum:{
                printf("%d\t\t%d\t\tIDnum\t\t%d\n", yyline, yycolumn, yylval);
                break;
            }
            case INTnum:{
                printf("%d\t\t%d\t\tINTnum\n", yyline, yycolumn);
                break;
            }
            case LBRACnum:{
                printf("%d\t\t%d\t\tLBRACnum\n", yyline, yycolumn);
                break;
            }
            case LPARENnum:{
                printf("%d\t\t%d\t\tLPARENnum\n", yyline, yycolumn);
                break;
            }
            case METHODnum:{
                printf("%d\t\t%d\t\tMETHODnum\n", yyline, yycolumn);
                break;
            }
            case NEnum:{
                printf("%d\t\t%d\t\tNEnum\n", yyline, yycolumn);
                break;
            }
            case ORnum:{
                printf("%d\t\t%d\t\tORnum\n", yyline, yycolumn);
                break;
            }
            case PROGRAMnum:{
                printf("%d\t\t%d\t\tPROGRAMnum\n", yyline, yycolumn);
                break;
            }
            case RBRACnum:{
                printf("%d\t\t%d\t\tRBRACnum\n", yyline, yycolumn);
                break;
            }
            case RPARENnum:{
                printf("%d\t\t%d\t\tRPARENnum\n", yyline, yycolumn);
                break;
            }
            case SEMInum:{
                printf("%d\t\t%d\t\tSEMInum\n", yyline, yycolumn);
                break;
            }
            case VALnum:{
                printf("%d\t\t%d\t\tVALnum\n", yyline, yycolumn);
                break;
            }
            case WHILEnum:{
                printf("%d\t\t%d\t\tWHILEnum\n", yyline, yycolumn);
                break;
            }
            case CLASSnum:{
                printf("%d\t\t%d\t\tCLASSnum\n", yyline, yycolumn);
                break;
            }
            case COMMAnum:{
                printf("%d\t\t%d\t\tCOMMAnum\n", yyline, yycolumn);
                break;
            }
            case DIVIDEnum:{
                printf("%d\t\t%d\t\tDIVIDEnum\n", yyline, yycolumn);
                break;
            }
            case ELSEnum:{
                printf("%d\t\t%d\t\tELSEnum\n", yyline, yycolumn);
                break;
            }
            case EQnum:{
                printf("%d\t\t%d\t\tEQnum\n", yyline, yycolumn);
                break;
            }
            case GEnum:{
                printf("%d\t\t%d\t\tGEnum\n", yyline, yycolumn);
                break;
            }
            case ICONSTnum:{
                printf("%d\t\t%d\t\tICONSTnum\n", yyline, yycolumn);
                break;
            }
            case IFnum:{
                printf("%d\t\t%d\t\tIFnum\n", yyline, yycolumn);
                break;
            }
            case LBRACEnum:{
                printf("%d\t\t%d\t\tLBRACEnum\n", yyline, yycolumn);
                break;
            }
            case LEnum:{
                printf("%d\t\t%d\t\tLEnum\n", yyline, yycolumn);
                break;
            }
            case LTnum:{
                printf("%d\t\t%d\t\tLTnum\n", yyline, yycolumn);
                break;
            }
            case MINUSnum:{
                printf("%d\t\t%d\t\tMINUSnum\n", yyline, yycolumn);
                break;
            }
            case NOTnum:{
                printf("%d\t\t%d\t\tNOTnum\n", yyline, yycolumn);
                break;
            }
            case PLUSnum:{
                printf("%d\t\t%d\t\tPLUSnum\n", yyline, yycolumn);
                break;
            }
            case RBRACEnum:{
                printf("%d\t\t%d\t\tRBRACEnum\n", yyline, yycolumn);
                break;
            }
            case RETURNnum:{
                printf("%d\t\t%d\t\tRETURNnum\n", yyline, yycolumn);
                break;
            }
            case SCONSTnum:{
                printf("%d\t\t%d\t\tSCONSTnum\t%d\n", yyline, yycolumn, yylval);
                break;
            }
            case TIMESnum:{
                printf("%d\t\t%d\t\tTIMESnum\n", yyline, yycolumn);
                break;
            }
            case VOIDnum:{
                printf("%d\t\t%d\t\tVOIDnum\n", yyline, yycolumn);
                break;
            }
            case EOFnum:{
                printf("%d\t\t%d\t\tEOFnum\n", yyline, yycolumn);
                break;
            }
            case COMMENTnum:{
                break;
            }
            case SPACESnum:{
                break;
            }
            case WRONGCOMMENTnum:{
                printf("Error: A problem about comment is found at line %d, column%d!\n", yyline, yycolumn);
                break;
            }
            case WRONGIDnum:{
                printf("Error: Malformed identifier at line%d, column%d!\n", yyline, yycolumn);
                break;
            }
            case WRONGSTRINGnum:{
                printf("Error: Unmatched string constant, at line%d, column%d!\n", yyline, yycolumn);
                break;
            }
            case UNRECOGINZEDnum:{
                printf("Error: Undefined symbol at line%d, column%d!\n", yyline, yycolumn);
                break;
            }
        }
        if (lexRtn == EOFnum) break;
    }

    printf("\nString Table:");
    for(int i=0;i<numofstring;i++){
        putchar(String_table[i]);

    }
    printf("\n");
    return 0;
}
