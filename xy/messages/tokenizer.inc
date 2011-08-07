
XY_MSG(e_mb_not_in_string, error, "Non-ASCII UTF-8 codepoint '%s' is present outside of a string literal. Multibyte characters are only allowed in strings.")
XY_MSG(e_bad_char_in_name, failed_assertion, "Bad character found with ordinal value '%d' found in name.")
XY_MSG(e_name_too_long, error, "The name (identifier/function name/type name/keyword; starting with '%s') is too long. The maximum length of a name is %u.")
XY_MSG(e_string_too_long, error, "String literal is too long. The maximum length of a string literal is %u bytes. If you want to make a really long string literal then place several string literals beside each other; they will automatically be concatenated.")
XY_MSG(e_string_not_terminated, recoverable_error, "String literal has no closing '\"' to indicate where the string ends. This could be because the string is malformed and is the last thing in the file, or it could also be because an error occured before we reached the end of the string while reading the file. I'm going to assume the former.")
XY_MSG(e_mb_escape_string, error, "Cannot escape (using the '\\') the multibyte character '%s' (or any multibyte characters for that matter) in a string literal. Valid escape sequences are \\a, \\b, \\f, \\n, \\r, \\t, \\v, \\\", \\\\, \\oOOO (where O is an octal digit), and \\xXX (where X is a hexadecimal digit).")
XY_MSG(e_invalid_octal_escape, error, "The character '%s' is not valid as a digit of an octal number in an escape sequence in a string literal. Octal escape characters take the form \\oOOO where O is a number from 0 to 7, inclusive.")
XY_MSG(e_invalid_hex_escape, error, "The character '%s' is not valid as a digit of a hexadecimal number in an escape sequence in a string literal. Hexadecimal escape characters take the form \\xXX where X is a digit from 0 to 9, a to f, or A to F, inclusive.")
XY_MSG(e_invalid_escape, error, "The escape sequence '\\%c' is invalid. Valid escape sequences are \\a, \\b, \\f, \\n, \\r, \\t, \\v, \\\", \\\\, \\oOOO (where O is an octal digit), and \\xXX (where X is a hexadecimal digit).")
XY_MSG(e_octal_escape_too_big, error, "The octal escape sequence '\\o%03o' (which evaluates to the character with value '%d') cannot fit in a single byte. The maximum evaluated value of an octal-escaped byte is 255, or \\o377.")
XY_MSG(e_unclosed_block_comment, recoverable_error, "%d block comment(s) (starting with '-*' and ending with '*-') was/were left unclosed. This could be because one or more block comments were opened near the end of the file and because one was left unclosed, or because an error occurred while reading the contents of the file. I am going to assume the former.")
XY_MSG(n_start_of_block_comment, note, "Start of unclosed block comment was here.")

