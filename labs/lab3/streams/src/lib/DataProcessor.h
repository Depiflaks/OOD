#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <cstdint>
#include <memory>

class IInputStream;
class IOutputStream;
class IDataProcessor;

using IDataProcessorPtr = std::unique_ptr<IDataProcessor>;

class IDataProcessor
{
public:
	virtual ~IDataProcessor() = default;
	virtual uint8_t ProcessByte(uint8_t data) = 0;
	virtual std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) = 0;
};

#endif /* DATAPROCESSOR_H */
