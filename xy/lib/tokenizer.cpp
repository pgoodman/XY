/*
 * tokenizer.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cassert>

#include "xy/include/tokenizer.hpp"

namespace xy {

    namespace {
        typedef enum : char {
            SCA_ERROR,
            SCA_BARRIER,
            SCA_OK,
        } suffix_char_action;

        static suffix_char_action NAME_SUFFIX_CHAR[]{
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
            SCA_BARRIER, //  36    $    .
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

        size_t write_to_buff(char *buffer, const char *str) throw() {
            char *bb(buffer);
            for(; '\0' != *str; ) {
                *bb++ = *str++;
            }
            return static_cast<size_t>(bb - buffer);
        }
    }

    tokenizer::tokenizer(void) throw()
        : ll()
        , chr(0)
        , cp()
        , state(READ_NEXT_CODEPOINT)
        , scratch{'\0'}
    { }

    void tokenizer::push_line_file_col(diagnostic_context &ctx) throw() {
        ctx.diag.push(io::c_file_line_col,
            ctx.top_file(), ll.line(), ll.column()
        );
        state = DONE;
    }

    bool tokenizer::get_octal_digit(
        diagnostic_context &ctx,
        size_t i
    ) throw() {
        if(!cp.is_ascii()) {
            ctx.diag.push(io::e_invalid_octal_escape, cp.to_cstring());
            push_line_file_col(ctx);
            return false;
        }

        char digit = cp.to_cstring()[0];
        if(digit < '0' || '7' < digit) {
            ctx.diag.push(io::e_invalid_octal_escape, cp.to_cstring());
            push_line_file_col(ctx);
            return false;
        }

        int16_t new_val = scratch[i] + (digit - '0');
        if(new_val > 255) { // wrap-around
            ctx.diag.push(io::e_octal_escape_too_big, new_val, new_val);
            push_line_file_col(ctx);
            return false;
        }
        scratch[i] = static_cast<char>(new_val);
        return true;
    }

    bool tokenizer::get_hex_digit(
        diagnostic_context &ctx,
        size_t i
    ) throw() {
        if(!cp.is_ascii()) {
            ctx.diag.push(io::e_invalid_octal_escape, cp.to_cstring());
            push_line_file_col(ctx);
            return false;
        }

        char digit = tolower(cp.to_cstring()[0]);
        if(!isalnum(digit) || 'f' < digit) {
            ctx.diag.push(io::e_invalid_octal_escape, cp.to_cstring());
            push_line_file_col(ctx);
            return false;
        } else if('a' <= digit) {
            digit -= 'a' - ':';
        }

        scratch[i] += digit - '0';
        return true;
    }

    bool tokenizer::get_token(
        io::file<io::read_tag> &f,
        diagnostic_context &ctx,
        token &tok
    ) throw() {
        token_type tt;
        scratch[0] = '\0';

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
                    ctx.diag.push(io::e_mb_not_in_string, cp.to_cstring());
                    push_line_file_col(ctx);
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
                    state = READ_NEXT_CODEPOINT;
                    return true;
                }

                // \r, \r\n
                if('\r' == chr) {
                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_NEW_LINE;

                    if(!ll.get_codepoint(f, ctx, cp)) {
                        state = DONE;
                    } else {
                        chr = cp.to_cstring()[0];

                        if(cp.is_ascii()) {
                            if('\n' == chr) {
                                state = READ_NEXT_CODEPOINT;
                            } else {
                                state = HAVE_ASCII_CODEPOINT;
                            }
                        } else {
                            state = HAVE_NON_ASCII_CODEPOINT;
                        }
                    }
                    return true;

                // string literal
                } else if('"' == chr) {

                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_STRING_LITERAL;

                    size_t i(0);
                    for(; ll.get_codepoint(f, ctx, cp); ) {

                        if(i >= BUFFER_LENGTH) {
                            ctx.diag.push(io::e_string_too_long, BUFFER_LENGTH);
                            ctx.diag.push(io::c_file_line_col,
                                ctx.top_file(), tok.line_, tok.col_
                            );
                            state = DONE;
                            return false;
                        } else if(cp.is_null()) {
                            goto early_termination_of_string;
                        }

                        if(cp.is_ascii()) {
                            chr = cp.to_cstring()[0];

                            if('"' == chr) {
                                scratch[i] = '\0';
                                goto found_end_of_string;
                            } else if('\\' == chr) {
                                if(!ll.get_codepoint(f, ctx, cp)) {
                                    goto early_termination_of_string;
                                } else if(!cp.is_ascii()) {
                                    ctx.diag.push(io::e_mb_escape_string,
                                        cp.to_cstring()
                                    );
                                    push_line_file_col(ctx);
                                    return false;
                                }

                                scratch[i] = '\0';

                                switch(cp.to_cstring()[0]) {
                                case 'a': scratch[i] = '\x07'; break;
                                case 'b': scratch[i] = '\x08'; break;
                                case 'f': scratch[i] = '\x0C'; break;
                                case 'n': scratch[i] = '\n'; break;
                                case 'r': scratch[i] = '\r'; break;
                                case 't': scratch[i] = '\t'; break;
                                case 'v': scratch[i] = '\x0B'; break;
                                case '"': scratch[i] = '"'; break;
                                case '\\': scratch[i] = '\\'; break;
                                case 'o':
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    get_octal_digit(ctx, i);
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    get_octal_digit(ctx, i);
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    get_octal_digit(ctx, i);
                                    i += 2;
                                    break;
                                case 'x':
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    get_hex_digit(ctx, i);
                                    if(!ll.get_codepoint(f, ctx, cp)) {
                                        goto early_termination_of_string;
                                    }
                                    get_hex_digit(ctx, i);
                                    i += 1;
                                    break;

                                default:
                                    ctx.diag.push(io::e_invalid_escape,
                                        cp.to_cstring()[0]
                                    );
                                    push_line_file_col(ctx);
                                    return false;
                                }
                            } else {
                                scratch[i] = chr;
                            }

                            i += 1;

                        } else {
                            i += write_to_buff(&(scratch[i]), cp.to_cstring());
                        }
                    }

                early_termination_of_string:
                    ctx.diag.push(io::e_string_not_terminated);
                    ctx.diag.push(io::c_file_line_col,
                        ctx.top_file(), tok.line_, tok.col_
                    );
                    state = DONE;
                    return false;

                found_end_of_string:
                    return true;
                }

                // skip non-newline whitespace
                if(isspace(chr)) {
                    continue;
                }

                if(isalpha(chr)) {

                    tok.line_ = ll.line();
                    tok.col_ = ll.column();

                    // type name, starts with upper case
                    if(tolower(chr) != chr) {
                        tok.type_ = T_TYPE_NAME;

                    // name, starts with lower case
                    } else {
                        tok.type_ = T_NAME;
                    }

                    scratch[0] = static_cast<char>(chr);
                    size_t i(1);

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
                            ctx.diag.push(io::e_name_too_long,
                                scratch, NAME_LENGTH
                            );
                            ctx.diag.push(io::c_file_line_col,
                                ctx.top_file(), tok.line_, tok.col_
                            );
                            state = DONE;
                            return false;
                        }

                        chr = cp.to_cstring()[0];

                        // error, character not allowed in a name
                        if(SCA_ERROR == NAME_SUFFIX_CHAR[chr]) {
                            ctx.diag.push(io::e_bad_char_in_name, chr);
                            push_line_file_col(ctx);
                            return false;

                        // token barrier
                        } else if(SCA_BARRIER == NAME_SUFFIX_CHAR[chr]) {
                            state = HAVE_ASCII_CODEPOINT;
                            break;

                        // collect another char
                        } else {
                            scratch[i] = static_cast<char>(chr);
                        }
                    }

                    scratch[i] = '\0';
                    return true;

                // integer or float
                } else if(isdigit(chr)) {
                    tok.line_ = ll.line();
                    tok.col_ = ll.column();
                    tok.type_ = T_INTEGER_LITERAL;
                    scratch[0] = chr;
                    size_t i(1);

                    scratch[i] = '\0';
                }
            }
        case DONE: break;
        }

        return false;
    }

    const char *tokenizer::get_value(void) const throw() {
        return &(scratch[0]);
    }

    const token_type tokenizer::SINGLE_CHAR_TOKENS[]{
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
        T_NEW_LINE, //  10     ^J
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
        T_MINUS, //  45     -
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
        T_COLON, //  58     :
        T_SEMICOLON, //  59     ;
        T_LESS_THAN, //  60     <
        T_EQUAL, //  61     =
        T_GREATER_THAN, //  62     >
        T_INVALID, //  63     ?
        T_INVALID, //  64     @
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
}

