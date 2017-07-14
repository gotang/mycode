#define LOG_NDEBUG 0
#define LOG_TAG "SpeakerDemo"
#include <utils/Log.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asoundlib.h>

#include "SpeakerDemo.h"

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t wave_id;
};

struct chunk_header {
	uint32_t id;
	uint32_t sz;
};

struct chunk_fmt {
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
};

SpeakerDemo::SpeakerDemo()
	:mUri(NULL),
	 mErrorCode(0) {

}

SpeakerDemo::~SpeakerDemo() {

}

void SpeakerDemo::setDataSource(const char *path) {
	mUri = strdup(path);
}

int SpeakerDemo::run()
{
	struct riff_wave_header riff_wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;
	uint32_t device = 0;
	uint32_t card = 0;
	uint32_t period_size = 1024;
	uint32_t period_count = 4;
	int more_chunks = 1;

	int fd = open(mUri, O_RDONLY | O_NONBLOCK);
	if(fd < 0) {
		return -1;
	}
	read(fd, &riff_wave_header, sizeof(riff_wave_header));

	if ((riff_wave_header.riff_id != ID_RIFF) || (riff_wave_header.wave_id != ID_WAVE)) {
		ALOGE("Error: '%s' is not a riff/wave file\n", (char*)mUri);
		close(fd);
		return -1;
	}
	system("tinymix 1 31");
	system("tinymix 16 1");

	while(more_chunks) {
		int bytes = read(fd, &chunk_header, sizeof(chunk_header));
		switch (chunk_header.id) {
			case ID_FMT:
				read(fd, &chunk_fmt, sizeof(chunk_fmt));
				/* If the format header is larger, skip the rest */
				if (chunk_header.sz > sizeof(chunk_fmt))
					lseek(fd, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
			    	break;

			case ID_DATA:
				/* Stop looking for chunks */
				more_chunks = 0;
				break;

			default:
				/* Unknown chunk, skip bytes */
				lseek(fd, chunk_header.sz, SEEK_CUR);
		}
	}

	struct pcm_config config;
	config.channels = chunk_fmt.num_channels;
	config.rate = chunk_fmt.sample_rate;
	config.period_size = period_size;
	config.period_count = period_count;
	if (chunk_fmt.bits_per_sample == 32)
		config.format = PCM_FORMAT_S32_LE;
	else if (chunk_fmt.bits_per_sample == 16)
		config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	struct pcm *pcm = pcm_open(card, device, PCM_OUT, &config);
	if (!pcm || !pcm_is_ready(pcm)) {
		return -1;
	}

	int size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
	uint8_t *buffer = (uint8_t *)malloc(size);
	if (buffer == NULL) {
		pcm_close(pcm);
		return -1;
	}
	ALOGV("playing wav file: %u ch, %u hz, %u bit\n", config.channels, config.rate, config.format);
	while(1) {
		int bytes = read(fd, buffer, size);
		if (bytes > 0) {
			if (pcm_write(pcm, buffer, bytes)) {
				break;
			}
		} else {
			break;
		}
	}
	ALOGV("end of the wav file\n");

	free(buffer);
	pcm_close(pcm);

	close(fd);
	return 0;
}
