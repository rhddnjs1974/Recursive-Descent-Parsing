#include "Parser.h"
#include "tokens.h"
#include <iostream>

using namespace std;

//v1 수정사항 id.is_unknown추가, *real_curr++ 를 (*real_curr)++ 로 수정
//의미 오류는 (OK) 처리


int id_counter = 0;

void Statement(const state* c, int* real_curr, int* end_curr, id* ident) {
	int this_curr, new_curr;
	id result;
	int id_num;
	result.value = UNKNOWN;
	this_curr = *real_curr;
	id_num = Ident(c, real_curr, ident);
	if (this_curr != *real_curr) {
		new_curr = *real_curr;
		Assignment_op(c, real_curr);
		if (new_curr != *real_curr) {
			//":="맞음
			new_curr = *real_curr;
			result = Expression(c, real_curr, ident, result);
			if (new_curr != *real_curr) {
				if (result.is_unknown == ERROR_PS)return;

				//완전히 파싱됐을 때만 여기 옴
				//문장 끝이 맞는지 확인
				if (*real_curr == *end_curr) {//==아니고 >여야 할수도 있음
					if (result.is_unknown == NO_PROB) {
						ident[id_num].value = result.value;
						ident[id_num].is_unknown = NO_PROB;//선언 됨
					}
					else {//result.is_unknown == UNKNOWN
						//정의 되지 않은 값을 집어 넣었으니 이미 선언되어 있었더라도 다시 unknown됨
						ident[id_num].is_unknown = UNKNOWN;
						//cout << "error from statemet experssion unknown, 아마도 출력 안할 건데 확인용." << endl;
					}
					cout << "(OK)\n\n";
					return;
				}
				//+,-,*,/,; 가능성 없음
				//ID,CONST,(,),:=이면 오류

				switch (c[*real_curr].token_type) {
				case IDENT:
				case CONST:
				case LEFT_PAREN:
					cout << "(ERROR) 연산자(OP) 부족\n\n"; break;
				case ASSIGN_OP:
					cout << "(ERROR) 중복 \":=\" 발생\n\n"; break;
				case RIGHT_PAREN:
					cout << "(ERROR) LEFT_PAREN\"(\" 부족\n\n"; break;
				default: break;
				}
				return;


			}
			//<experssion> 길이 0 오류
			//뒤에 더 있을 수도 없을 수도 있지만 첫번째가 피연산자가 아님
			//오류라면 값이 정의 되지 않음
			else {
				//이미 <experssion>에서 처리
				return;
			}
		}
		//":="아님 오류, 필요한 오류
		//오류라면 값이 정의 되지 않음
		else {
			ident[id_num].is_unknown = UNKNOWN;
			cout << "(ERROR) 식별자 뒤에는 대입 연산자 ':='가 필요합니다.\n\n";
			return;
		}
	}
	//ident 아님 오류, 필요한 오류
	//정의될 변수자체가 없음
	else {
		cout << "(ERROR) 문장은 식별자(ID)로 시작해야 합니다.\n\n";
		return;
	}
}
id Expression(const state* c, int* real_curr, id* ident, id result) {
	int this_curr;
	this_curr = *real_curr;
	result = Term(c, real_curr, ident, result);
	if (this_curr != *real_curr) {
		if (result.is_unknown == ERROR_PS)return result;
		return Term_tail(c, real_curr, ident, result);
	}
	//일단 오류인데 뭘까...
	//길이 0 오류인데 statement,factor에 전달

	result.is_unknown = ERROR_PS;
	cout << "(ERROR) 피연산자(ID,CONST) 부족\n\n";
	return result;
}
id Term_tail(const state* c, int* real_curr, id* ident, id result) {
	int this_curr, new_curr, add_op;
	id new_result;
	new_result.value = UNKNOWN;
	add_op = UNKNOWN;
	this_curr = *real_curr;
	add_op = Add_operator(c, real_curr);
	if (this_curr != *real_curr) {
		//"+" or "-"
		new_curr = *real_curr;
		new_result = Term(c, real_curr, ident, new_result);
		if (new_curr != *real_curr) {//이미 *real_curr; 다음값 보고 있으므로 그냥 리턴해도 됨
			if (new_result.is_unknown == ERROR_PS)return new_result;//이미 오류 있었음
			if (add_op == ADD_OP) {
				if (new_result.is_unknown == NO_PROB) {
					result.value = result.value + new_result.value;
				}
				else {
					//선언 안된 변수 참조함
					result.is_unknown = UNKNOWN;
				}
			}
			else if (add_op == SUB_OP) {
				if (new_result.is_unknown == NO_PROB) {
					result.value = result.value - new_result.value;
				}
				else {
					//선언 안된 변수 참조함
					result.is_unknown = UNKNOWN;
				}

			}
			else {
				//Add_operator가 어셉되서 이거 되면 프로그램 오류
				//출력될 일 없음
				cout << "error from term_tail not add_op" << endl;
			}
			//계속 파싱
			return Term_tail(c, real_curr, ident, result);
		}
		//Add_operator여기서 밖에 없으므로 뒤가 어셉 안되면 오류
		//오류라면 값이 정의 되지 않음
		else {
			result.is_unknown = ERROR_PS;
			//ID,CONST,( 가능성 없음
			//+,-,*,/,),:=,;이면 오류
			switch (c[*real_curr].token_type) {
			case ADD_OP:
			case SUB_OP:
			case MULT_OP:
			case DIV_OP:
				cout << "(ERROR) 중복 연산자(" << c[*real_curr - 1].lexeme << " " << c[*real_curr].lexeme << ") 발생\n\n"; break;
			case ASSIGN_OP:
				cout << "(ERROR) 중복 \":=\" 발생\n\n"; break;
			case RIGHT_PAREN:
			default:
				cout << "(ERROR) 피연산자(ID,CONST) 부족\n\n"; break;
			}
			return result;
		}
	}
	//엡실론이므로 받은 값 그대로 반환
	return result;
}
id Term(const state* c, int* real_curr, id* ident, id result) {
	//Expression에서 몇개 바꿈
	int this_curr;
	this_curr = *real_curr;
	result = Factor(c, real_curr, ident, result);
	if (this_curr != *real_curr) {
		if (result.is_unknown == ERROR_PS)return result;//이미 오류 있었음
		return Factor_tail(c, real_curr, ident, result);
	}
	//길이 0 오류인데 statement에서 처리할 거임
	return result;//길이 0이기에 바뀐것 없음
}
id Factor_tail(const state* c, int* real_curr, id* ident, id result) {
	//Term_tail에서 몇개 바꿈
	int this_curr, new_curr, mult_op;
	id new_result;
	new_result.value = UNKNOWN;
	mult_op = UNKNOWN;
	this_curr = *real_curr;
	mult_op = Mult_operator(c, real_curr);
	if (this_curr != *real_curr) {
		//"*" or "/"
		new_curr = *real_curr;
		new_result = Factor(c, real_curr, ident, new_result);
		if (new_curr != *real_curr) {//이미 *real_curr; 다음값 보고 있으므로 그냥 리턴해도 됨
			if (new_result.is_unknown == ERROR_PS)return new_result;//이미 오류 있었고, 오류구문 출력함
			if (mult_op == MULT_OP) {
				if (new_result.is_unknown == NO_PROB) {
					result.value = result.value * new_result.value;
				}
				else {
					//선언 안된 변수 참조함
					result.is_unknown = UNKNOWN;
				}
			}
			else if (mult_op == DIV_OP) {
				if (new_result.is_unknown == NO_PROB) {
					result.value = result.value / new_result.value;
				}
				else {
					//선언 안된 변수 참조함
					result.is_unknown = UNKNOWN;
				}
			}
			else {
				//Mult_operator가 어셉되서 이거 되면 프로그램 오류
				//출력될 일 없음
				cout << "error from factor_tail not mult_op" << endl;
			}
			//계속 파싱
			return Factor_tail(c, real_curr, ident, result);
		}
		//Mult_operator여기서 밖에 없으므로 뒤가 어셉 안되면 오류
		//오류라면 값이 정의 되지 않음
		else {
			result.is_unknown = ERROR_PS;
			//ID,CONST,( 가능성 없음
			//+,-,*,/,),:=,;이면 오류
			switch (c[*real_curr].token_type) {
			case ADD_OP:
			case SUB_OP:
			case MULT_OP:
			case DIV_OP:
				cout << "(ERROR) 중복 연산자(" << c[*real_curr - 1].lexeme << " " << c[*real_curr].lexeme << ") 발생\n\n"; break;
			case ASSIGN_OP:
				cout << "(ERROR) 중복 \":=\" 발생\n\n"; break;
			case RIGHT_PAREN:
			default:
				cout << "(ERROR) 피연산자(ID,CONST) 부족\n\n"; break;
			}
			return result;
		}
	}
	//엡실론이므로 받은 값 그대로 반환
	return result;
}
id Factor(const state* c, int* real_curr, id* ident, id result) {
	int this_curr, new_curr, index;
	this_curr = *real_curr;
	Left_paren(c, real_curr);
	if (this_curr != *real_curr) {
		//"("맞음
		new_curr = *real_curr;
		result = Expression(c, real_curr, ident, result);
		if (new_curr != *real_curr) {
			if (result.is_unknown == ERROR_PS)return result;//이미 오류 있었음
			//expression 제대로 동작함
			new_curr = *real_curr;
			Right_paren(c, real_curr);
			if (new_curr != *real_curr) {
				//<left_paren><expression><right_paren> 어셉
				return result;
			}
			//")"아님, "("있었기에 오류
			//오류라면 값이 정의 되지 않음
			else {
				result.is_unknown = ERROR_PS;
				cout << "(ERROR) RIGHT_PAREN\")\" 부족\n\n";
				return result;
			}
		}
		//<expression>아님, "("있었기에 오류
		//정확히는 "(" 다음 파싱되지 않음,
		else {
			//result.is_unknown = 0;
			//cout << "(ERROR) 피연산자(ID,CONST) 부족" << endl;
			//이미 <experssion>에서 처리
			return result;
		}
	}
	//"("아님, <ident>,<const> 실행해보기

	index = Ident(c, real_curr, ident);//ident의 index
	if (this_curr != *real_curr) {//이미 real_curr 다음것 보고 있음
		if (ident[index].is_unknown == NO_PROB) {
			//선언된 변수 참조
			result.value = ident[index].value;
			result.is_unknown = NO_PROB;
		}
		else {
			//cout << "선언되지 않은 변수 " << ident[index].lexeme << " 참조" << endl;
			//의미 오류는 단순 ok 처리
		}
		return result;//<ident>
	}
	//이미*real_curr == this_curr
	result.value = Const(c, real_curr);
	if (this_curr != *real_curr) {//이미 real_curr 다음것 보고 있음
		result.is_unknown = NO_PROB;
		return result;//<const>
	}
	//오류임
	//cout << "error from factor 모든 구문문법 아님, 오류구문 만들어야함" << endl;
	//factor는 길이가 0이고, 오류는 위에 있는 함수에서 처리할 것임
	return result;
}
int Const(const state* c, int* real_curr) {
	//문자열 숫자 int로 바꿔서 반환
	if (c[*real_curr].token_type == CONST)
		return stoi(c[(*real_curr)++].lexeme);
	return UNKNOWN;//unknown값(99)들어가버림,차피 사용 안할거라 상관 없음
}
int Ident(const state* c, int* real_curr, id* ident) {
	//값을 받을 수도 있어서 ident의 위치 숫자 반환
	//호출 되는 곳: Statement, Factor
	if (c[*real_curr].token_type == IDENT) {
		for (int i = 0; i < id_counter; i++) {
			if (c[*real_curr].lexeme == ident[i].lexeme) {
				(*real_curr)++;
				return i;
			}
		}
		//정의되지 않은 ID값, 프로그램 오류 아니면 실행X
		cout << "ID인데 정의되지 않은 변수명임" << endl;
		return -1;
	}
	return -1;//차피 반환값 사용 안할거라 상관 없음
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