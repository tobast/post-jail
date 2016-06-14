#include "cfgFiles.h"

#include <unistd.h>
#include <sys/stat.h>

bool fileExists(const char* path) {
	struct stat fStat;
	return (stat(path, &fStat) == 0);
}

bool hasCooldownLock() {
	return fileExists(LOCK_FILE_NAME);
}

Config defaultConfig() { // FIXME DEBUG - this function MUST be removed later
	Config out;
	out.challengeUrl = "https://tobast.fr/hooks/postdump";
	out.httpHeader = "Content-Type: application/json";
	out.postHeader = "{ \"greet\":\"Oh, hello!\", \"challenge\":\"";
	out.postFooter = "\", \"farewell\":\"See ya!\" }";
	out.challengeLength = 8;

	return out;
}

