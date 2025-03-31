#ifndef WEBX_DATA_BUFFER_H
#define WEBX_DATA_BUFFER_H

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <cstring>
#include <crc32/Crc32.h>
#include <spdlog/spdlog.h>

/**
 * A dynamic buffer for storing and manipulating binary data.
 */
class WebXDataBuffer {
public:
    /**
     * Default constructor. Initializes an empty buffer with zero capacity and size.
     */
    WebXDataBuffer() :
        _buffer(0),
        _capacity(0),
        _size(0) {
    }

    /**
     * Constructor that initializes the buffer with a specified initial capacity.
     * @param initialCapacity The initial capacity of the buffer.
     */
    WebXDataBuffer(size_t initialCapacity) :
        _buffer(0),
        _capacity(initialCapacity),
        _size(0) {

        _buffer = (unsigned char *)malloc(_capacity);
    }

    /**
     * Constructor that wraps an existing buffer.
     * @param buffer Pointer to the existing buffer.
     * @param bufferSize Size of the existing buffer.
     */
    WebXDataBuffer(unsigned char * buffer, size_t bufferSize) :
        _buffer(buffer),
        _capacity(bufferSize),
        _size(bufferSize) {

    }

    /**
     * Destructor. Frees the allocated buffer memory if it exists.
     */
    virtual ~WebXDataBuffer() {
        if (_buffer != 0) {
            free (this->_buffer);
            _buffer = 0;
        }
    }

    /**
     * Gets the pointer to the internal buffer.
     * @return Pointer to the internal buffer.
     */
    unsigned char * getBuffer() const {
        return this->_buffer;
    }

    /**
     * Gets the current size of the buffer (number of bytes stored).
     * @return The size of the buffer.
     */
    size_t getBufferSize() const {
        return this->_size;
    }

    /**
     * Gets the current capacity of the buffer.
     * @return The capacity of the buffer.
     */
    size_t getCapacity() const {
        return this->_capacity;
    }

    /**
     * Resets the buffer size to zero without deallocating memory.
     */
    void reset() {
        this->_size = 0;
    }

    /**
     * Appends data to the buffer, resizing it if necessary.
     * @param data Pointer to the data to append.
     * @param length Length of the data to append.
     * @return True if the data was successfully appended, false otherwise.
     */
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

    /**
     * Calculates the CRC32 checksum of the buffer's contents.
     * @return The calculated checksum.
     */
    uint32_t calculateChecksum() const {
        uint32_t checksum = crc32_16bytes(this->_buffer, this->_size);
        return checksum;  
    }

private:
    /**
     * Calculates the next power of two greater than or equal to the given value.
     * @param v The input value.
     * @return The next power of two.
     */
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