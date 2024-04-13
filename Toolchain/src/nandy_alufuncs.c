#include "nandy_alufuncs.h"

bool parity(word_t w) {
	// there's faster ways to do this but who cares it's 8 bits
    for(size_t i = 0; i < 8*sizeof(word_t); i++) {
		w ^= w >> i;
	}
	return w & 1;
}

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