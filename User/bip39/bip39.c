/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "bip39.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "sha2.h"
// #include "bip39_chinese_simplified.h"
// #include "bip39_chinese_traditional.h"
#include "bip39_english.h"
// #include "bip39_french.h"
// #include "bip39_italian.h"
// #include "bip39_japanese.h"
// #include "bip39_spanish.h"
#include "memzero.h"
#include "options.h"
#include "includes.h"
#include "rng.h"

const char* mnemonic_generate(int strength) {
	if (strength % 32 || strength < 128 || strength > 256) {
		return 0;
	}
	uint8_t data[32];

	if (!random_buffer(data, 32)) {
		return 0;
	}

	const char *r = mnemonic_from_data(data, strength / 8);
	memzero(data, sizeof(data));
	return r;
}

const int random_buffer(uint8_t *buf, uint8_t len) {
	uint32_t r = 0;

	for (uint8_t i = 0; i < len; i++) {
		if (i % 4 == 0) {
			if (HAL_RNG_GenerateRandomNumber(&hrng, &r) != HAL_OK) {
				return 0;
			}
			DBG_LOG("Got 32bits of Good Entropy...");
		}
		buf[i] = (r >> ((i % 4) * 8)) & 0xFF;
	}
	return 1;
}

const char* mnemonic_from_data(const uint8_t *data, int len) {
	if (len % 4 || len < 16 || len > 32) {
		return 0;
	}

	uint8_t bits[32 + 1];

	sha256_Raw(data, len, bits);
	// checksum
	bits[len] = bits[0];
	// data
	memcpy(bits, data, len);

	int mlen = len * 3 / 4;
	static CONFIDENTIAL char mnemo[24 * 10];

	int i, j, idx;
	char *p = mnemo;
	for (i = 0; i < mlen; i++) {
		idx = 0;
		for (j = 0; j < 11; j++) {
			idx <<= 1;
			idx += (bits[(i * 11 + j) / 8] & (1 << (7 - ((i * 11 + j) % 8))))
					> 0;
		}
		strcpy(p, wordlist[idx]);
		p += strlen(wordlist[idx]);
		*p = (i < mlen - 1) ? ' ' : 0;
		p++;
	}
	memzero(bits, sizeof(bits));

	return mnemo;
}

