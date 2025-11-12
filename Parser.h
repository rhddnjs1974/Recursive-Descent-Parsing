#pragma once
#include <string>

using namespace std;

typedef struct state {
    int token_type;
    string lexeme;
} state;

typedef struct id {
    string lexeme;
    int value;
} id;

void Program();
void Statements();
void Stmts_tail();
void Statement(const state* c, int* real_curr, id* ident);
int Expression(const state* c, int* real_curr, id* ident);
int Term_tail(const state* c, int* real_curr, id* ident, int term_result);
int Term(const state* c, int* real_curr, id* ident);
int Factor_tail(const state* c, int* real_curr, id* ident, int factor_result);
int Factor(const state* c, int* real_curr, id* ident);
int Const(const state* c, int* real_curr);
int* Ident(const state* c, int* real_curr, id* ident);
void Assignment_op(const state* c, int* real_curr);
void Semi_colon(const state* c, int* real_curr);
int Add_operator(const state* c, int* real_curr);
int Mult_operator(const state* c, int* real_curr);
void Left_paren(const state* c, int* real_curr);
void Right_paren(const state* c, int* real_curr);
