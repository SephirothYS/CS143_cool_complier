/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%option noyywrap
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;


extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

%}

%Start COMMENT_BLOCK
%Start SINGLE_LINE_COMMENT
%Start STRING_CONST_BEGIN

/*
 * Define names for regular expressions here.
 */
CLASS           [Cc][Ll][Aa][Ss][Ss]
ELSE            [Ee][Ll][Ss][Ee]
IF              [Ii][Ff]
FI              [Ff][Ii]
IN              [Ii][Nn]
INHERITS        [Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
LET             [Ll][Ee][Tt]
LOOP            [Ll][Oo][Oo][Pp]
POOL            [Pp][Oo][Oo][Ll]
THEN            [Tt][Hh][Ee][Nn]
WHILE           [Ww][Hh][Ii][Ll][Ee]
CASE            [Cc][Aa][Ss][Ee]
ESAC            [Ee][Ss][Aa][Cc]
OF              [Oo][Ff]
NEW             [Nn][Ee][Ww]
ISVOID          [Ii][Ss][Vv][Oo][Ii][Dd]
INT_CONST       [0-9]+
BOOL_CONST      (t[Rr][Uu][Ee])|(f[Aa][Ll][Ss][Ee])
TYPEID          [A-Z][A-Za-z0-9_]*
OBJECTID        [a-z][A-Za-z0-9_]*
ASSIGN          <-
NOT             [Nn][Oo][Tt]
LE              <=
WHITE_SPACE     [ \t\f\r\v]+
DARROW          =>
LET_STMT        [Ll][Ee][Tt]_[Ss][Tt][Mm][Tt]

%%

 /*
  *  Nested comments
  */

<COMMENT_BLOCK>{
  

[^\n(*)]* {}

"*)" {
  BEGIN INITIAL;
}

[(*)] {}

"\n" {
  curr_lineno++;
}

<<EOF>> {
  yylval.error_msg = "EOF in comment";
  BEGIN INITIAL;
  return ERROR;
}

}

 /*
  *   The single-line comment
  */

<SINGLE_LINE_COMMENT>{


.* {}

"\n" {
  curr_lineno++;
  BEGIN INITIAL;
}

}

<STRING_CONST_BEGIN>{

<<EOF>> {
  yylval.error_msg = "EOF in string constant";
  BEGIN INITIAL;
  return ERROR;
}

[^\\\"\n]* {
  yymore();
}


\n {
  yylval.error_msg = "Unterminated string constant";
  BEGIN INITIAL;
  curr_lineno++;
  return ERROR;
}

\\\n {
  curr_lineno++;
  yymore();
}

\\[^\n] {
  yymore();
}

\\ {
  yymore();
}

\" {
  int n = yyleng;
  std::string ans;  

  for(int i = 0 ; i  < n ; i++ ){
    if( yytext[i] == '\0'){
      yylval.error_msg = "String contains null character.";
      BEGIN INITIAL;
      return ERROR;
    }
  }

  std::string str = yytext;
  str = str.substr(0,str.size() - 1);
  n = str.size();
  for(int i = 0 ; i < n; i++){
    if(str[i] == '\\'){
      if(str[i + 1] == 'b'){
        ans += '\b';
      }
      else if(str[i + 1] == 't'){
        ans += '\t';
      }
      else if(str[i + 1] == 'n'){
        ans += '\n';
      }
      else if(str[i + 1] == 'f'){
        ans += '\f';
      }
      else{
        ans += str[i + 1];
      }
      i++;
    }
    else{
      ans += str[i];
    }
  }

  if(ans.size() <= MAX_STR_CONST - 1){
    cool_yylval.symbol = stringtable.add_string(const_cast<char*>(ans.c_str()));
    BEGIN INITIAL;
    return (STR_CONST);
  }
  else{
    cool_yylval.error_msg = "String constant too long";
    BEGIN INITIAL;
    return ERROR;
  }
}

}

<INITIAL>{

{WHITE_SPACE} {}

"\n" {
  curr_lineno++;
}

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */

{CLASS}       {return (CLASS);}

{ELSE}        {return (ELSE);}

{IF}          {return (IF);}

{FI}          {return (FI);}

{IN}          {return (IN);}

{INHERITS}    {return (INHERITS);}

{LET}         {return (LET);}

{LOOP}        {return (LOOP);}

{POOL}        {return (POOL);}

{THEN}        {return (THEN);}

{WHILE}       {return (WHILE);}

{CASE}        {return (CASE);}

{ESAC}        {return (ESAC);}

{OF}          {return (OF);}

{NEW}         {return (NEW);}

{ISVOID}      {return (ISVOID);}

{NOT}         {return (NOT);}

{LET_STMT}    { }

error         { }

{INT_CONST} {
  cool_yylval.symbol = inttable.add_string(yytext);
  return (INT_CONST);
}

{BOOL_CONST} {
  if (yytext[0] == 't') cool_yylval.boolean = 1;
  else cool_yylval.boolean = 0;
  return (BOOL_CONST);
}

{TYPEID} {
  cool_yylval.symbol = idtable.add_string(yytext);
  return (TYPEID);
}

{OBJECTID} {
  cool_yylval.symbol = idtable.add_string(yytext);
  return (OBJECTID);
}



 /*
  *   The single-character operators.
  */

"+"         {return static_cast<int>('+');}

"-"         {return static_cast<int>('-');}

"*"         {return static_cast<int>('*');}

"/"         {return static_cast<int>('/');}

"~"         {return static_cast<int>('~');}

"<"         {return static_cast<int>('<');}

"="         {return static_cast<int>('=');}

"("         {return static_cast<int>('(');}

")"         {return static_cast<int>(')');}

"{"         {return static_cast<int>('{');}

"}"         {return static_cast<int>('}');}

";"         {return static_cast<int>(';');}

","         {return static_cast<int>(',');}

":"         {return static_cast<int>(':');}

"@"         {return static_cast<int>('@');}

"."         {return static_cast<int>('.');}





 /*
  *  The multiple-character operators.
  */

{DARROW}		{ return (DARROW); }

{LE}        {return (LE);}

{ASSIGN}    {return (ASSIGN);}

"--" {
  BEGIN SINGLE_LINE_COMMENT;
}

"(*" {
  BEGIN COMMENT_BLOCK;
}

"*)" {
  yylval.error_msg = "Unmatched *).";
  return ERROR;
}

"\"" {
  BEGIN STRING_CONST_BEGIN;
}


. {
  yylval.error_msg = yytext;
  return ERROR;
}

}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%
