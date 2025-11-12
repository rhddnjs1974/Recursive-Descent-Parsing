// lexer.cpp
#include "lexer.h"
#include <cctype>

// 식별자 시작 / 이어지는 문자 체크
static bool is_ident_start(char c) {
    return (std::isalpha((unsigned char)c) != 0) || (c == '_');
}
static bool is_ident_char(char c) {
    return (std::isalnum((unsigned char)c) != 0) || (c == '_');
}

// 입력 전체 문자열을 토큰으로
std::vector<state> lex_all(const std::string& text) {
    std::vector<state> tokens;

    // pos: 현재 위치
    std::size_t pos = 0;
    std::size_t len = text.size();

    while (pos < len) {
        // 현재 문자
        unsigned char ch = (unsigned char)text[pos];

        // 공백 넘기기
        if (std::isspace(ch)) {
            pos += 1;
            continue;
        }

        // 두 글자 ":="
        if (text[pos] == ':' && (pos + 1) < len && text[pos + 1] == '=') {
            state t;
            t.token_type = ASSIGN_OP;
            t.lexeme     = ":=";
            tokens.push_back(t);
            pos += 2;
            continue;
        }

        // 한 글자 연산자
        if (text[pos] == '+') {
            state t; t.token_type = ADD_OP; t.lexeme = "+";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == '-') {
            state t; t.token_type = SUB_OP; t.lexeme = "-";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == '*') {
            state t; t.token_type = MULT_OP; t.lexeme = "*";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == '/') {
            state t; t.token_type = DIV_OP; t.lexeme = "/";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == '(') {
            state t; t.token_type = LEFT_PAREN; t.lexeme = "(";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == ')') {
            state t; t.token_type = RIGHT_PAREN; t.lexeme = ")";
            tokens.push_back(t);
            pos += 1;
            continue;
        }
        if (text[pos] == ';') {
            state t; t.token_type = SEMI_COLON; t.lexeme = ";";
            tokens.push_back(t);
            pos += 1;
            continue;
        }

        // 숫자 토큰: 연속된 숫자
        if (std::isdigit(ch)) {
            std::size_t start = pos;
            std::size_t end   = pos + 1;

            while (end < len && std::isdigit((unsigned char)text[end])) {
                end += 1;
            }

            state t;
            t.token_type = CONST;
            t.lexeme     = text.substr(start, end - start);
            tokens.push_back(t);

            pos = end;
            continue;
        }

        // 식별자 토큰
        if (is_ident_start(text[pos])) {
            std::size_t start = pos;
            std::size_t end   = pos + 1;

            while (end < len && is_ident_char(text[end])) {
                end += 1;
            }

            state t;
            t.token_type = IDENT;
            t.lexeme     = text.substr(start, end - start);
            tokens.push_back(t);

            pos = end;
            continue;
        }

        // 그 외 한 글자 UNKNOWN
        {
            state t;
            t.token_type = UNKNOWN;
            t.lexeme     = std::string(1, text[pos]);
            tokens.push_back(t);
        }
        pos += 1;
    }

    return tokens;
}

// ; 기준으로 문장 구간 나누기
std::vector<std::pair<std::size_t, std::size_t>>
split_statements(const std::vector<state>& toks) {
    std::vector<std::pair<std::size_t, std::size_t>> segments;

    std::size_t begin = 0;
    std::size_t i = 0;
    std::size_t total = toks.size();

    // ; 만날 때마다 [begin, i] 구간 추가
    while (i < total) {
        if (toks[i].token_type == SEMI_COLON) {
            segments.push_back(std::make_pair(begin, i)); // ;는 문장에서 제외
            begin = i + 1;  // 다음 문장은 그 다음 토큰부터 시작
        }
        i += 1;
    }

    // 남은부분 문장으로 추가
    if (begin < total) {
        segments.push_back(std::make_pair(begin, total));
    }

    return segments;
}
