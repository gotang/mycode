/*
 * Copyright (C) 2009 The Android Open Source Project
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

#ifndef DATA_SOURCE_H_
#define DATA_SOURCE_H_
#include <list>
#include <sys/types.h>
#include <utils/Mutex.h>
#include <MediaErrors.h>
using namespace std;

namespace avformat {

class DataSource {
public:
    enum Flags {
        kWantsPrefetching      = 1,
        kStreamedFromLocalHost = 2,
        kIsCachingDataSource   = 4,
        kIsHTTPBasedSource     = 8,
    };
    //DataSource must be deleted by caller.
    static DataSource* CreateFromURI(const char *uri);

    DataSource() {
    	ALOGI("DataSource, this %p", this);
    }

    virtual status_t initCheck() const = 0;

    virtual ssize_t readAt(off64_t offset, void *data, size_t size) = 0;

    // Convenience methods:
    bool getUInt16(off64_t offset, uint16_t *x);
    bool getUInt24(off64_t offset, uint32_t *x); // 3 byte int, returned as a 32-bit int
    bool getUInt32(off64_t offset, uint32_t *x);
    bool getUInt64(off64_t offset, uint64_t *x);

    // May return ERROR_UNSUPPORTED.
    virtual status_t getSize(off64_t *size);

    virtual uint32_t flags() {
        return 0;
    }

    virtual status_t reconnectAtOffset(off64_t offset) {
        return ERROR_UNSUPPORTED;
    }

    ////////////////////////////////////////////////////////////////////////////

    bool sniff(const char **mimeType, float *confidence);

    // The sniffer can optionally fill in "meta" with an AMessage containing
    // a dictionary of values that helps the corresponding extractor initialize
    // its state without duplicating effort already exerted by the sniffer.
    typedef bool (*SnifferFunc)(
            DataSource *source, const char **mimeType,
            float *confidence);

    static void RegisterDefaultSniffers();

    virtual const char *getUri() {
        return NULL;
    }

    virtual const char* getMIMEType() const;

    virtual ~DataSource() {
    	ALOGI("~DataSource, this %p", this);
    }

private:
    static Mutex gSnifferMutex;
    static list<SnifferFunc> gSniffers;
    static bool gSniffersRegistered;

    static void RegisterSniffer_l(SnifferFunc func);

    DataSource(const DataSource &);
    DataSource &operator=(const DataSource &);
};

}  // namespace avformat

#endif  // DATA_SOURCE_H_
