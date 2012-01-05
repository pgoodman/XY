
#include "xy/include/color.hpp"

XY_MSG(c_file, special, XY_F_BOLD "%s: " XY_F_DEF)
XY_MSG(c_file_line_col, special, XY_F_BOLD "%s:%u:%u: " XY_F_DEF)
XY_MSG(c_highlight, special, "%s")

#include "xy/messages/lexer.inc"
#include "xy/messages/tokenizer.inc"
#include "xy/messages/parser.inc"
#include "xy/messages/resolve_names.inc"
#include "xy/messages/type_checker.inc"

