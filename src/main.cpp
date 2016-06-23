#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <string.h>

#include <linux/random.h>
#include <syscall.h>
#include <unistd.h>

#include "cfgFiles.h"
#include "postSender.h"

/// Picks a character from USED_CHARS using the given byte
inline char charOfRand(char randByte) {
	return USED_CHARS[randByte % USED_CHARS_LEN];
}

void genChallenge(unsigned challengeLen, char* buff) {
	assert(challengeLen < MAX_CHALLENGE_LEN);

	// Get crypto-quality random numbers from the system
	unsigned pos = 0;
	while(pos < challengeLen) { 
		pos += syscall(SYS_getrandom, buff+pos,
				challengeLen-pos, GRND_RANDOM);
	}

	for(pos=0; pos < challengeLen; pos++)
		buff[pos] = charOfRand(buff[pos]);
	buff[challengeLen] = '\0';
}

void genPostData(const char* header, const char* randBuff,
		const char* footer, char* buff)
{
	strcpy(buff, header);
	int cpypos = strlen(header); // Overrides \0 then.
	strcpy(buff+cpypos, randBuff);
	cpypos += strlen(randBuff);
	strcpy(buff + cpypos, footer); // Copies a \0 character.
}

void lockDeleter(time_t creationTime, time_t deleteAfter) {
	pid_t cPid = fork();
	if(cPid < 0) { // Error
		return; // TODO better error handling?
	}
	else if(cPid == 0) { // Child
		int timeToSleep = difftime(deleteAfter, time(NULL));
		while(timeToSleep > 0) {
			sleep(timeToSleep);
			timeToSleep = difftime(deleteAfter, time(NULL));
		}
		deleteCooldownLock(creationTime+15);
	}
	// Else parent
}

void onFailure(time_t lockCreation, time_t lockDeleteTime) {
	fprintf(stderr, "Authentication failed.\n");
	lockDeleter(lockCreation, lockDeleteTime);
}

void onSuccess(const Config& cfg) {
	execl(cfg.onSuccessProgram, cfg.onSuccessProgram, NULL);
	fprintf(stderr, "execl failed.\n");
	exit(1); // If this is executed, execl failed.
}

void readInput(const Config& cfg, const char* challenge) {
	time_t startInputTime = time(NULL);
	time_t lockDeleteTime = startInputTime + cfg.authCooldownTime;
	int tryId;

	for(tryId=0; tryId < 3; tryId++) {
		if(tryId > 0)
			printf("Type in the challenge [try %d/3]: ", tryId+1);

		char userPass[MAX_CHALLENGE_LEN+1];
		unsigned typePos = 0;
		do {
			char c = getchar();
			if(c == '\n')
				break;
			userPass[typePos] = c;
			typePos++;
		} while(typePos < MAX_CHALLENGE_LEN);
		userPass[typePos] = '\0';

		if(difftime(time(NULL), startInputTime) > TIMEOUT_DELAY) {
			onFailure(startInputTime, lockDeleteTime);
			printf("Timed out.\n");
			exit(2);
		}

		if(strcmp(userPass, challenge) == 0) {
			deleteCooldownLock(startInputTime+15); // 15s are allowed
			onSuccess(cfg);
			exit(0);
		}

		printf("Wrong challenge.\n");
	}
	if(tryId == 3) { // Failed three times
		onFailure(startInputTime, lockDeleteTime);
		exit(2);
	}
}

int main(int /*argc*/, char** /*argv*/) {
	if(hasCooldownLock()) {
		printf("The last failed login attempt is too recent. "
				"Please wait.\n");
		exit(2);
	}

	// TODO maybe more configured security checks here?
	
	Config cfg;
	cfg = readConfig(CONFIG_PATH);

	if(cfg.challengeLength >= MAX_CHALLENGE_LEN) {
		fprintf(stderr, "The required challenge is too long (%u bytes), it "
				"should be at most %u chars long.\n",
				cfg.challengeLength, MAX_CHALLENGE_LEN);
		exit(1);
	}
	
	printf("Sending a challenge... ");

	char challenge[MAX_CHALLENGE_LEN + 1];
	genChallenge(cfg.challengeLength, challenge);
	
	char* postBuffer = (char*)malloc(
			strlen(cfg.postHeader) + strlen(cfg.postFooter) +
			cfg.challengeLength + 10);
	genPostData(cfg.postHeader, challenge, cfg.postFooter, postBuffer);
	
	sendChallenge(cfg.httpHeader, postBuffer, cfg.challengeUrl);
	free(postBuffer);

	if(!createCooldownLock()) {
		fprintf(stderr, "Could not create cooldown lock.\n");
		printf("Unexpected error, aborting.\n");
		exit(1);
	}
	
	printf("Challenge sent. Please type it in:\n");
	
	// === INPUT ===
	readInput(cfg, challenge); // This function should NEVER terminate.

	// This point should never be reached -- either we signal an error, or
	// we fork then exit(0).
	// Then, after this point, we consider that we've encountered a crash...
	fprintf(stderr, "Unexpected program termination.\n");
	return 1;
}

