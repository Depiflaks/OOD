#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <algorithm>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

class IDataProcessor;

using IDataProcessorPtr = std::shared_ptr<IDataProcessor>;

class IDataProcessor
{
public:
	virtual ~IDataProcessor() = default;
	virtual uint8_t ProcessByte(uint8_t data) = 0;
	virtual std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) = 0;
};

class DecodingDataProcessor : public IDataProcessor
{
private:
	std::vector<uint8_t> m_decodeTable;
	int m_key;

public:
	DecodingDataProcessor(int key)
		: m_key(key)
	{
		GenerateDecodeTable();
	}

	uint8_t ProcessByte(uint8_t data) override
	{
		return m_decodeTable[data];
	}

	std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) override
	{
		uint8_t* data = static_cast<uint8_t*>(buffer);
		for (std::streamsize i = 0; i < size; ++i)
		{
			data[i] = m_decodeTable[data[i]];
		}
		return size;
	}

private:
	void GenerateDecodeTable()
	{
		std::vector<uint8_t> encodeTable(256);
		for (int i = 0; i < 256; ++i)
		{
			encodeTable[i] = static_cast<uint8_t>(i);
		}

		std::mt19937 gen(m_key);
		std::shuffle(encodeTable.begin(), encodeTable.end(), gen);

		m_decodeTable.resize(256);
		for (int i = 0; i < 256; ++i)
		{
			m_decodeTable[encodeTable[i]] = static_cast<uint8_t>(i);
		}
	}
};

class EncodingDataProcessor : public IDataProcessor
{
private:
	std::vector<uint8_t> m_encodeTable;
	int m_key;

public:
	EncodingDataProcessor(int key)
		: m_key(key)
	{
		GenerateEncodeTable();
	}

	uint8_t ProcessByte(uint8_t data) override
	{
		return m_encodeTable[data];
	}

	std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) override
	{
		uint8_t* data = static_cast<uint8_t*>(buffer);
		for (std::streamsize i = 0; i < size; ++i)
		{
			data[i] = m_encodeTable[data[i]];
		}
		return size;
	}

private:
	void GenerateEncodeTable()
	{
		m_encodeTable.resize(256);
		for (int i = 0; i < 256; ++i)
		{
			m_encodeTable[i] = static_cast<uint8_t>(i);
		}

		std::mt19937 gen(m_key);
		std::shuffle(m_encodeTable.begin(), m_encodeTable.end(), gen);
	}
};
;

#endif /* DATAPROCESSOR_H */
