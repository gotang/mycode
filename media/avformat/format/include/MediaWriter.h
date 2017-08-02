/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_WRITER_H_

#define MEDIA_WRITER_H_

namespace avformat {

struct MediaSource;
struct MetaData;

struct MediaWriter {
    MediaWriter()
        : mMaxFileSizeLimitBytes(0),
          mMaxFileDurationLimitUs(0) {
    }

    virtual status_t addSource(MediaSource *source) = 0;
    virtual bool reachedEOS() = 0;
    virtual status_t start() = 0;
    virtual status_t stop() = 0;
    virtual status_t pause() = 0;

    virtual void setMaxFileSize(int64_t bytes) { mMaxFileSizeLimitBytes = bytes; }
    virtual void setMaxFileDuration(int64_t durationUs) { mMaxFileDurationLimitUs = durationUs; }
    virtual void setListener() {
    }

    virtual status_t dump(int fd, const vector<const char*> & args) {
        return OK;
    }

    virtual void setStartTimeOffsetMs(int ms) {}
    virtual int32_t getStartTimeOffsetMs() const { return 0; }

protected:
    virtual ~MediaWriter() {}
    int64_t mMaxFileSizeLimitBytes;
    int64_t mMaxFileDurationLimitUs;

    void notify(int msg, int ext1, int ext2) {
    }
private:
    MediaWriter(const MediaWriter &);
    MediaWriter &operator=(const MediaWriter &);
};

}  // namespace avformat

#endif  // MEDIA_WRITER_H_
