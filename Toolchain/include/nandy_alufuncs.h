#ifndef NANDY_ALUFUNCS_H
#define NANDY_ALUFUNCS_H

#include <stdbool.h>
#include <nandy_tools.h>

word_t alu_add(word_t a, word_t b, bool cin, bool* cout);
word_t alu_sub(word_t a, word_t b, bool cin, bool* cout);
word_t alu_sl(word_t a, word_t b, bool cin, bool* cout);
word_t alu_sr(word_t a, word_t b, bool cin, bool* cout);

#endif