#ifndef WEBX_DATA_BUFFER_H
#define WEBX_DATA_BUFFER_H

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <cstring>
#include <crc32/Crc32.h>
#include <spdlog/spdlog.h>

class WebXDataBuffer {
public:
    WebXDataBuffer() :
        _buffer(0),
        _capacity(0),
        _size(0) {
    }

    WebXDataBuffer(size_t initialCapacity) :
        _buffer(0),
        _capacity(initialCapacity),
        _size(0) {

        _buffer = (unsigned char *)malloc(_capacity);
    }

    WebXDataBuffer(unsigned char * buffer, size_t bufferSize) :
        _buffer(buffer),
        _capacity(bufferSize),
        _size(bufferSize) {

    }

    virtual ~WebXDataBuffer() {
        if (_buffer != 0) {
            free (this->_buffer);
            _buffer = 0;
        }
    }

    unsigned char * getBuffer() const {
        return this->_buffer;
    }

    size_t getBufferSize() const {
        return this->_size;
    }

    size_t getCapacity() const {
        return this->_capacity;
    }

    void reset() {
        this->_size = 0;
    }

    bool appendData(unsigned char * data, size_t length) {
        size_t newSize = this->_size + length;
        size_t capacity = this->_capacity;

        // allocate or grow buffer
        while (newSize > capacity) {
            if (capacity == 0) {
                capacity = upperPowerOfTwo(newSize);

            } else {
                capacity = capacity * 2;
            }
        }

        if (capacity > this->_capacity) {
            if (this->_capacity == 0) {
                this->_buffer = (unsigned char *)malloc(capacity);

            } else {
                this->_buffer = (unsigned char *)realloc(this->_buffer, capacity);
            }

            if (this->_buffer == 0) {
                spdlog::error("Failed to allocate {:zu} bytes for data buffer", capacity);
                return false;
            }
            this->_capacity = capacity;
        }


        // copy new bytes to end of buffer
        memcpy(this->_buffer + this->_size, data, length);
        this->_size = newSize;

        return true;
    }

    uint32_t calculateChecksum() const {
        spdlog::trace("Calculating checksum of WebXDataBuffer");
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        uint32_t checksum = crc32_16bytes(this->_buffer, this->_size);

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duration = end - start;
        spdlog::trace("Checksum for {:d} bytes in {:f}us", this->_size, duration.count());
        return checksum;  
    }

private:
    unsigned long upperPowerOfTwo(unsigned long v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

private:
    unsigned char * _buffer;
    size_t _capacity;
    size_t _size;
};

#endif /* WEBX_DATA_BUFFER_H */