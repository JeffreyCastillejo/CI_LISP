%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbTable;
}

%token <sval> FUNC SYMBOL TYPE
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL QUIT LET

%type <astNode> s_expr f_expr number

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
           fprintf(stderr, "yacc: s_expr ::= number\n");
           $$ = $1;
       }
       | f_expr {
       	fprintf(stderr, "yacc: s_expr ::= f_expr\n");
           $$ = $1;
       }
       | QUIT {
           fprintf(stderr, "yacc: s_expr ::= QUIT\n");
           exit(EXIT_SUCCESS);
       }
       | error {
           fprintf(stderr, "yacc: s_expr ::= error\n");
           yyerror("unexpected token");
           $$ = NULL;
       };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode(NULL, $1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode(NULL, $1, DOUBLE_TYPE);
 }
    | TYPE INT {
	fprintf(stderr, "yacc: number ::= TYPE INT_LITERAL\n");
	$$ = createNumberNode($1, $2, INT_TYPE);
    }
    | TYPE DOUBLE {
	fprintf(stderr, "yacc: number ::= TYPE DOUBLE_LITERAL\n");
	$$ = createNumberNode($1, $2, DOUBLE_TYPE);
    };


f_expr:
     LPAREN FUNC s_expr RPAREN {
            fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
            $$ = createFunctionNode($2, $3, NULL);
        }
        | LPAREN FUNC s_expr s_expr RPAREN {
            fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
            $$ = createFunctionNode($2, $3, $4);
        }

%%

