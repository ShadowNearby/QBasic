//
// Created by yanjs on 2022/11/9.
//

#pragma once


#include "../utility/QUtility.h"

class Token
{
public:
    enum class Kind
    {
        Number,
        Identifier,
        LeftParen,
        RightParen,
        LeftSquare,
        RightSquare,
        LeftCurly,
        RightCurly,
        LessThan,
        GreaterThan,
        Equal,
        Plus,
        Minus,
        Asterisk,
        Slash,
        Hash,
        Dot,
        Comma,
        Colon,
        Semicolon,
        SingleQuote,
        DoubleQuote,
        Comment,
        Pipe,
        End,
        Power,
        Question,
        Unexpected,

    };

    explicit Token(Kind kind) noexcept: m_kind{kind}
    {}

    Token(Kind kind, const char *beg, std::size_t len) noexcept
            : m_kind{kind}, m_lexeme(beg, len)
    {}

    Token(Kind kind, const char *beg, const char *end) noexcept
            : m_kind{kind}, m_lexeme(beg, std::distance(beg, end))
    {}

    [[nodiscard]] Kind kind() const noexcept
    { return m_kind; }

    void kind(Kind kind) noexcept
    { m_kind = kind; }

    [[nodiscard]] bool is(Kind kind) const noexcept
    { return m_kind == kind; }

    [[nodiscard]] bool is_not(Kind kind) const noexcept
    { return m_kind != kind; }

    [[nodiscard]] bool is_one_of(Kind k1, Kind k2) const noexcept
    { return is(k1) || is(k2); }

    template<typename... Ts>
    bool is_one_of(Kind k1, Kind k2, Ts... ks) const noexcept
    {
        return is(k1) || is_one_of(k2, ks...);
    }

    [[nodiscard]] std::string lexeme() const noexcept
    { return m_lexeme; }

    void lexeme(std::string lexeme) noexcept
    {
        m_lexeme = std::move(lexeme);
    }

private:
    Kind m_kind;
    std::string m_lexeme;
};

class Lexer
{
public:
    explicit Lexer(const char *beg) noexcept: m_beg(beg)
    {}

    Lexer();

    void setBeg(const char *beg);

    Token next() noexcept;

private:
    Token identifier() noexcept;

    Token power() noexcept;

    Token number() noexcept;

    Token slash_or_comment() noexcept;

    Token atom(Token::Kind) noexcept;

    [[nodiscard]] char peek() const noexcept
    { return *m_beg; }

    char get() noexcept
    { return *m_beg++; }

    const char *m_beg = nullptr;
};


void test();
