/*
 * tokenizer.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cassert>
#include <cstring>
#include <vector>
#include <utility>

#include "xy/include/tokenizer.hpp"
#include "xy/include/array.hpp"
#include "xy/include/io/line_highlight.hpp"

namespace xy {

    namespace {
        typedef enum : char {
            SCA_ERROR,
            SCA_BARRIER,
            SCA_OK
        } suffix_char_action;

        /// how the tokenizer should behave when it encounters a specific ASCII
        /// character as the next character in a name/identifier. A BARRIER
        /// signals the end of the name, an OK signals that the character should
        /// be appended to the name, and an ERROR represents an unexpected
        /// character.
        static const suffix_char_action NAME_CHAR[]{
            SCA_BARRIER, //  0    ^@    .
            SCA_ERROR, //  1    ^A    .
            SCA_ERROR, //  2    ^B    .
            SCA_ERROR, //  3    ^C    .
            SCA_ERROR, //  4    ^D    .
            SCA_ERROR, //  5    ^E    .
            SCA_ERROR, //  6    ^F    .
            SCA_ERROR, //  7    ^G    .
            SCA_ERROR, //  8    ^H    .
            SCA_BARRIER, //  9    ^I    .
            SCA_BARRIER, //  10    ^J    .
            SCA_ERROR, //  11    ^K    .
            SCA_ERROR, //  12    ^L    .
            SCA_BARRIER, //  13    ^M    .
            SCA_ERROR, //  14    ^N    .
            SCA_ERROR, //  15    ^O    .
            SCA_ERROR, //  16    ^P    .
            SCA_ERROR, //  17    ^Q    .
            SCA_ERROR, //  18    ^R    .
            SCA_ERROR, //  19    ^S    .
            SCA_ERROR, //  20    ^T    .
            SCA_ERROR, //  21    ^U    .
            SCA_ERROR, //  22    ^V    .
            SCA_ERROR, //  23    ^W    .
            SCA_ERROR, //  24    ^X    .
            SCA_ERROR, //  25    ^Y    .
            SCA_ERROR, //  26    ^Z    .
            SCA_ERROR, //  27    ^[    .
            SCA_ERROR, //  28    ^\    .
            SCA_ERROR, //  29    ^]    .
            SCA_ERROR, //  30    ^^    .
            SCA_ERROR, //  31    ^_    .
            SCA_BARRIER, //  32         .
            SCA_BARRIER, //  33    !    .
            SCA_BARRIER, //  34    "    .
            SCA_BARRIER, //  35    #    .
            SCA_OK, //  36    $    .
            SCA_BARRIER, //  37    %    .
            SCA_BARRIER, //  38    &    .
            SCA_OK, //  39    '    .
            SCA_BARRIER, //  40    (    .
            SCA_BARRIER, //  41    )    .
            SCA_BARRIER, //  42    *    .
            SCA_BARRIER, //  43    +    .
            SCA_BARRIER, //  44    ,    .
            SCA_BARRIER, //  45    -    .
            SCA_BARRIER, //  46    .    .
            SCA_BARRIER, //  47    /    .
            SCA_OK, //  48    0    .
            SCA_OK, //  49    1    .
            SCA_OK, //  50    2    .
            SCA_OK, //  51    3    .
            SCA_OK, //  52    4    .
            SCA_OK, //  53    5    .
            SCA_OK, //  54    6    .
            SCA_OK, //  55    7    .
            SCA_OK, //  56    8    .
            SCA_OK, //  57    9    .
            SCA_BARRIER, //  58    :    .
            SCA_BARRIER, //  59    ;    .
            SCA_BARRIER, //  60    <    .
            SCA_BARRIER, //  61    =    .
            SCA_BARRIER, //  62    >    .
            SCA_BARRIER, //  63    ?    .
            SCA_BARRIER, //  64    @    .
            SCA_OK, //  65    A    .
            SCA_OK, //  66    B    .
            SCA_OK, //  67    C    .
            SCA_OK, //  68    D    .
            SCA_OK, //  69    E    .
            SCA_OK, //  70    F    .
            SCA_OK, //  71    G    .
            SCA_OK, //  72    H    .
            SCA_OK, //  73    I    .
            SCA_OK, //  74    J    .
            SCA_OK, //  75    K    .
            SCA_OK, //  76    L    .
            SCA_OK, //  77    M    .
            SCA_OK, //  78    N    .
            SCA_OK, //  79    O    .
            SCA_OK, //  80    P    .
            SCA_OK, //  81    Q    .
            SCA_OK, //  82    R    .
            SCA_OK, //  83    S    .
            SCA_OK, //  84    T    .
            SCA_OK, //  85    U    .
            SCA_OK, //  86    V    .
            SCA_OK, //  87    W    .
            SCA_OK, //  88    X    .
            SCA_OK, //  89    Y    .
            SCA_OK, //  90    Z    .
            SCA_BARRIER, //  91    [    .
            SCA_BARRIER, //  92    \    .
            SCA_BARRIER, //  93    ]    .
            SCA_BARRIER, //  94    ^    .
            SCA_OK, //  95    _    .
            SCA_BARRIER, //  96    `    .
            SCA_OK, //  97    a    .
            SCA_OK, //  98    b    .
            SCA_OK, //  99    c    .
            SCA_OK, //  100    d    .
            SCA_OK, //  101    e    .
            SCA_OK, //  102    f    .
            SCA_OK, //  103    g    .
            SCA_OK, //  104    h    .
            SCA_OK, //  105    i    .
            SCA_OK, //  106    j    .
            SCA_OK, //  107    k    .
            SCA_OK, //  108    l    .
            SCA_OK, //  109    m    .
            SCA_OK, //  110    n    .
            SCA_OK, //  111    o    .
            SCA_OK, //  112    p    .
            SCA_OK, //  113    q    .
            SCA_OK, //  114    r    .
            SCA_OK, //  115    s    .
            SCA_OK, //  116    t    .
            SCA_OK, //  117    u    .
            SCA_OK, //  118    v    .
            SCA_OK, //  119    w    .
            SCA_OK, //  120    x    .
            SCA_OK, //  121    y    .
            SCA_OK, //  122    z    .
            SCA_BARRIER, //  123    {    .
            SCA_BARRIER, //  124    |    .
            SCA_BARRIER, //  125    }    .
            SCA_BARRIER, //  126    ~    .
            SCA_ERROR, //  127    ^?    .
        };

        /// tokens that can be matched by a single ASCII character
        static const token_type SINGLE_CHAR_TOKENS[]{
            T_INVALID, //  0     ^@
            T_INVALID, //  1     ^A
            T_INVALID, //  2     ^B
            T_INVALID, //  3     ^C
            T_INVALID, //  4     ^D
            T_INVALID, //  5     ^E
            T_INVALID, //  6     ^F
            T_INVALID, //  7     ^G
            T_INVALID, //  8     ^H
            T_INVALID, //  9     ^I
            T_INVALID, //  10     ^J // T_NEW_LINE
            T_INVALID, //  11     ^K
            T_INVALID, //  12     ^L
            T_INVALID, //  13     ^M
            T_INVALID, //  14     ^N
            T_INVALID, //  15     ^O
            T_INVALID, //  16     ^P
            T_INVALID, //  17     ^Q
            T_INVALID, //  18     ^R
            T_INVALID, //  19     ^S
            T_INVALID, //  20     ^T
            T_INVALID, //  21     ^U
            T_INVALID, //  22     ^V
            T_INVALID, //  23     ^W
            T_INVALID, //  24     ^X
            T_INVALID, //  25     ^Y
            T_INVALID, //  26     ^Z
            T_INVALID, //  27     ^[
            T_INVALID, //  28     ^\ .
            T_INVALID, //  29     ^]
            T_INVALID, //  30     ^^
            T_INVALID, //  31     ^_
            T_INVALID, //  32
            T_INVALID, //  33     !
            T_INVALID, //  34     "
            T_INVALID, //  35     #
            T_INVALID, //  36     $
            T_PERCENT, //  37     %
            T_AMPERSAND, //  38     &
            T_INVALID, //  39     '
            T_OPEN_PAREN, //  40     (
            T_CLOSE_PAREN, //  41     )
            T_ASTERISK, //  42     *
            T_PLUS, //  43     +
            T_COMMA, //  44     ,
            T_INVALID, //  45     -
            T_PERIOD, //  46     .
            T_FORWARD_SLASH, //  47     /
            T_INVALID, //  48     0
            T_INVALID, //  49     1
            T_INVALID, //  50     2
            T_INVALID, //  51     3
            T_INVALID, //  52     4
            T_INVALID, //  53     5
            T_INVALID, //  54     6
            T_INVALID, //  55     7
            T_INVALID, //  56     8
            T_INVALID, //  57     9
            T_INVALID, //  58     :
            T_SEMICOLON, //  59     ;
            T_LESS_THAN, //  60     <
            T_INVALID, //  61     =
            T_GREATER_THAN, //  62     >
            T_INVALID, //  63     ?
            T_AT, //  64     @
            T_INVALID, //  65     A
            T_INVALID, //  66     B
            T_INVALID, //  67     C
            T_INVALID, //  68     D
            T_INVALID, //  69     E
            T_INVALID, //  70     F
            T_INVALID, //  71     G
            T_INVALID, //  72     H
            T_INVALID, //  73     I
            T_INVALID, //  74     J
            T_INVALID, //  75     K
            T_INVALID, //  76     L
            T_INVALID, //  77     M
            T_INVALID, //  78     N
            T_INVALID, //  79     O
            T_INVALID, //  80     P
            T_INVALID, //  81     Q
            T_INVALID, //  82     R
            T_INVALID, //  83     S
            T_INVALID, //  84     T
            T_INVALID, //  85     U
            T_INVALID, //  86     V
            T_INVALID, //  87     W
            T_INVALID, //  88     X
            T_INVALID, //  89     Y
            T_INVALID, //  90     Z
            T_OPEN_BRACKET, //  91     [
            T_BACKWARD_SLASH, //  92     \ .
            T_CLOSE_BRACKET, //  93     ]
            T_HAT, //  94     ^
            T_INVALID, //  95     _
            T_INVALID, //  96     `
            T_INVALID, //  97     a
            T_INVALID, //  98     b
            T_INVALID, //  99     c
            T_INVALID, //  100     d
            T_INVALID, //  101     e
            T_INVALID, //  102     f
            T_INVALID, //  103     g
            T_INVALID, //  104     h
            T_INVALID, //  105     i
            T_INVALID, //  106     j
            T_INVALID, //  107     k
            T_INVALID, //  108     l
            T_INVALID, //  109     m
            T_INVALID, //  110     n
            T_INVALID, //  111     o
            T_INVALID, //  112     p
            T_INVALID, //  113     q
            T_INVALID, //  114     r
            T_INVALID, //  115     s
            T_INVALID, //  116     t
            T_INVALID, //  117     u
            T_INVALID, //  118     v
            T_INVALID, //  119     w
            T_INVALID, //  120     x
            T_INVALID, //  121     y
            T_INVALID, //  122     z
            T_OPEN_BRACE, //  123     {
            T_PIPE, //  124     |
            T_CLOSE_BRACE, //  125     }
            T_TILDE, //  126     ~
            T_INVALID, //  127     ^?
        };

        /// memcpy/strcpy equivalent that returns the number of characters
        /// written. This does not write a null character to the end of
        /// the buffer
        static size_t write_to_buff(char *buffer, const char *str) throw() {
            char *bb(buffer);
            for(; '\0' != *str; ) {
                *bb++ = *str++;
            }
            return static_cast<size_t>(bb - buffer);
        }

        /// is a character a decimal digit?
        static bool is_decimal(char c) throw() {
            return isdigit(c);
        }

        /// is a character an octal digit?
        static bool is_octal(char c) throw() {
            return '0' <= c && c <= '7';
        }

        /// is a character a hexadecimal digit?
        static bool is_hex(char c) throw() {
            const char cl(tolower(c));
            return isdigit(c) || ('a' <= cl && cl <= 'f');
        }

        /// a names to token type mapping of reserved keywords in the language
        typedef struct {
            const char * const str;
            const size_t len;
            const token_type type;
        } reserved_name;
        static reserved_name RESERVED_NAMES[]{
            /*{"take",        5U, T_TAKE},
            {"give",        5U, T_GIVE},
            {"shared",      7U, T_SHARED},
            {"share",       6U, T_SHARE},
            {"begin",       6U, T_BEGIN},
            {"end",         4U, T_END},
            {"into",        5U, T_INTO},
            {"own",         4U, T_OWN},
            {"let",         4U, T_LET},
            {"if",          3U, T_IF},
            {"then",        5U, T_THEN},
            {"else",        5U, T_ELSE},*/
            {"let",         4U, T_LET},
            {"import",      7U, T_IMPORT},
            {"return",      7U, T_RETURN},
            {"yield",       6U, T_YIELD},
            {"union",       6U, T_UNION},
            {"record",      7U, T_RECORD},
            {"function",    9U, T_FUNCTION},
        };
    }

    tokenizer::tokenizer(void) throw()
        : ll()
        , chr(0)
        , cp()
        , state(READ_NEXT_CODEPOINT)
    { }

    /// push the file, line, and column information as a message into the
    /// diagnostic context's message queue.
    void tokenizer::push_file_line_col(diagnostic_context &ctx) throw() {
        ctx.report(io::c_file_line_col,
            ctx.file(), ll.line(), ll.column()
        );
        state = DONE;
    }

    void tokenizer::push_file_line_col_point(diagnostic_context &ctx) throw() {
        push_file_line_col(ctx);
        ctx.report(io::c_highlight, io::highlight_column(
            ctx.file(), ll.line(), ll.column()
        ));
        state = DONE;
    }

    void tokenizer::push_file_line_col_under(diagnostic_context &ctx, uint32_t start_col) throw() {
        push_file_line_col(ctx);
        ctx.report(io::c_highlight, io::highlight_line(
            ctx.file(), ll.line(), start_col, ll.column()
        ));
        state = DONE;
    }

    void tokenizer::push_file_line_col_left(diagnostic_context &ctx, uint32_t start_col) throw() {
        push_file_line_col(ctx);
        ctx.report(io::c_highlight, io::highlight_left(
            ctx.file(), ll.line(), start_col, ll.column()
        ));
        state = DONE;
    }

    /// interpret a byte as a digit in an octal number
    bool tokenizer::get_octal_digit(diagnostic_context &ctx, char *chr, uint32_t start_col) throw() {

        if(!cp.is_ascii()) {
            ctx.report(io::e_invalid_octal_escape, cp.to_cstring());
            push_file_line_col_left(ctx, start_col);
            return false;
        }

        char digit = cp.to_cstring()[0];
        if(digit < '0' || '7' < digit) {
            ctx.report(io::e_invalid_octal_escape, cp.to_cstring());
            push_file_line_col_left(ctx, start_col);
            return false;
        }

        int16_t new_val = (static_cast<int16_t>(*chr) * 8) + (digit - '0');

        if(new_val > 255) { // wrap-around
            ctx.report(io::e_octal_escape_too_big, new_val, new_val);
            push_file_line_col_under(ctx, start_col);
            return false;
        }
        *chr = static_cast<char>(new_val);
        return true;
    }

    /// interpret a byte as a digit in a hexadecimal number.
    bool tokenizer::get_hex_digit(diagnostic_context &ctx, char *chr, uint32_t start_col) throw() {
        if(!cp.is_ascii()) {
            ctx.report(io::e_invalid_hex_escape, cp.to_cstring());
            push_file_line_col_left(ctx, start_col);
            return false;
        }

        char digit = tolower(cp.to_cstring()[0]);
        if(!isalnum(digit) || 'f' < digit) {
            ctx.report(io::e_invalid_hex_escape, cp.to_cstring());
            push_file_line_col_left(ctx, start_col);
            return false;
        } else if('a' <= digit) {
            digit -= 'a' - ':';
        }
        const int16_t new_val = (static_cast<int16_t>(*chr) * 16) + (digit - '0');
        *chr = static_cast<char>(new_val);
        return true;
    }

    /// get the next token. If a token is found, this function returns true,
    /// and the token argument is updated to contain relevant information. If
    /// the token is one in which the lexed value is meaningful, e.g. an
    /// identifier name, then that value is stored in the scratch buffer.
    bool tokenizer::get_token(
        support::byte_reader &f,
        diagnostic_context &ctx,
        token &tok,
        char (&scratch)[MAX_TOKEN_LENGTH]
    ) throw() {
        token_type tt(T_INVALID);
        size_t i(0);
        bool (*digit_p)(char) = is_decimal;
        int last_chr('\0');
        uint32_t last_line(0);
        uint32_t last_col(0);

        scratch[0] = '\0';
        tok.type_ = T_INVALID;
        tok.num_columns_ = 0;

        switch(state) {
        case READ_NEXT_CODEPOINT:
            for(; ll.get_codepoint(f, ctx, cp); ) {

        case HAVE_NEXT_CODEPOINT:
                if(cp.is_null()) {
                    state = DONE;
                    break;
                }

                if(!cp.is_ascii()) {
        case HAVE_NON_ASCII_CODEPOINT:
                    ctx.report(io::e_mb_not_in_string, cp.to_cstring());
                    push_file_line_col_point(ctx);
                    return false;
                }

                chr = cp.to_cstring()[0];

        case HAVE_ASCII_CODEPOINT:

                tt = SINGLE_CHAR_TOKENS[chr];

                // single character token
                if(T_INVALID != tt) {
                    tok.col_ = ll.column();
                    tok.line_ = ll.line();
                    tok.type_ = tt;
                    tok.num_columns_ = 1;

                    state = READ_NEXT_CODEPOINT;
                    return true;
                }

                // equals (=) or double arrow (=>)
                if('=' == chr) {

                    tok.col_ = ll.column();
                    tok.line_ = ll.line();
                    tok.type_ = T_EQUAL;
                    tok.num_columns_ = 1;

                    if(!ll.get_codepoint(f, ctx, cp) || cp.is_null()) {
                        state = DONE;
                    } else if(!cp.is_ascii()) {
                        state = HAVE_NON_ASCII_CODEPOINT;
                    } else if('>' == cp.to_cstring()[0]) {
                        tok.num_columns_ = 2;
                        tok.type_ = T_DOUBLE_ARROW;
                    } else {
                        state = HAVE_ASCII_CODEPOINT;
                    }
                    return true;

                // colon-colon (::), assign (:=)
                } else if(':' == chr) {
                    tok.col_ = ll.column();
                    tok.line_ = ll.line();
                    tok.type_ = T_INVALID;
                    tok.num_columns_ = 1;

                    if(!ll.get_codepoint(f, ctx, cp) || cp.is_null()) {
                        state = DONE;
                        return false;
                    } else if(!cp.is_ascii()) {
                        state = HAVE_NON_ASCII_CODEPOINT;
                        return false;
                    }

                    if('=' == cp.to_cstring()[0]) {
                        state = READ_NEXT_CODEPOINT;
                        tok.type_ = T_ASSIGN;
                        ++tok.num_columns_;
                        return true;
                    } else if(':' == cp.to_cstring()[0]) {
                        state = READ_NEXT_CODEPOINT;
                        tok.type_ = T_DECLARE;
                        ++tok.num_columns_;
                        return true;
                    } else {
                        state = HAVE_ASCII_CODEPOINT;
                        return false;
                    }

                // minus/dash, arrow, comment, or block comment
                } else if('-' == chr) {
                    tok.col_ = ll.column();
                    tok.line_ = ll.line();
                    tok.type_ = T_MINUS;
                    tok.num_columns_ = 1;

                    if(!ll.get_codepoint(f, ctx, cp) || cp.is_null()) {
                        state = DONE;
                        return true;
                    } else if(!cp.is_ascii()) {
                        state = HAVE_NON_ASCII_CODEPOINT;
                        return true;
                    }

                    chr = cp.to_cstring()[0];

                    if('>' == chr) {
                        state = READ_NEXT_CODEPOINT;
                        tok.type_ = T_ARROW;
                        ++(tok.num_columns_);

                    // single-line comment
                    } else if('-' == chr) {
                        //tok.type_ = T_NEW_LINE;
                        //tok.num_columns_ = 0;
                        tok.type_ = T_INVALID;
                        state = READ_NEXT_CODEPOINT;

                        for(; ll.get_codepoint(f, ctx, cp); ) {
                            if(cp.is_null()) {
                                tok.line_ = ll.line();
                                tok.col_ = ll.column();
                                state = DONE;
                                break;
                            } else if(cp.is_ascii()) {
                                chr = cp.to_cstring()[0];
                                if('\n' == chr) {
                                    tok.line_ = ll.line();
                                    tok.col_ = ll.column();
                                    break;
                                } else if('\r' == chr) {
                                    tok.line_ = ll.line();
                                    tok.col_ = ll.column();

                                    if(!ll.get_codepoint(f, ctx, cp) || cp.is_null()) {
                                        break;
                                    }

                                    if(!cp.is_ascii()) {
                                        state = HAVE_NON_ASCII_CODEPOINT;
                                        break;
                                    }

                                    chr = cp.to_cstring()[0];
                                    if('\n' != chr) {
                                        state = HAVE_ASCII_CODEPOINT;
                                        break;
                                    }

                                    tok.line_ = ll.line();
                                    tok.col_ = ll.column();
                                    break;
                                }
                            }
                        }

                    // block comment
                    } else if('*' == chr) {
                        tok.type_ = T_INVALID;
                        tok.num_columns_ = 0;

                        std::vector<std::pair<uint32_t, uint32_t> > positions;
                        int num_to_close(1);

                        positions.push_back(std::make_pair(ll.line(), ll.column() - 1));

                        for(; ll.get_codepoint(f, ctx, cp); ) {
                            if(cp.is_null()) {
                                break;
                            }

                            if(cp.is_ascii()) {
                                chr = cp.to_cstring()[0];
                                //if('\r' == chr || '\n' == chr) {
                                //    tok.type_ = T_NEW_LINE;
                                //}
                                if('*' != chr && '-' != chr) {
                                    continue;
                                }

                            try_next_char_in_comment:

                                if(!ll.get_codepoint(f, ctx, cp)
                                || cp.is_null()) {
                                    break;
                                } else if(!cp.is_ascii()) {
                                    continue;
                                }

                                last_chr = chr;
                                chr = cp.to_cstring()[0];

                                if('*' == last_chr && '-' == chr) {
                                    positions.pop_back();
                                    --num_to_close;
                                    if(0 == num_to_close) {
                                        tok.line_ = ll.line();
                                        tok.col_ = ll.column();
                                        break;
                                    }
                                    continue;
                                } else if('-' == last_chr && '*' == chr) {
                                    positions.push_back(std::make_pair(ll.line(), ll.column() - 1));
                                    ++num_to_close;
                                    continue;
                                }

                                if('*' == chr || '-' == chr) {
                                    goto try_next_char_in_comment;
                                }
                            }
                        }

                        if(0 == num_to_close) {

                            // we didn't find any new lines in the block comment,
                            // so don't report it as a new line token
                            if(T_INVALID == tok.type_) {
                                state = READ_NEXT_CODEPOINT;
                                continue;
                            }

                            return true;
                        }

                        ctx.report(io::e_unclosed_block_comment, num_to_close);
                        push_file_line_col(ctx);

                        // report on all the unclosed block comments
                        for(; !positions.empty(); positions.pop_back()) {
                            ctx.report(io::n_start_of_block_comment);
                            ctx.report(io::c_file_line_col,
                                ctx.file(), positions.back().first, positions.back().second
                            );
                            ctx.report(io::c_highlight, io::highlight_column(
                                ctx.file(), positions.back().first, positions.back().second
                            ));
                        }

                        return false;

                    } else {
                        state = HAVE_ASCII_CODEPOINT;
                    }
                    return true;

                // \r, \r\n
                } else if('\r' == chr) {
                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    //tok.type_ = T_NEW_LINE;
                    tok.type_ = T_INVALID;
                    tok.num_columns_ = 0;
                    state = READ_NEXT_CODEPOINT;

                    if(!ll.get_codepoint(f, ctx, cp)) {
                        state = DONE;
                    } else {
                        chr = cp.to_cstring()[0];

                        if(cp.is_ascii()) {
                            if('\n' != chr) {
                                state = HAVE_ASCII_CODEPOINT;
                            }
                        } else {
                            state = HAVE_NON_ASCII_CODEPOINT;
                        }
                    }

                    //return true;

                // string literal
                } else if('"' == chr) {

                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_STRING_LITERAL;
                    state = READ_NEXT_CODEPOINT;
                    tok.num_columns_ = 1;

                    i = 0;
                    for(; ll.get_codepoint(f, ctx, cp); ) {

                        if(i >= BUFFER_LENGTH) {
                            ctx.report(io::e_string_too_long, static_cast<unsigned>(BUFFER_LENGTH));
                            ctx.report(io::c_file_line_col,
                                ctx.file(), tok.line_, tok.col_
                            );
                            ctx.report(io::c_highlight, io::highlight_column(
                                ctx.file(), tok.line_, tok.col_
                            ));
                            state = DONE;
                            scratch[0] = '\0';
                            return false;

                        } else if(cp.is_null()) {
                            goto early_termination_of_string;
                        }

                        ++(tok.num_columns_);

                        if(cp.is_ascii()) {
                            chr = cp.to_cstring()[0];

                            // end of the string
                            if('"' == chr) {
                                scratch[i] = '\0';
                                goto found_end_of_string;

                            // escape sequence
                            } else if('\\' == chr) {
                                const uint32_t esc_start(ll.column());
                                if(!ll.get_codepoint(f, ctx, cp)) {
                                    goto early_termination_of_string;
                                } else if(!cp.is_ascii()) {
                                    ctx.report(io::e_mb_escape_string,
                                        cp.to_cstring()
                                    );
                                    push_file_line_col_point(ctx);
                                    scratch[0] = '\0';
                                    return false;
                                }

                                scratch[i] = '\0';

                                switch(cp.to_cstring()[0]) {

                                // basic escape characters
                                case 'a': scratch[i] = '\x07'; break;
                                case 'b': scratch[i] = '\x08'; break;
                                case 'f': scratch[i] = '\x0C'; break;
                                case 'n': scratch[i] = '\n'; break;
                                case 'r': scratch[i] = '\r'; break;
                                case 't': scratch[i] = '\t'; break;
                                case 'v': scratch[i] = '\x0B'; break;
                                case '"': scratch[i] = '"'; break;
                                case '\\': scratch[i] = '\\'; break;

                                // octal escape sequence, e.g. \oOOO
                                case 'o':
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    if(!get_octal_digit(ctx, &(scratch[i]), esc_start)) {
                                        state = DONE;
                                        scratch[0] = '\0';
                                        return false;
                                    }
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    if(!get_octal_digit(ctx, &(scratch[i]), esc_start)) {
                                        state = DONE;
                                        scratch[0] = '\0';
                                        return false;
                                    }
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    if(!get_octal_digit(ctx, &(scratch[i]), esc_start)) {
                                        state = DONE;
                                        scratch[0] = '\0';
                                        return false;
                                    }
                                    i += 2;
                                    break;

                                // hexadecimal escape sequence, e.g. \xXX
                                case 'x':
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    if(!get_hex_digit(ctx, &(scratch[i]), esc_start)) {
                                        state = DONE;
                                        scratch[0] = '\0';
                                        return false;
                                    }
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    if(!get_hex_digit(ctx, &(scratch[i]), esc_start)) {
                                        state = DONE;
                                        scratch[0] = '\0';
                                        return false;
                                    }
                                    i += 1;
                                    break;

                                // unknown escape character
                                default:
                                    ctx.report(io::e_invalid_escape,
                                        cp.to_cstring()[0]
                                    );
                                    push_file_line_col_left(ctx, esc_start);
                                    scratch[0] = '\0';
                                    return false;
                                }

                            // normal ascii in string
                            } else {
                                scratch[i] = chr;
                            }

                            i += 1;

                        // multibyte character in string
                        } else {
                            i += write_to_buff(&(scratch[i]), cp.to_cstring());
                        }
                    }

                early_termination_of_string:
                    ctx.report(io::e_string_not_terminated);
                    ctx.report(io::c_file_line_col,
                        ctx.file(), tok.line_, tok.col_
                    );
                    ctx.report(io::c_highlight, io::highlight_column(
                        ctx.file(), tok.line_, tok.col_
                    ));
                    state = DONE;
                    scratch[0] = '\0';
                    return false;

                found_end_of_string:
                    return true;
                }

                // skip non-newline whitespace
                if(isspace(chr)) {
                    continue;

                // integer or float
                } else if(isdigit(chr)) {
                    i = 1;
                    state = READ_NEXT_CODEPOINT;

                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_INTEGER_LITERAL;
                    tok.num_columns_ = 1;
                    scratch[0] = chr;

                    for(; ll.get_codepoint(f, ctx, cp); ++i) {
                        if(cp.is_null()) {
                            break;
                        }
                        if(!cp.is_ascii()) {
                            state = HAVE_NON_ASCII_CODEPOINT;
                            break;
                        }
                        chr = cp.to_cstring()[0];
                        scratch[i] = chr;

                        if(!digit_p(chr)) {

                            // decimal point, make sure there is only one.
                            if('.' == chr) {

                                if(T_INTEGER_LITERAL == tok.type_) {

                                    // found decimal point in non-decimal formatted
                                    // int, e.g. 0xAB.C, which we won't accept
                                    if(is_decimal != digit_p) {
                                        state = HAVE_ASCII_CODEPOINT;
                                        break;
                                    }
                                    tok.type_ = T_RATIONAL_LITERAL;

                                // this is the 2nd decimal point we've seen,
                                // see it as a period for the next token round
                                } else {
                                    state = HAVE_ASCII_CODEPOINT;
                                    break;
                                }

                            // hexadecimal integer, 0x
                            } else if('x' == chr && 1 == i && '0' == scratch[0]) {
                                digit_p = is_hex;

                            // octal integer, 0o
                            } else if('o' == chr && 1 == i && '0' == scratch[0]) {
                                digit_p = is_octal;

                            // undo seeing the token as a floating point, and
                            // instead shoot out two tokens.
                            } else if('.' == last_chr) {
                                tok.type_ = T_INTEGER_LITERAL;
                                scratch[i - 1] = '\0';
                                scratch[i] = '\0';
                                state = HAVE_PERIOD_AFTER_INT;
                                return true;

        case HAVE_PERIOD_AFTER_INT:

                                tok.type_ = T_PERIOD;
                                tok.line_ = last_line;
                                tok.col_ = last_col;
                                scratch[0] = '\0';
                                state = HAVE_ASCII_CODEPOINT;
                                return true;

                            } else {
                                state = HAVE_ASCII_CODEPOINT;
                                break;
                            }
                        }

                        last_chr = chr;
                        last_line = ll.line();
                        last_col = ll.column();
                        ++(tok.num_columns_);
                    }

                    scratch[i] = '\0';
                    return true;

                // name / type name
                // invariant: chr is not a digit
                } else if(SCA_OK == NAME_CHAR[chr]) {

                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_NAME;
                    tok.num_columns_ = 1;

                    // type name, starts with upper case
                    if(tolower(chr) != chr) {
                        tok.type_ = T_TYPE_NAME;
                    }

                    state = READ_NEXT_CODEPOINT;
                    scratch[0] = static_cast<char>(chr);
                    i = 1;

                    for(; ll.get_codepoint(f, ctx, cp); ++i) {
                        if(cp.is_null()) {
                            state = DONE;
                            break;
                        } else if(!cp.is_ascii()) {
                            state = HAVE_NON_ASCII_CODEPOINT;
                            break;
                        }

                        if(i >= NAME_LENGTH) {
                            scratch[i] = '\0';
                            ctx.report(io::e_name_too_long,
                                scratch, static_cast<unsigned>(NAME_LENGTH)
                            );
                            ctx.report(io::c_file_line_col,
                                ctx.file(), tok.line_, tok.col_
                            );
                            ctx.report(io::c_highlight, io::highlight_column(
                                ctx.file(), tok.line_, tok.col_
                            ));
                            state = DONE;
                            scratch[0] = '\0';
                            return false;
                        }

                        chr = cp.to_cstring()[0];

                        // error, character not allowed in a name
                        if(SCA_ERROR == NAME_CHAR[chr]) {
                            ctx.report(io::e_bad_char_in_name, chr);
                            push_file_line_col_point(ctx);
                            scratch[0] = '\0';
                            return false;

                        // token barrier
                        } else if(SCA_BARRIER == NAME_CHAR[chr]) {
                            state = HAVE_ASCII_CODEPOINT;
                            break;

                        // collect another char
                        } else {
                            scratch[i] = static_cast<char>(chr);
                            ++(tok.num_columns_);
                        }
                    }
                    scratch[i] = '\0';

                    if(T_NAME == tok.type_) {
                        for(i = 0; i < array::length(RESERVED_NAMES); ++i) {
                            if(0 == strncmp(scratch, RESERVED_NAMES[i].str, RESERVED_NAMES[i].len)) {
                                tok.type_ = RESERVED_NAMES[i].type;
                                break;
                            }
                        }
                    }

                    return true;

                }
            }
        case DONE:
            tok.type_ = T_EOF;
            break;
        }

        return false;
    }
}

