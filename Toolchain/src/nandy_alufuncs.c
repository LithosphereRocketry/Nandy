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