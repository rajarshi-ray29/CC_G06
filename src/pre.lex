/* just like Unix wc */
%option noyywrap
%option prefix="foo"

%x comment
%x comment2
%x DEFINE
%x DEFINE2
%x UNDEF
%x IFDEF
%x IFDEF2
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
<DEFINE>" " {BEGIN(DEFINE2); return 1;}
<DEFINE2>[^\\\n]+ {if(map[key] == "1") map[key] = ""; map[key] += yytext; return 5;}
<DEFINE2>"\\\n" {return 1;}
<DEFINE2>[\n]+ {BEGIN(INITIAL); return 1;}

"#undef " {BEGIN(UNDEF); return 2;}
<UNDEF>[a-zA-Z]+ {map.erase(yytext); return 2;}
<UNDEF>[ \n]+ {BEGIN(INITIAL); return 2;}

"#ifdef " {BEGIN(IFDEF); return 1;}
<IFDEF>[a-zA-Z]+ {key=yytext; if(map.find(key)!=map.end()) BEGIN(IFDEF2); return 1;}
<IFDEF>[^#a-zA-Z]+
<IFDEF>[#a-zA-Z]+ {key = yytext; if(key=="endif") BEGIN(INITIAL); else BEGIN(IFDEF); return 1;}
<IFDEF2>"\n" {BEGIN(INITIAL); return 1;}

"#endif" {BEGIN(ENDIF); return 1;}
<ENDIF>[\n]+ {BEGIN(INITIAL); return 1;}

"/*"         BEGIN(comment);
<comment>[^*]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/]*   /* eat up '*'s not followed by '/'s */
<comment>"*"+"/"        {BEGIN(INITIAL);}

"//"    BEGIN(comment2);
<comment2>. /* om nom */
<comment2>[ \n]+ {BEGIN(INITIAL);}

[a-zA-Z]+ {return 3;}
. {return 4;}
%%