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

        uint8_t* data = static_cast<uint8_t*>(buffer);
        std::vector<uint8_t> compressedData;

        for (std::streamsize i = 0; i < size;)
        {
            uint8_t value = data[i];
            uint8_t count = 1;
            
            while (i + count < size && data[i + count] == value && count < 255)
            {
                count++;
            }

            if (count > 3 || value == 0xFF)
            {
                compressedData.push_back(0xFF);
                compressedData.push_back(value);
                compressedData.push_back(count);
            }
            else
            {
                for (int j = 0; j < count; j++)
                {
                    compressedData.push_back(value);
                }
            }
            
            i += count;
        }

        if (compressedData.size() <= static_cast<size_t>(size))
        {
            std::memcpy(buffer, compressedData.data(), compressedData.size());
            return compressedData.size();
        }
        else
        {
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
        uint8_t* input = static_cast<uint8_t*>(buffer);
        std::vector<uint8_t> decompressed;

        std::streamsize i = 0;
        while (i < size)
        {
            if (input[i] == 0xFF && i + 2 < size)
            {
                uint8_t value = input[i + 1];
                uint8_t count = input[i + 2];
                for (int j = 0; j < count; j++)
                {
                    decompressed.push_back(value);
                }
                i += 3;
            }
            else
            {
                decompressed.push_back(input[i]);
                i++;
            }
        }

        if (decompressed.size() <= static_cast<size_t>(size))
        {
            std::memcpy(buffer, decompressed.data(), decompressed.size());
            return decompressed.size();
        }
        else
        {
            return size;
        }
    }
};

#endif /* DATAPROCESSOR_H */
