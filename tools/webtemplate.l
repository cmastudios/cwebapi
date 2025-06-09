%{
#include <iostream>
#include <sstream>
#include <stdio.h>
static std::stringstream curr;
static enum struct Mode { NORMAL, STATEMENT, EXPRESSION} mode = Mode::NORMAL;
static void flush(bool eol=false) {
    if (curr.str().empty()) return;
    switch (mode) {
        case Mode::NORMAL:
            std::cout << "out << \"" << curr.str() << "\"";
            break;
        case Mode::STATEMENT:
            std::cout << curr.str();
            break;
        case Mode::EXPRESSION:
            std::cout << "out << " << curr.str();
            break;
    }
    curr.str("");
    if (eol) {
        std::cout << " << std::endl";
    }
        switch (mode) {
        case Mode::NORMAL:
        case Mode::EXPRESSION:
            std::cout << ";";
            break;
    }
    std::cout << std::endl;
}
static void change_mode(Mode new_mode) {
    flush();
    mode = new_mode;
}
%}
%option noyywrap

%%
"{%" change_mode(Mode::STATEMENT);
"%}" change_mode(Mode::NORMAL);
"{{" change_mode(Mode::EXPRESSION);
"}}" change_mode(Mode::NORMAL);
. if (*yytext == '"' || *yytext == '\\') {curr << '\\'; } curr << yytext;
\n flush(true);
%%

int main(int argc, char *argv[])
{
    ++argv, --argc;  /* skip over program name */
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

    yylex();
}
