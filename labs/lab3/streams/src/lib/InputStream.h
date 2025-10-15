#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>

class IInputStream
{
public:
	virtual ~IInputStream() = default;
	virtual bool IsEOF() = 0;
	virtual uint8_t ReadByte() = 0;
	virtual std::streamsize ReadBlock(void* dstBuffer, std::streamsize size) = 0;
};

class FileInputStream : public IInputStream
{
public:
	FileInputStream(std::unique_ptr<std::ifstream> file)
		: m_file(std::move(file))
	{
	}

	bool IsEOF() override
	{
		return m_file->peek() == EOF;
	}

	uint8_t ReadByte() override
	{
		char byte;
		if (!m_file->get(byte))
		{
			throw std::runtime_error("ReadByte failed");
		}
		return static_cast<uint8_t>(byte);
	}

	std::streamsize ReadBlock(void* dstBuffer, std::streamsize size) override
	{
		if (!m_file->read(static_cast<char*>(dstBuffer), size))
		{
			return m_file->gcount();
		}
		return size;
	}

	void Close()
	{
		if (m_file)
		{
			m_file->close();
		}
	}

private:
	std::unique_ptr<std::ifstream> m_file;
};

class MemoryInputStream : public IInputStream
{
public:
	MemoryInputStream(const uint8_t* data, std::streamsize size)
		: m_data(data)
		, m_size(size)
		, m_position(0)
	{
	}

	bool IsEOF() override
	{
		return m_position >= m_size;
	}

	uint8_t ReadByte() override
	{
		if (m_position >= m_size)
		{
			throw std::runtime_error("ReadByte failed");
		}
		return m_data[m_position++];
	}

	std::streamsize ReadBlock(void* dstBuffer, std::streamsize size) override
	{
		if (m_position >= m_size)
		{
			return 0;
		}

		std::streamsize bytesToRead = std::min(size, m_size - m_position);
		std::memcpy(dstBuffer, m_data + m_position, bytesToRead);
		m_position += bytesToRead;
		return bytesToRead;
	}

private:
	const uint8_t* m_data;
	std::streamsize m_size;
	std::streamsize m_position;
};

#endif /* INPUTSTREAM_H */
