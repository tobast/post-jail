#pragma once
#include "constants.h"
#include <ctime>
#include <string>

/// Checks whether a file exists
bool fileExists(const char* path);

/// Checks whether the cooldown lock file is present or not
bool hasCooldownLock();

/// Creates the cooldown lock
bool createCooldownLock();

/// Deletes the cooldown lock
bool deleteCooldownLock(time_t ifBefore);

struct Config {
	std::string challengeUrl;
	std::string postHeader;
	std::string postFooter;
	std::string httpHeader;

	std::string onSuccessProgram;

	unsigned challengeLength;

	unsigned authCooldownTime;
};

Config readConfig(const char* path);

Config defaultConfig();

