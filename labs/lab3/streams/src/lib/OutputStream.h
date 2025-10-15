#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

class IOutputStream
{
public:
	virtual ~IOutputStream() = default;
	virtual void WriteByte(uint8_t data) = 0;
	virtual void WriteBlock(const void* srcData, std::streamsize size) = 0;
	virtual void Close() = 0;
};

class FileOutputStream : public IOutputStream
{
private:
	std::unique_ptr<std::ofstream> m_file;
	bool m_isClosed;

public:
	FileOutputStream(std::unique_ptr<std::ofstream> file)
		: m_file(std::move(file))
		, m_isClosed(false)
	{
	}

	void WriteByte(uint8_t data) override
	{
		CheckFileNotClosed();
		m_file->put(static_cast<char>(data));
		CheckWriteGood();
	}

	void WriteBlock(const void* srcData, std::streamsize size) override
	{
		CheckFileNotClosed();
		m_file->write(static_cast<const char*>(srcData), size);
		CheckWriteGood();
	}

	void Close() override
	{
		if (m_file)
		{
			m_file->close();
		}
		m_isClosed = true;
	}

private:
	void CheckFileNotClosed()
	{
		if (m_isClosed)
		{
			throw std::runtime_error("Write failed");
		}
	}

	void CheckWriteGood()
	{
		if (!m_file->good())
		{
			throw std::runtime_error("Write failed");
		}
	}
};

class MemoryOutputStream : public IOutputStream
{
private:
	std::vector<uint8_t> m_buffer;
	bool m_isClosed;

public:
	MemoryOutputStream()
		: m_isClosed(false)
	{
	}

	void WriteByte(uint8_t data) override
	{
		CheckFileNotClosed();
		m_buffer.push_back(data);
	}

	void WriteBlock(const void* srcData, std::streamsize size) override
	{
		CheckFileNotClosed();
		const uint8_t* data = static_cast<const uint8_t*>(srcData);
		m_buffer.insert(m_buffer.end(), data, data + size);
	}

	void Close() override
	{
		m_isClosed = true;
	}

	const std::vector<uint8_t>& GetData() const
	{
		return m_buffer;
	}

private:
	void CheckFileNotClosed()
	{
		if (m_isClosed)
		{
			throw std::runtime_error("Write failed");
		}
	}
};
#endif /* OUTPUTSTREAM_H */
