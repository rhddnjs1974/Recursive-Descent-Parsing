#include "Parser.h"
#include <iostream>
using namespace std;

#define CONST 11
#define IDENT 12
#define SEMI_COLON 19
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define UNKNOWN 99

using namespace std;

int* isunknown = nullptr;
int  isunknown_n = 0;
int  last_ident_index = -1;

int id_counter = 3;

void Program() {
   //같이 처리 고민
}
void Statements() {
   //같이 처리 고민
}
void Stmts_tail() {
   //같이 처리 고민
}
void Statement(const state* c, int* real_curr, id* ident) {
   int this_curr, new_curr, result;
   int* idp;
   result = UNKNOWN;
   this_curr = *real_curr;
   idp = Ident(c, real_curr, ident);
   if (this_curr != *real_curr) {
      new_curr = *real_curr;
      Assignment_op(c, real_curr);
      if (new_curr != *real_curr) {
         //":="맞음
         new_curr = *real_curr;
         result = Expression(c, real_curr, ident);
         if (new_curr != *real_curr) {
            *idp = result;
            return;
         }
         //<experssion>은 항상 1이상의 길이임, 아마도 factor에서 걸러져서 여기는 안올듯
         else {
            cout << "error from statemet not experssion, 아마도 출력할 일 없을 것." << endl;
            return;
         }
      }
      //":="아님 오류
      else {
         cout << ":= 아님 오류, 오류구문 만들어야함" << endl;
         return;
      }
   }
   //ident 아님 오류
   else {
      cout << "error from statemet not ident, 오류구문 만들어야함" << endl;
      return;
   }
}
int Expression(const state* c, int* real_curr, id* ident) {
   int this_curr, result, mult_op;
   result = UNKNOWN;
   this_curr = *real_curr;
   result = Term(c, real_curr, ident);
   if (this_curr != *real_curr) {
      return Term_tail(c, real_curr, ident, result);
   }
   //일단 오류인데 뭘까...
   cout << "error from eperssion, 오류구문 만들어야함" << endl;
   return UNKNOWN; //추가함
}
int Term_tail(const state* c, int* real_curr, id* ident, int term_result) {
   int this_curr, new_curr, result, add_op;
   result = UNKNOWN;
   add_op = UNKNOWN;
   this_curr = *real_curr;
   add_op = Add_operator(c, real_curr);
   if (this_curr != *real_curr) {
      //"+" or "-"
      new_curr = *real_curr;
      result = Term(c, real_curr, ident);
      if (new_curr != *real_curr) {//이미 *real_curr; 다음값 보고 있으므로 그냥 리턴해도 됨
         if (add_op == ADD_OP) {
            return Term_tail(c, real_curr, ident, term_result + result);
         }
         else if (add_op == SUB_OP) {
            return Term_tail(c, real_curr, ident, term_result - result);
         }
         else {//term가 어셉되서 이거 되면 프로그램 오류
            cout << "error from term_tail not add_op" << endl;
         }
      }
      //Add_operator여기서 밖에 없으므로 뒤가 어셉 안되면 오류
      else {
         cout << "error from term_tail not term, 오류구문 만들어야함" << endl;
      }
   }
   //엡실론이므로 받은 값 그대로 반환
   return term_result;
}
int Term(const state* c, int* real_curr, id* ident) {
   //Expression에서 몇개 바꿈
   int this_curr, result, mult_op;
   result = UNKNOWN;
   this_curr = *real_curr;
   result = Factor(c, real_curr, ident);
   if (this_curr != *real_curr) {
      return Factor_tail(c, real_curr, ident, result);
   }
   //일단 오류인데 뭘까...
   cout << "error from term, 오류구문 만들어야함" << endl;
   return UNKNOWN; // 추가함
}
int Factor_tail(const state* c, int* real_curr, id* ident, int factor_result) {
   //Term_tail에서 몇개 바꿈
   int this_curr, new_curr, result,mult_op;
   result = UNKNOWN;
   mult_op = UNKNOWN;
   this_curr = *real_curr;
   mult_op = Mult_operator(c, real_curr);
   if (this_curr != *real_curr) {
      //"*" or "/"
      new_curr = *real_curr;
      result = Factor(c, real_curr, ident);
      if (new_curr != *real_curr) {//이미 *real_curr; 다음값 보고 있으므로 그냥 리턴해도 됨
         if (mult_op == MULT_OP) {
            return Factor_tail(c, real_curr, ident, factor_result * result);
         }
         else if(mult_op == DIV_OP) {
            return Factor_tail(c, real_curr, ident, factor_result / result);
         }
         else {//factor가 어셉되서 이거 되면 프로그램 오류
            cout << "error from factor_tail not mult_op" << endl;
         }
      }
      //Mult_operator여기서 밖에 없으므로 뒤가 어셉 안되면 오류
      else {
         cout << "error from factor_tail not factor, 오류구문 만들어야함" << endl;
      }
   }
   //엡실론이므로 받은 값 그대로 반환
   return factor_result;
}
int Factor(const state* c, int* real_curr, id* ident) {
   int this_curr, new_curr, result;
   result = UNKNOWN;
   this_curr = *real_curr;
   Left_paren(c, real_curr);
   if (this_curr != *real_curr) {
      //"("맞음
      new_curr = *real_curr;
      result = Expression(c, real_curr, ident);
      if (new_curr != *real_curr) {
         //expression 제대로 동작함
         new_curr = *real_curr;
         Right_paren(c, real_curr);
         if (new_curr != *real_curr) {
            //<left_paren><expression><right_paren> 어셉
            return result;
         }
         //")"아님, "("있었기에 오류
         else
            cout << ") 아님, ( 있었기에 오류, 오류구문 만들어야함" << endl;
      }
      //<expression>아님, "("있었기에 오류
      else
         cout << "<expression> 아님, ( 있었기에 오류, 오류구문 만들어야함" << endl;
   }
   //"("아님, <ident>,<const> 실행해보기
   *real_curr = this_curr;
   result = *Ident(c, real_curr, ident);//foctor에서는 값이 필요하므로 주소값에서 변수값 참조
   if (this_curr != *real_curr) {//이미 real_curr 다음것 보고 있음
      return result;//<ident>
   }
   //이미*real_curr == this_curr
   result = Const(c, real_curr);
   if (this_curr != *real_curr) {//이미 real_curr 다음것 보고 있음
      return result;//<const>
   }
   //오류임
   cout << "error from factor 모든 구문문법 아님, 오류구문 만들어야함" << endl;
   return result;
}
int Const(const state* c, int* real_curr) {
   //문자열 숫자 int로 바꿔서 반환
   if (c[*real_curr].token_type == CONST)
      return stoi(c[(*real_curr)++].lexeme);
   return UNKNOWN;//unknown값(99)들어가버림,차피 사용 안할거라 상관 없음
}
int* Ident(const state* c, int* real_curr, id* ident) {
   //값을 받을 수도 있어서 주소값 반환
   //호출 되는 곳: Statement, Factor
   //Statement에서 주소값에 값 넣기
   //Factor에서 주소값참조로 값 찾기
   if (c[*real_curr].token_type == IDENT) {
      for (int i = 0; i < id_counter; i++) {
         if (c[*real_curr].lexeme == ident[i].lexeme) {
            (*real_curr)++;
            return &(ident[i].value);
         }
      }
      //정의되지 않은 ID값, 프로그램 오류 아니면 실행X
      cout << "ID인데 정의되지 않은 변수명임" << endl;
      return NULL;
   }
   return NULL;//차피 반환값 사용 안할거라 상관 없음
}
void Assignment_op(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == ASSIGN_OP)
      (*real_curr)++;
   return;
}
void Semi_colon(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == SEMI_COLON)
      (*real_curr)++;
   return;
}
int Add_operator(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == ADD_OP) {
      (*real_curr)++;
      return ADD_OP;
   }
   else if (c[*real_curr].token_type == SUB_OP) {
      (*real_curr)++;
      return SUB_OP;
   }
   else return UNKNOWN;
}
int Mult_operator(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == MULT_OP) {
      (*real_curr)++;
      return MULT_OP;
   }
   else if (c[*real_curr].token_type == DIV_OP) {
      (*real_curr)++;
      return DIV_OP;
   }
   return UNKNOWN;
}
void Left_paren(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == LEFT_PAREN)
      (*real_curr)++;
   return;
}
void Right_paren(const state* c, int* real_curr) {
   if (c[*real_curr].token_type == RIGHT_PAREN)
      (*real_curr)++;
   return;
}