#include "nandy_alufuncs.h"

word_t alu_add(word_t a, word_t b, bool cin, bool* cout) {
    int result = (((int) a) & 0xFF) + (((int) b) & 0xFF) + ((int) cin);
	if(cout) {
        *cout = (result & 0x100) != 0;
    }
    return result & 0xFF;
}

word_t alu_sub(word_t a, word_t b, bool cin, bool* cout) {
    int result = (((int) a) & 0xFF) + 255 + ((int) cin) - (((int) b) & 0xFF);
	if(cout) {
        *cout = (result & 0x100) != 0;
    }
    return result & 0xFF;
}

word_t alu_sl(word_t a, word_t b, bool cin, bool* cout) {
    word_t result = a << 1 | (cin ? 1 : 0);
    if(cout) {
        *cout = (a >> 7) & 1;
    }
    return result;
}

word_t alu_sr(word_t a, word_t b, bool cin, bool* cout) {
    word_t result = a >> 1 | (cin ? 0x80 : 0);
    if(cout) {
        *cout = a & 1;
    }
    return result;
}