%option noyywrap
%option prefix="foo"

%x COMMENT
%x COMMENT_ALT
%x DEFINE
%x DEFINE_ALT
%x UNDEF
%x IFDEF
%x IFDEF_ALT
%x ELIF
%x ELSE
%x ENDIF
%{
#include <string>
#include <unordered_map>
using namespace std;

string key;
unordered_map<string, string> map;
%}
%%

"#def " {BEGIN(DEFINE); return 1;}
<DEFINE>[a-zA-Z]+ {key = yytext; map[key]="1"; return 1;}
<DEFINE>[\n]+ {BEGIN(INITIAL); return 1;}
<DEFINE>" " {BEGIN(DEFINE_ALT); return 1;}
<DEFINE_ALT>[^\\\n]+ {if(map[key] == "1") map[key] = ""; map[key] += yytext; return 5;}
<DEFINE_ALT>"\\\n" {return 1;}
<DEFINE_ALT>[\n]+ {BEGIN(INITIAL); return 1;}

"#undef " {BEGIN(UNDEF); return 2;}
<UNDEF>[a-zA-Z]+ {map.erase(yytext); return 2;}
<UNDEF>[ \n]+ {BEGIN(INITIAL); return 2;}

"#ifdef " {BEGIN(IFDEF); return 1;}
<IFDEF>[a-zA-Z]+ {key=yytext; if(map.find(key)!=map.end()) BEGIN(IFDEF_ALT); return 1;}
<IFDEF>[^#a-zA-Z]+
<IFDEF>[#a-zA-Z]+ {key = yytext; if(key=="endif") BEGIN(INITIAL); else BEGIN(IFDEF); return 1;}
<IFDEF_ALT>"\n" {BEGIN(INITIAL); return 1;}

"#elif " {BEGIN(ELIF); return 1;}
<ELIF>[a-zA-Z]+ {key=yytext; if(map.find(key)!=map.end() && map[key] == "1") BEGIN(IFDEF_ALT); return 1;}
<ELIF>[^#a-zA-Z]+
<ELIF>[#a-zA-Z]+ {key = yytext; if(key=="endif") BEGIN(INITIAL); else if (key == "else") BEGIN(ELSE); return 1; }
<IFDEF_ALT>"\n" {BEGIN(INITIAL); return 1;}

"#else" {BEGIN(ELSE); return 1;}
<ELSE>[\n]+ {BEGIN(INITIAL); return 1;}

"#endif" {BEGIN(ENDIF); return 1;}
<ENDIF>[\n]+ {BEGIN(INITIAL); return 1;}

"/*"         BEGIN(COMMENT);
<COMMENT>[^*]*        
<COMMENT>"*"+[^*/]*   
<COMMENT>"*"+"/"        {BEGIN(INITIAL);}

"//"    BEGIN(COMMENT_ALT);
<COMMENT_ALT>. 
<COMMENT_ALT>[ \n]+ {BEGIN(INITIAL);}

[a-zA-Z]+ {return 3;}
. {return 4;}
%%