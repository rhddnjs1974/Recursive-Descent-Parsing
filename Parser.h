#pragma once
#include <string>
#include "tokens.h"

using namespace std;

typedef struct state {
    int token_type;
    string lexeme;
} state;

typedef struct id {
    string lexeme;
    int value = 0;
    int is_unknown = UNKNOWN;//0이면 정의되지 않음 or 정의 되지 않은 변수를 참조함, 오류
} id;


void Statement(const state* c, int* real_curr, int* end_curr, id* ident);

id Expression(const state* c, int* real_curr, id* ident, id result);
id Term_tail(const state* c, int* real_curr, id* ident, id result);
id Term(const state* c, int* real_curr, id* ident, id result);
id Factor_tail(const state* c, int* real_curr, id* ident, id result);
id Factor(const state* c, int* real_curr, id* ident, id result);

int Const(const state* c, int* real_curr);
int Ident(const state* c, int* real_curr, id* ident);
void Assignment_op(const state* c, int* real_curr);
void Semi_colon(const state* c, int* real_curr);
int Add_operator(const state* c, int* real_curr);
int Mult_operator(const state* c, int* real_curr);
void Left_paren(const state* c, int* real_curr);
void Right_paren(const state* c, int* real_curr);
