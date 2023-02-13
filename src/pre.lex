/* just like Unix wc */
%option noyywrap
%option prefix="foo"

%x comment
%x comment2
%x DEFINE
%x DEFINE2
%x UNDEF
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