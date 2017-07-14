#define LOG_NDEBUG 0
#define LOG_TAG "TFCardTest"
#include <utils/Log.h>

#include <assert.h>
#include "TFCardDemo.h"

int main(int argc, char *argv[]) {
	TFCardDemo *tfCard = new TFCardDemo();
	tfCard->write();
#if 0
	int err = tfCard->init("/mnt/extsd");
	assert(err != 0);
	err = tfCard->write();
	assert(err != 0);
	err = tfCard->reset();
	assert(err != 0);
#endif
	delete tfCard;
	return 0;
}
