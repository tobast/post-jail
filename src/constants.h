#pragma once

extern const char* LOCK_FILE_NAME;
const unsigned MAX_CHALLENGE_LEN = 128; // Must be < 256 for /dev/random

extern const char* USED_CHARS;
const unsigned USED_CHARS_LEN=64; // WARNING! If this is not a divisor of 256,
	// the characters will *NOT* be picked uniformly.

const unsigned TIMEOUT_DELAY=5*60; // 5 minutes
