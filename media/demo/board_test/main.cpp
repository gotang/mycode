#define LOG_NDEBUG 0
#define LOG_TAG "main"
#include <utils/Log.h>

#include "BoardTestConfig.h"

using namespace android;
int main(int argc, char *argv[]) {
	BoardTestConfig *config = new BoardTestConfig();
	delete config;
	return 0;
}
