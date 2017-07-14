#define LOG_NDEBUG 0
#define LOG_TAG "MICDemo"
#include <utils/Log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asoundlib.h>

#include "MICDemo.h"

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define FORMAT_PCM 1

struct wav_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t riff_fmt;
	uint32_t fmt_id;
	uint32_t fmt_sz;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t data_id;
	uint32_t data_sz;
};

MICDemo::MICDemo()
	:mUri(NULL),
	 mErrorCode(0),
	 mDuration(0){

}

MICDemo::~MICDemo() {

}


void MICDemo::setDataSource(const char *path) {
	mUri = strdup(path);
}

void MICDemo::setDuration(int sec) {
	mDuration = sec;
}

int MICDemo::run()
{
	uint32_t card = 0;
	uint32_t device = 0;
	uint32_t channels = 2;
	uint32_t rate = 44100;
	uint32_t bits = 16;
	uint32_t frames;
	uint32_t period_size = 1024;
	uint32_t period_count = 4;

	int fd = open (mUri, O_CREAT | O_TRUNC | O_RDWR, 0644);
	if(fd < 0) {
		return -1;
	}

	struct wav_header header = {
		ID_RIFF,
		0,
		ID_WAVE,
		ID_FMT,
		16,
		FORMAT_PCM,
		channels,
		rate,
		(bits / 8) * channels * rate,
		(bits / 8) * channels,
		bits,
		ID_DATA,
		0
	};

	lseek(fd, sizeof(struct wav_header), SEEK_SET);

//	frames = CaptureSample(recFp, card, device, header.num_channels, header.sample_rate, header.bits_per_sample, period_size, period_count);
	struct pcm_config config;

	config.channels = channels;
	config.in_init_channels = channels;
	config.rate = rate;
	config.period_size = period_size;
	config.period_count = period_count;
	if (bits == 32)
		config.format = PCM_FORMAT_S32_LE;
	else
		config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	struct pcm *pcm = pcm_open_req(card, device, PCM_IN, &config, rate);
	if (pcm == NULL || pcm_is_ready(pcm) == 0) {
		ALOGE("open PCM device failed\n");
		return 0;
	}

	uint32_t size = pcm_get_buffer_size(pcm);
	uint8_t *buffer = (uint8_t*)malloc(size);
	if (buffer == NULL) {
		ALOGE("malloc %d bytes failed\n", size);
		pcm_close(pcm);
		return 0;
	}

	uint32_t captureBytes = 0;
	uint32_t totalSize =  (bits / 8) * channels * rate * mDuration;
	ALOGV("capturing, channel %d, rate %d, bits %d, size %d\n", channels, rate, bits, totalSize);

	while (!pcm_read(pcm, buffer, size)) {
		if (write(fd, buffer, size) != (ssize_t)size) {
			ALOGE("capturing sound error\n");
			break;
		}
		captureBytes += size;
		if(totalSize <= captureBytes) {
			break;
		}
	}

	free(buffer);
	pcm_close(pcm);

	frames = captureBytes / ((bits / 8) * channels);

	header.data_sz = frames * header.block_align;
	lseek(fd, 0, SEEK_SET);
	write(fd, &header, sizeof(struct wav_header));

	close(fd);
	ALOGV("captrued frame %d", frames);
	return 0;
}
