#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "tokens.h"
#include "Parser.h"
#include "lexer.h"

extern int id_counter;

extern int* isunknown;
extern int  isunknown_n;

// 각 구간 ID/CONST/OP 개수
static void dump_counts(const std::vector<state>& toks, std::size_t L, std::size_t R) {
    int idc = 0, cnst = 0, op = 0;
    for (std::size_t i = L; i < R; ++i) {
        switch (toks[i].token_type) {
            case IDENT: ++idc; break;
            case CONST: ++cnst; break;
            //case ASSIGN_OP:
            case ADD_OP:
            case SUB_OP:
            case MULT_OP:
            case DIV_OP: ++op; break;
            default: break;
        }
    }
    std::cout << "ID: " << idc << "; CONST: " << cnst << "; OP: " << op << ";\n";
}

// 토큰들을 그대로 이어붙이고 ; 붙이기
static void echo_stmt(const std::vector<state>& toks, std::size_t L, std::size_t R) {
    for (std::size_t i = L; i < R; ++i) std::cout << toks[i].lexeme;
    std::cout << ";\n";
}

// 중복 체크
static bool seen_name(const std::vector<id>& table, const std::string& name) {
    for (const auto& row : table) if (row.lexeme == name) return true;
    return false;
}

// id 테이블 구성
static std::vector<id> build_id_table(const std::vector<state>& toks,
                                      const std::vector<std::pair<std::size_t,std::size_t>>& stmts) {
    std::vector<id> tbl;
    for (const auto& seg : stmts) {
        const std::size_t L = seg.first, R = seg.second;
        for (std::size_t i = L; i < R; ++i) {
            if (toks[i].token_type == IDENT) {
                const std::string& name = toks[i].lexeme;
                if (!seen_name(tbl, name)) {
                    id row; row.lexeme = name; row.value = UNKNOWN;
                    tbl.push_back(row);
                }
            }
        }
    }
    if (tbl.empty()) { id d; d.lexeme = "_"; d.value = UNKNOWN; tbl.push_back(d); } // 초기값 unknown
    return tbl;
}

// 최종 결과 출력
static void render_result(const std::vector<id>& tbl) {
    std::cout << "Result ==> ";
    for (std::size_t i = 0; i < tbl.size(); ++i) {
        if (i) std::cout << ' ';
        std::cout << tbl[i].lexeme << ": "
                  << (tbl[i].value == UNKNOWN ? std::string("Unknown")
                                              : std::to_string(tbl[i].value))
                  << ';';
    }
    std::cout << '\n';
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);

    // 입력 파일 열기
    if (argc < 2) {
        std::cerr << "Usage: " << (argc ? argv[0] : "prog") << " <inputfile>\n";
        return 1;
    }
    std::ifstream fin(argv[1]);
    if (!fin) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    // 렉싱
    const std::string src((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    const std::vector<state> toks = lex_all(src);
    const auto stmts = split_statements(toks);

    // id 테이블 구성
    std::vector<id> sym = build_id_table(toks, stmts);
    if (id_counter < static_cast<int>(sym.size())) id_counter = static_cast<int>(sym.size());

    // isunknown
    isunknown_n = id_counter;
    isunknown   = new int[isunknown_n];
    for (int i = 0; i < isunknown_n; ++i) isunknown[i] = 0;   // 처음엔 모두 Unknown

    // 문장별 파싱 / 일단 ok 출력
    for (const auto& seg : stmts) {
        const std::size_t L = seg.first, R = seg.second;
        echo_stmt(toks, L, R);
        dump_counts(toks, L, R);
        int cur = static_cast<int>(L);
        Statement(toks.data(), &cur, sym.data());
        std::cout << "(OK)\n\n";
    }

    // 결과 출력
    render_result(sym);
    return 0;
}
