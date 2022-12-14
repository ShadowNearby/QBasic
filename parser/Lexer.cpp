//
// Created by yanjs on 2022/11/9.
//

#include "Lexer.h"

bool is_space(char c) noexcept
{
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool is_digit(char c) noexcept
{
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

bool is_identifier_char(char c) noexcept
{
    switch (c) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
            return true;
        default:
            return false;
    }
}

Token Lexer::atom(Token::Kind kind) noexcept
{ return {kind, m_beg++, 1}; }

Token Lexer::next() noexcept
{
    while (is_space(peek())) get();

    switch (peek()) {
        case '\0':
            return {Token::Kind::End, m_beg, 1};
        default:
            return atom(Token::Kind::Unexpected);
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return identifier();
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return number();
        case '?':
            return atom(Token::Kind::Question);
        case '(':
            return atom(Token::Kind::LeftParen);
        case ')':
            return atom(Token::Kind::RightParen);
        case '[':
            return atom(Token::Kind::LeftSquare);
        case ']':
            return atom(Token::Kind::RightSquare);
        case '{':
            return atom(Token::Kind::LeftCurly);
        case '}':
            return atom(Token::Kind::RightCurly);
        case '<':
            return atom(Token::Kind::LessThan);
        case '>':
            return atom(Token::Kind::GreaterThan);
        case '=':
            return atom(Token::Kind::Equal);
        case '+':
            return atom(Token::Kind::Plus);
        case '-':
            return atom(Token::Kind::Minus);
        case '*':
            return power();
        case '/':
            return slash_or_comment();
        case '#':
            return atom(Token::Kind::Hash);
        case '.':
            return atom(Token::Kind::Dot);
        case ',':
            return atom(Token::Kind::Comma);
        case ':':
            return atom(Token::Kind::Colon);
        case ';':
            return atom(Token::Kind::Semicolon);
        case '\'':
            return atom(Token::Kind::SingleQuote);
        case '"':
            return atom(Token::Kind::DoubleQuote);
        case '|':
            return atom(Token::Kind::Pipe);
    }
}

Token Lexer::identifier() noexcept
{
    const char *start = m_beg;
    get();
    while (is_identifier_char(peek())) get();
    return {Token::Kind::Identifier, start, m_beg};
}

Token Lexer::number() noexcept
{
    const char *start = m_beg;
    get();
    while (is_digit(peek())) get();
    return {Token::Kind::Number, start, m_beg};
}

Token Lexer::slash_or_comment() noexcept
{
    const char *start = m_beg;
    get();
    if (peek() == '/') {
        get();
        start = m_beg;
        while (peek() != '\0') {
            if (get() == '\n') {
                return {Token::Kind::Comment, start,
                        static_cast<size_t>(std::distance(start, m_beg) - 1)};
            }
        }
        return {Token::Kind::Unexpected, m_beg, 1};
    } else {
        return {Token::Kind::Slash, start, 1};
    }
}

Token Lexer::power() noexcept
{
    const char *start = m_beg;
    get();
    if (peek() == '*') {
        get();
        return {Token::Kind::Power, start, 2};
    } else return {Token::Kind::Asterisk, m_beg - 1, 1};
}

void Lexer::setBeg(const char *beg)
{
    m_beg = beg;
}

Lexer::Lexer()
= default;


std::ostream &operator<<(std::ostream &os, const Token::Kind &kind)
{
    static const char *const names[]{
            "Number", "Identifier", "LeftParen", "RightParen", "LeftSquare",
            "RightSquare", "LeftCurly", "RightCurly", "LessThan", "GreaterThan",
            "Equal", "Plus", "Minus", "Asterisk", "Slash",
            "Hash", "Dot", "Comma", "Colon", "Semicolon",
            "SingleQuote", "DoubleQuote", "Comment", "Pipe", "End",
            "Power", "Question", "Unexpected",
    };
    return os << names[static_cast<int>(kind)];
}

//void test()
//{
//    auto tiny = "100 REM Program to print the Fibonacci sequence.\n"
//                "110 LET max = 10000\n"
//                "120 LET n1 = 0\n"
//                "130 LET n2 = 1\n"
//                "140 IF n1 > max THEN 190\n"
//                "145 PRINT n1\n"
//                "150 LET n3 = n1 + n2\n"
//                "160 LET n1 = n2\n"
//                "170 LET n2 = n3\n"
//                "180 GOTO 140\n"
//                "190 END";
//    Lexer lex(tiny);
//    for (auto token = lexer.next();
//         !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
//         token = lexer.next()) {
//        std::cout << std::setw(12) << token.kind() << " |" << token.lexeme()
//                  << "|\n";
//    }
//}