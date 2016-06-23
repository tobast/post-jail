#pragma once
#include "constants.h"
#include <ctime>

/// Checks whether a file exists
bool fileExists(const char* path);

/// Checks whether the cooldown lock file is present or not
bool hasCooldownLock();

/// Creates the cooldown lock
bool createCooldownLock();

/// Deletes the cooldown lock
bool deleteCooldownLock(time_t ifBefore);

struct Config {
	const char* challengeUrl;
	const char* postHeader;
	const char* postFooter;
	const char* httpHeader;

	const char* onSuccessProgram;

	unsigned challengeLength;

	unsigned authCooldownTime;
};

Config readConfig(const char* path);

Config defaultConfig();

