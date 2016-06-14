#pragma once
#include "constants.h"

/// Checks whether a file exists
bool fileExists(const char* path);

/// Checks whether the cooldown lock file is present or not
bool hasCooldownLock();

struct Config {
	const char* challengeUrl;
	const char* postHeader;
	const char* postFooter;
	const char* httpHeader;

	unsigned challengeLength;
};

Config defaultConfig();

