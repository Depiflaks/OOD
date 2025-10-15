#ifndef STREAMDECORATOR_H
#define STREAMDECORATOR_H

#include "DataProcessor.h"
#include "InputStream.h"
#include "OutputStream.h"


using IInputStreamPtr = std::unique_ptr<IInputStream>;
using IOutputStreamPtr = std::unique_ptr<IOutputStream>;

class InputStreamDecorator : public IInputStream
{
protected:
	IInputStreamPtr m_inputStream;
	IDataProcessorPtr m_dataProcessor;

public:
	InputStreamDecorator(IInputStreamPtr inputStream, IDataProcessorPtr dataProcessor)
		: m_inputStream(std::move(inputStream))
		, m_dataProcessor(std::move(dataProcessor))
	{
	}

	bool IsEOF() override
	{
		return m_inputStream->IsEOF();
	}

	uint8_t ReadByte() override
	{
		uint8_t data = m_inputStream->ReadByte();
		return m_dataProcessor->ProcessByte(data);
	}

	std::streamsize ReadBlock(void* dstBuffer, std::streamsize size) override
	{
		std::streamsize bytesRead = m_inputStream->ReadBlock(dstBuffer, size);
		return m_dataProcessor->ProcessDataBlock(dstBuffer, bytesRead);
	}
};

class OutputStreamDecorator : public IOutputStream
{
protected:
	IOutputStreamPtr m_outputStream;
	IDataProcessorPtr m_dataProcessor;

public:
	OutputStreamDecorator(IOutputStreamPtr outputStream, IDataProcessorPtr dataProcessor)
		: m_outputStream(std::move(outputStream))
		, m_dataProcessor(std::move(dataProcessor))
	{
	}

	void WriteByte(uint8_t data) override
	{
		uint8_t processed = m_dataProcessor->ProcessByte(data);
		m_outputStream->WriteByte(processed);
	}

	void WriteBlock(const void* srcData, std::streamsize size) override
	{
		std::vector<uint8_t> buffer(size);
		std::memcpy(buffer.data(), srcData, size);
		std::streamsize processedSize = m_dataProcessor->ProcessDataBlock(buffer.data(), size);
		m_outputStream->WriteBlock(buffer.data(), processedSize);
	}

	void Close() override
	{
		m_outputStream->Close();
	}
};

#endif /* STREAMDECORATOR_H */
