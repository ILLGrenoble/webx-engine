#ifndef WEBX_BINARY_BUFFER_H
#define WEBX_BINARY_BUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <string.h>
#include <map>

/**
 * WebXBinaryBuffer is the parent class for all custom messages providing automatic serialisation of data that
 * can be sent to/from Host, IO and Clusters and deserialisation on the receiver side. It uses an optimisation
 * of data storage to statically offset member data in custom classes.
 *
 * WebXBinaryBuffer is intended for use with generated classes using the k1bufferc generator. The user write .k1b
 * IDL files containing information on the structure of the messages. The generator creates C++ classes inheriting
 * from the WebXBinaryBuffer.
 *
 * As an optimisation, a receiver simply inserts a raw buffer (unsigned char *) into the object - this buffer is then used
 * as the storage of the member data.
 */
class WebXBinaryBuffer {
public:
	/**
	 * Exception class for unavailable queue.
	 */
	class OverflowException {
		public:
		OverflowException(uint32_t offset, uint32_t dataSize, uint32_t bufferLength) : offset(offset), dataSize(dataSize), bufferLength(bufferLength) {}
		uint32_t offset;
		uint32_t dataSize;
		uint32_t bufferLength;
	};

	/**
	 * Structure of the header of the buffer
	 */
	typedef struct {
		unsigned char sessionId[16];
		uint64_t clientIndexMask;
		uint64_t timestampMs;
		uint32_t messageTypeId;
		uint32_t messageId;
		uint32_t bufferLength;
		char unused[4];
	} MessageHeader;


	/**
	 * Full composition of the message. An endFlag is used at the developers discretion - this is placed at the end of a message
	 * as it is the last sequence of bytes of the message to be transferred. This can be used to indicate if a message has been
	 * handled or not.
	 */
	typedef struct {
		MessageHeader header;
		unsigned char * content;
	} MessageComposition;
	
	WebXBinaryBuffer(unsigned char * buffer, size_t bufferLength, const unsigned char (& sessionId)[16], uint64_t clientIndexMask, uint32_t messageTypeId) :
		_bufferContent(buffer),
		_bufferLength(bufferLength),
		_writeOffset(sizeof(MessageHeader)),
		_readOffset(sizeof(MessageHeader)) {

		++_messageCounter;

		MessageHeader * messageHeader = this->getMessageHeader();
		memcpy(messageHeader->sessionId, sessionId, 16);
		messageHeader->clientIndexMask = clientIndexMask;
		messageHeader->timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		messageHeader->messageTypeId = messageTypeId;
		messageHeader->messageId = _messageCounter;
		messageHeader->bufferLength = bufferLength;
	}

    WebXBinaryBuffer(unsigned char * buffer, size_t bufferLength) :
            _bufferContent(buffer),
            _bufferLength(bufferLength),
            _writeOffset(0),
            _readOffset(0) {
    }

	virtual ~WebXBinaryBuffer() {}

	/**
	 * Returns the message header (placed at the beginning of the buffer).
	 */
	inline MessageHeader * getMessageHeader() const {
		return (MessageHeader *)_bufferContent;
	}

	/**
	 * Returns the type of message buffer
	 */
	inline uint32_t getMessageTypeId() const {
		return ((MessageHeader *)_bufferContent)->messageTypeId;
	}

	/**
	 * Returns the length of the buffer
	 */
	inline uint32_t getByteLength() const {
		return (size_t)((MessageHeader *)_bufferContent)->bufferLength;
	}

	/**
	 * Returns the unique identifier of the message buffer instance.
	 */
	inline uint32_t getMessageId() const {
		return ((MessageHeader *)_bufferContent)->messageId;
	}

	/**
	 * Returns the raw buffer containing all message values (and header)
	 */
	inline const unsigned char * getBuffer() const {
		return _bufferContent;
	}

	/**
	 * Static method to return a MessageHeader from raw data without creating a WebXBinaryBuffer object
	 */
	static inline MessageHeader * getMessageHeader(unsigned char * data) {
		return (MessageHeader *)data;
	}


	/**
	 * Static method to return a MessageHeader from raw data without creating a WebXBinaryBuffer object
	 */
	static inline const MessageHeader * getMessageHeader(const unsigned char * data) {
		return (MessageHeader *)data;
	}


	/**
	 * Static method to return the type of message  contained in serialized buffer data.
	 */
	static inline uint32_t getMessageTypeId(unsigned char * data) {
		return ((WebXBinaryBuffer::MessageHeader *)data)->messageTypeId;
	}

	/**
	 * Static method to return the unique identifier of a message buffer contained in the serialized data.
	 */
	static inline uint32_t getMessageId(unsigned char * data) {
		return ((WebXBinaryBuffer::MessageHeader *)data)->messageId;
	}

	/**
	 * Adds additional payload data at runtime. Allows for dynamic values to be added to buffer payload.
	 *
	 * Note : Values must be added and retrieved in the same order.
	 */
	template<class T>
	inline void write(T value) {
		// Get the offset to place the value
		uint32_t valueOffset = getNextWriteOffset<T>();

		if (valueOffset + sizeof(T) > _bufferLength) {
			throw OverflowException(valueOffset, sizeof(T), _bufferLength);
		}

		// Set value
		*((T *)(&_bufferContent[valueOffset])) = value;
	}

	inline void append(unsigned char * buffer, size_t bufferLength) {
		if (_writeOffset + bufferLength > _bufferLength) {
			throw OverflowException(_writeOffset, bufferLength, _bufferLength);
		}

		memcpy(&_bufferContent[_writeOffset], buffer, bufferLength);
		_writeOffset += bufferLength;
	}

	/**
	 * Gets additional payload data at runtime. Allows for dynamic values to be retreived from buffer payload.
	 *
	 * Note : Values must be added and retrieved in the same order.
	 */
	template<class T>
	inline T read() {
		uint32_t valueOffset = getNextReadOffset<T>();

		if (valueOffset + sizeof(T) > _bufferLength) {
			throw OverflowException(valueOffset, sizeof(T), _bufferLength);
		}

		// Get value
		return *((T *)(&_bufferContent[valueOffset]));
	}

	/**
	 * Resets the dynamic payload data offset
	 */
	inline void resetReadOffset() {
		_readOffset = sizeof(MessageHeader);
	}

    inline void advanceReadOffset(unsigned int increment) {
        _readOffset += increment;
    }


private:
	/**
	 * Template method to update the offset of writing member data in the raw message buffer.
	 */
	template<class T>
	inline uint32_t getNextWriteOffset() {
		// Ensure alignment
		unsigned int sizeOfT = sizeof(T);
		unsigned int padding = (_writeOffset % sizeOfT) > 0 ? sizeOfT - (_writeOffset % sizeOfT) : 0;
		unsigned int position = _writeOffset + padding;

		_writeOffset += sizeOfT + padding;

		return position;
	}

	/**
	 * Template method to update the offset of reading data from the raw message buffer.
	 */
	template<class T>
	inline uint32_t getNextReadOffset() {
		// Ensure alignment
		unsigned int sizeOfT = sizeof(T);
		unsigned int padding = (_readOffset % sizeOfT) > 0 ? sizeOfT - (_readOffset % sizeOfT) : 0;
		unsigned int position = _readOffset + padding;

		_readOffset += sizeOfT + padding;

		return position;
	}


private:
	unsigned char * _bufferContent;
	uint32_t _bufferLength; 
	uint32_t _writeOffset;
	uint32_t _readOffset;

	static uint32_t _messageCounter;
};


#endif /* WEBX_BINARY_BUFFER_H */
