%define api.value.type { ParserValue }

%code requires {
#include <iostream>
#include <vector>
#include <string>

#include "parser_util.hh"
#include "symbol.hh"

}

%code {

#include <cstdlib>

extern int yylex();
extern int yyparse();

extern NodeStmts* final_values;

SymbolTable symbol_table;

int yyerror(std::string msg);

}

%token TPLUS TDASH TSTAR TSLASH
%token <lexeme> TINT_LIT TIDENT TDATA
%token  TLET TDBG
%token TSCOL TLPAREN TRPAREN TEQUAL
%token TQUESTION TCOLON

%type <node> Expr Stmt
%type <stmts> Program StmtList

%left TPLUS TDASH
%left TSTAR TSLASH
%right TQUESTION TCOLON

%%
/* include assignment  */
Program :                
        { final_values = nullptr; }
        | StmtList TSCOL 
        { final_values = $1; }
	    ;

StmtList : Stmt                
         { $$ = new NodeStmts(); $$->push_back($1); }
	     | StmtList TSCOL Stmt 
         { $$->push_back($3); }
	     ;

Stmt : TLET TIDENT TCOLON TDATA TEQUAL Expr
     {
        if($6->data_type =="int" && $4=="short")
        {
            yyerror("Type mismatch.\n");
        }
        if($6->data_type == "long" &&( $4=="int" || $4=="short"))
        {
            yyerror("Type mismatch.\n");
        }
            $$ = new NodeLet($2,$4,$6);
     }
     |
      TIDENT TEQUAL Expr
    {
        if(!symbol_table.contains($1)) {
            // tried to redeclare variable, so error
            yyerror("tried to redeclare variable.\n");
        } else {
            symbol_table.insert($1);

            $$ = new NodeAssn($1, $3);
        }
    }
    |
    TDBG Expr
    { 
        $$ = new NodeDebug($2);
    }
     ;

Expr : TINT_LIT               
     {
        $$ = new NodeInt(stol($1)); 
     }
     | TIDENT
     { 
        if(symbol_table.contains($1))
            $$ = new NodeIdent($1); 
        else
            yyerror("using undeclared variable.\n");
     }
     | Expr TPLUS Expr
     { $$ = new NodeBinOp(NodeBinOp::PLUS, $1, $3); }
     | Expr TDASH Expr
     { $$ = new NodeBinOp(NodeBinOp::MINUS, $1, $3); }
     | Expr TSTAR Expr
     { $$ = new NodeBinOp(NodeBinOp::MULT, $1, $3); }
     | Expr TSLASH Expr
     { $$ = new NodeBinOp(NodeBinOp::DIV, $1, $3); }
     | TLPAREN Expr TRPAREN { $$ = $2; }
     | Expr TQUESTION Expr TCOLON Expr
     { $$ = new NodeTernary(NodeTernary::TERN_OP, $1, $3, $5); }
     ;

%%

int yyerror(std::string msg) {
    std::cerr << "Error! " << msg << std::endl;
    exit(1);
}