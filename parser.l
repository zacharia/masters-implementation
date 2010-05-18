/*
Parser - flex++ file

Zacharia Crumley

*/

%{

//Make sure that none of these are 0 or less. 0 and -1 are reserved for other stuff.
#define ASSIGN 2
#define PLUS 6
#define MINUS 7
#define DIVIDE 8
#define MULTIPLY 9
#define RULE_SEPARATOR 10
#define START_SYMBOL 11
#define OPEN_BRACKET 12
#define CLOSE_BRACKET 13
#define STACK_PUSH 14
#define STACK_POP 15
#define SYMBOL 16
#define NUMBER 17

//if you want stuff to be echoed uncomment this line
#define __ECHO_ON__ ECHO;
//otherwise uncomment this line
//#define __ECHO_ON__

%}

%option warn
%option noyywrap
%option yylineno
%option nodefault

int [0-9]+
float [0-9]+\.[0-9]+
number {int}|{float}

symbol [A-Za-z]+

%%

"->"			{ __ECHO_ON__ return ASSIGN; }

"+"			{ __ECHO_ON__ return PLUS; }
"-"			{ __ECHO_ON__ return MINUS; }
"/"			{ __ECHO_ON__ return DIVIDE; }
"*"			{ __ECHO_ON__ return MULTIPLY; }

";"			{ __ECHO_ON__ return RULE_SEPARATOR; }
"$"			{ __ECHO_ON__ return START_SYMBOL; }

"("			{ __ECHO_ON__ return OPEN_BRACKET; }
")"			{ __ECHO_ON__ return CLOSE_BRACKET; }

{symbol}                { __ECHO_ON__ return SYMBOL; }

{number}		{ __ECHO_ON__ return NUMBER; }

<<EOF>>			{ return -1; }

<*>.|\n			{ __ECHO_ON__ }

%%

