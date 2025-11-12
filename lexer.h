#pragma once
#include <string>
#include <vector>
#include "tokens.h"
#include "Parser.h"   // state, id 구조체를 여기서 가져다 씀

// 입력 전체를 토큰화
std::vector<state> lex_all(const std::string& text);

// 세미콜론 기준으로 문장 경계 [begin,end) 리스트
std::vector<std::pair<size_t,size_t>>
split_statements(const std::vector<state>& toks);
