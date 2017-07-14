#define LOG_NDEBUG 0
#define LOG_TAG "MICTest"
#include <utils/Log.h>

#include <stdlib.h>
#include <assert.h>

#include "SpeakerDemo.h"

int main(int argc, char *argv[]) {
	SpeakerDemo *speaker = new SpeakerDemo();
    int opt;
	const char *path  = NULL;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
        	path = optarg;
            break;
        default:
        	exit(0);
        }
    }
    if(path == NULL) {
    	path = "/system/res/others/startup.wav";
    }

	speaker->setDataSource(path);
	int err = speaker->run();
	assert(err != 0);
	delete speaker;
	return 0;
}
