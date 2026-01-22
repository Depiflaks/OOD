#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <algorithm>
#include <cstdint>
#include <cstring>
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

class PackingDataProcessor : public IDataProcessor
{
public:
	uint8_t ProcessByte(uint8_t data) override
	{
		return data;
	}

	std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) override
	{
		if (size == 0)
		{
			return 0;
		}

		auto* data = static_cast<uint8_t*>(buffer);
		std::vector<uint8_t> compressedData;
		compressedData.reserve(static_cast<size_t>(size));

		for (std::streamsize i = 0; i < size;)
		{
			uint8_t value = data[i];
			std::streamsize count = 1;

			while (i + count < size && data[i + count] == value && count < 255)
			{
				++count;
			}

			// Encode runs longer than 3 or any 0xFF bytes (since 0xFF is our marker)
			if (count > 3 || value == 0xFF)
			{
				compressedData.push_back(0xFF);
				compressedData.push_back(value);
				compressedData.push_back(static_cast<uint8_t>(count));
			}
			else
			{
				for (std::streamsize j = 0; j < count; ++j)
				{
					compressedData.push_back(value);
				}
			}

			i += count;
		}

		// Only use compressed form if it's actually smaller than original.
		if (compressedData.size() < static_cast<size_t>(size))
		{
			std::memcpy(buffer, compressedData.data(), compressedData.size());
			return static_cast<std::streamsize>(compressedData.size());
		}
		else
		{
			// Keep original uncompressed data in buffer, signal original size.
			return size;
		}
	}
};

class UnpackingDataProcessor : public IDataProcessor
{
public:
	uint8_t ProcessByte(uint8_t data) override
	{
		return data;
	}

	std::streamsize ProcessDataBlock(void* buffer, std::streamsize size) override
	{
		if (size == 0)
		{
			return 0;
		}

		auto* input = static_cast<uint8_t*>(buffer);
		std::vector<uint8_t> decompressed;
		decompressed.reserve(static_cast<size_t>(size) * 2);

		std::streamsize i = 0;
		while (i < size)
		{
			if (input[i] == 0xFF)
			{
				// If there's not enough bytes for a marker (malformed/truncated),
				// treat 0xFF as a literal byte.
				if (i + 2 >= size)
				{
					decompressed.push_back(0xFF);
					++i;
				}
				else
				{
					uint8_t value = input[i + 1];
					uint8_t count = input[i + 2];
					decompressed.insert(decompressed.end(), count, value);
					i += 3;
				}
			}
			else
			{
				decompressed.push_back(input[i]);
				++i;
			}
		}

		// Copy decompressed data back into buffer (caller is expected to provide
		// sufficient capacity).
		std::memcpy(buffer, decompressed.data(), decompressed.size());
		return static_cast<std::streamsize>(decompressed.size());
	}
};

#endif /* DATAPROCESSOR_H */
