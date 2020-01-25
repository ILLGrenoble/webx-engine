#ifndef WEBX_DATA_BUFFER_H
#define WEBX_DATA_BUFFER_H

#include <stdlib.h>
#include <string>

class WebXDataBuffer {
public:
    WebXDataBuffer();
    WebXDataBuffer(size_t initialCapacity);
    WebXDataBuffer(unsigned char * buffer, size_t bufferSize);
    virtual ~WebXDataBuffer();

    bool appendData(unsigned char * data, size_t length);

    unsigned char * getBuffer() const {
        return this->_buffer;
    }

    size_t getBufferSize() const {
        return this->_size;
    }

    uint32_t calculateChecksum() const;

private:
    unsigned long upperPowerOfTwo(unsigned long v);

private:
    unsigned char * _buffer;
    size_t _capacity;
    size_t _size;
};

#endif /* WEBX_DATA_BUFFER_H */