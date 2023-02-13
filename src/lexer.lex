%option noyywrap
%{
#include "parser.hh"
#include <string>
#include <map>
#include <stack>
 
extern int yyerror(std::string msg);
 
std::map<std::string, std::string> macros;
std::stack<int> multiline_comment_stack;
 
inline void push_multiline_comment(int state) {
multiline_comment_stack.push(state);
}
 
inline void pop_multiline_comment() {
multiline_comment_stack.pop();
}
inline bool in_multiline_comment() {
return !multiline_comment_stack.empty();
}
%}
 
%%
 
"+" { return TPLUS; }
"-" { return TDASH; }
"*" { return TSTAR; }
"/" { return TSLASH; }
";" { return TSCOL; }
"(" { return TLPAREN; }
")" { return TRPAREN; }
"=" { return TEQUAL; }
"dbg" { return TDBG; }
"let" { return TLET; }
[0-9]+ { yylval.lexeme = std::string(yytext); return TINT_LIT; }
 
 
[a-zA-Z]+ yylval.lexeme = std::string(yytext); return TIDENT;
 
[ \t\n]+ { /* skip */ }
 
"//".* { /* skip */ }
 
[/][][^*]*[*]+([^/*][^*]*[*]+)*[/] { /* skip */ }
 
"#def" { BEGIN(DEF); }
<DEF>[a-zA-Z]+ { yylval.lexeme = std::string(yytext); BEGIN(DEF_IDENT); }
 
<DEF_IDENT>.*\n {
macros[yylval.lexeme] = std::string(yytext + yyleng);
BEGIN(INITIAL);
}
 
"#undef" { BEGIN(UNDEF); }
<UNDEF>[a-zA-Z]+ {
if (macros.count(std::string(yytext)) > 0) {
macros.erase(std::string(yytext));
}
BEGIN(INITIAL);
}
. { yyerror("unknown char"); }
 
%%
 
std::string token_to_string(int token, const char *lexeme) {
if (macros.count(std::string(lexeme)) > 0) {
return macros[std::string(lexeme)];
}
 
std::string s;
switch (token) {
    case TPLUS: s = "TPLUS"; break;
    case TDASH: s = "TDASH"; break;
    case TSTAR: s = "TSTAR"; break;
    case TSLASH: s = "TSLASH"; break;
        case TSCOL: s = "TSCOL"; break;
        case TLPAREN: s = "TLPAREN"; break;
        case TRPAREN: s = "TRPAREN"; break;
        case TEQUAL: s = "TEQUAL"; break;
        
        case TDBG: s = "TDBG"; break;
        case TLET: s = "TLET"; break;
        
        case TINT_LIT: s = "TINT_LIT"; s.append("  ").append(lexeme); break;
        case TIDENT: s = "TIDENT"; s.append("  ").append(lexeme); break;
    }
 
    return s;
}