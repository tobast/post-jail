#include "cfgFiles.h"

#include <ctime>
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>
#include <fstream>
#include <string>

bool fileExists(const char* path) {
	struct stat fStat;
	return (stat(path, &fStat) == 0);
}

bool hasCooldownLock() {
	return fileExists(LOCK_FILE_NAME);
}

bool deleteCooldownLock(time_t ifBefore) {
	struct stat fStat;
	if(stat(LOCK_FILE_NAME, &fStat) != 0)
		return false;
	if(fStat.st_mtim.tv_sec < ifBefore)
		return (remove(LOCK_FILE_NAME) == 0);
	return false;
}

bool createCooldownLock() {
	FILE* handle = fopen(LOCK_FILE_NAME, "w");
	if(handle == NULL)
		return false;
	fclose(handle);
	return true;
}

std::string strip(const std::string& str) {
	int beg=-1,end=-1;
	for(size_t pos=0; pos < str.size(); pos++) {
		if(str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\n')
			beg = pos;
	}
	for(int pos=(int)str.size() - 1; pos >= 0; pos--) {
		if(str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\n')
			end = pos+1;
	}
	if(end < 0 || beg < 0) // String is [ \t\n]*
		return "";

	return str.substr(beg, end-beg);
}

int toNumber(const std::string& str) {
	// Not even using atoi: we want to be sure it's an integer, and properly
	// handle errors.

	int outNum=0;
	for(size_t pos=0; pos < str.size(); pos++) {
		if(pos > sizeof(int) * 8 * 0.30102 - 1 /*log(2)/log(10)*/) {
			fprintf(stderr, "Possible overflow on %s, aborting.\n",
					str.c_str());
			exit(1);
		}
		if(str[pos] < '0' || str[pos] > '9') {
			fprintf(stderr, "Unexpected character '%c' in %s, a number. "
					"Aborting.\n", str[pos], str.c_str());
			exit(1);
		}
		outNum *= 10;
		outNum += str[pos] - '0';
	}
	return outNum;
}

Config readConfig(const char* path) {
	std::ifstream handle(path);
	Config cfg = defaultConfig();

	if(handle.is_open()) {
		std::string line;
		while(getline(handle, line)) {
			if(line[0] == '#')
				continue;
			size_t eqPos = line.find("=");
			if(eqPos == std::string::npos)
				continue;
			std::string tag = strip(line.substr(0, eqPos));
			std::string val = strip(line.substr(eqPos+1));

			// Bwearkh.
			if(tag == "url")
				cfg.challengeUrl = val.c_str();
			else if(tag == "http_header")
				cfg.httpHeader = val.c_str();
			else if(tag == "post_header")
				cfg.postHeader = val.c_str();
			else if(tag == "post_footer")
				cfg.postFooter = val.c_str();
			else if(tag == "on_success")
				cfg.onSuccessProgram = val.c_str();
			else if(tag == "challenge_length")
				cfg.challengeLength = toNumber(val);
			else if(tag == "cooldown_time")
				cfg.authCooldownTime = toNumber(val);
			else {
				fprintf(stderr, "Invalid configuration tag: %s.\n",
						tag.c_str());
				exit(1);
			}
		}
	}
	else {
		fprintf(stderr, "Can't open configuration file. Aborting.\n");
		exit(1);
	}
	return cfg;
}

Config defaultConfig() { // FIXME DEBUG - this function MUST be removed later
	Config out;
	out.challengeUrl = "https://tobast.fr/hooks/postdump";
	out.httpHeader = "Content-Type: application/json";
	out.postHeader = "{ \"greet\":\"Oh, hello!\", \"challenge\":\"";
	out.postFooter = "\", \"farewell\":\"See ya!\" }";
	out.onSuccessProgram = "/bin/bash";
	out.challengeLength = 8;
	out.authCooldownTime = 5*60;

	return out;
}

