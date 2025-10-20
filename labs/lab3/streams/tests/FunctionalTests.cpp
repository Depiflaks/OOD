#include "../src/lib/InputStream.h"
#include "../src/lib/OutputStream.h"
#include "../src/lib/StreamDecorator.h"
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace functional_tests
{

template <typename Decorator, typename... Args>
auto MakeDecorator(const Args&... args)
{
	return [&](auto&& object) {
		return std::make_unique<Decorator>(std::forward<decltype(object)>(object), args...);
	};
}

template <typename Component, typename Decorator>
auto operator<<(Component&& component, const Decorator& decorate)
{
	return decorate(std::forward<Component>(component));
}

class FileTransformerTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		std::filesystem::create_directory("test_files");
	}

	void TearDown() override
	{
		std::filesystem::remove_all("test_files");
	}

	void CreateTestFile(const std::string& filename, const std::vector<uint8_t>& data)
	{
		std::ofstream file(filename, std::ios::binary);
		file.write(reinterpret_cast<const char*>(data.data()), data.size());
		file.close();
	}

	std::vector<uint8_t> ReadTestFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary);
		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<uint8_t> data(size);
		file.read(reinterpret_cast<char*>(data.data()), size);
		file.close();
		return data;
	}
};

TEST_F(FileTransformerTest, BasicInputWorks)
{
	std::vector<uint8_t> testData = { 0x01, 0x02, 0x03, 0x04 };
	CreateTestFile("test_files/input1.bin", testData);

	auto inputFile = std::make_unique<std::ifstream>("test_files/input1.bin", std::ios::binary);
	auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));

	std::vector<uint8_t> buffer(testData.size());
	std::streamsize bytesRead = inputStream->ReadBlock(buffer.data(), buffer.size());

	EXPECT_EQ(bytesRead, testData.size());
	EXPECT_EQ(buffer, testData);
	EXPECT_TRUE(inputStream->IsEOF());

	inputStream->Close();
}

TEST_F(FileTransformerTest, BasicOutputWorks)
{
	std::vector<uint8_t> testData = { 0x05, 0x06, 0x07, 0x08 };

	auto outputFile = std::make_unique<std::ofstream>("test_files/output1.bin", std::ios::binary);
	auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

	outputStream->WriteBlock(testData.data(), testData.size());
	outputStream->Close();

	auto result = ReadTestFile("test_files/output1.bin");
	EXPECT_EQ(result, testData);
}

TEST_F(FileTransformerTest, InputWithDecodingWorks)
{
	std::vector<uint8_t> testData = { 0x01, 0x02, 0x03, 0x04 };
	CreateTestFile("test_files/input2.bin", testData);

	auto inputFile = std::make_unique<std::ifstream>("test_files/input2.bin", std::ios::binary);
	auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));

	auto decodedStream = std::move(inputStream) << MakeDecorator<DecodingInputStreamDecorator>(123);

	std::vector<uint8_t> buffer(testData.size());
	EXPECT_NO_THROW(std::streamsize bytesRead = decodedStream->ReadBlock(buffer.data(), buffer.size()));
}

TEST_F(FileTransformerTest, OutputWithEncodingWorks)
{
	std::vector<uint8_t> testData = { 0x0A, 0x0B, 0x0C, 0x0D };

	auto outputFile = std::make_unique<std::ofstream>("test_files/output2.bin", std::ios::binary);
	auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

	auto encodedStream = std::move(outputStream) << MakeDecorator<EncodingOutputStreamDecorator>(456);

	encodedStream->WriteBlock(testData.data(), testData.size());
	encodedStream->Close();

	EXPECT_TRUE(std::filesystem::exists("test_files/output2.bin"));
}

TEST_F(FileTransformerTest, InputWithDoubleDecodingWorks)
{
	std::vector<uint8_t> testData = { 0x11, 0x12, 0x13, 0x14 };
	CreateTestFile("test_files/input3.bin", testData);

	auto inputFile = std::make_unique<std::ifstream>("test_files/input3.bin", std::ios::binary);
	auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));

	auto doubleDecodedStream = std::move(inputStream)
		<< MakeDecorator<DecodingInputStreamDecorator>(111)
		<< MakeDecorator<DecodingInputStreamDecorator>(222);

	std::vector<uint8_t> buffer(testData.size());
	EXPECT_NO_THROW(doubleDecodedStream->ReadBlock(buffer.data(), buffer.size()));
}

TEST_F(FileTransformerTest, OutputWithDoubleEncodingWorks)
{
	std::vector<uint8_t> testData = { 0x15, 0x16, 0x17, 0x18 };

	auto outputFile = std::make_unique<std::ofstream>("test_files/output3.bin", std::ios::binary);
	auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

	auto doubleEncodedStream = std::move(outputStream)
		<< MakeDecorator<EncodingOutputStreamDecorator>(333)
		<< MakeDecorator<EncodingOutputStreamDecorator>(444);

	EXPECT_NO_THROW(doubleEncodedStream->WriteBlock(testData.data(), testData.size()));
	doubleEncodedStream->Close();
}

TEST_F(FileTransformerTest, InputWithDecodingAndUnpackingWorks)
{
	std::vector<uint8_t> testData = { 0x21, 0x22, 0x23, 0x24 };
	CreateTestFile("test_files/input4.bin", testData);

	auto inputFile = std::make_unique<std::ifstream>("test_files/input4.bin", std::ios::binary);
	auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));

	auto processedStream = std::move(inputStream)
		<< MakeDecorator<DecodingInputStreamDecorator>(555)
		<< MakeDecorator<UnpackingInputStreamDecorator>();

	std::vector<uint8_t> buffer(testData.size());
	EXPECT_NO_THROW(processedStream->ReadBlock(buffer.data(), buffer.size()));
}

TEST_F(FileTransformerTest, OutputWithPackingAndEncodingWorks)
{
	std::vector<uint8_t> testData = { 0x25, 0x26, 0x27, 0x28, 0x28, 0x28 };

	auto outputFile = std::make_unique<std::ofstream>("test_files/output4.bin", std::ios::binary);
	auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

	auto processedStream = std::move(outputStream)
		<< MakeDecorator<PackingOutputStreamDecorator>()
		<< MakeDecorator<EncodingOutputStreamDecorator>(666);

	EXPECT_NO_THROW(processedStream->WriteBlock(testData.data(), testData.size()));
	processedStream->Close();
}

TEST_F(FileTransformerTest, InputWithTripleProcessingWorks)
{
	std::vector<uint8_t> testData = { 0x31, 0x32, 0x33, 0x34 };
	CreateTestFile("test_files/input5.bin", testData);

	auto inputFile = std::make_unique<std::ifstream>("test_files/input5.bin", std::ios::binary);
	auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));

	auto tripleProcessedStream = std::move(inputStream)
		<< MakeDecorator<DecodingInputStreamDecorator>(777)
		<< MakeDecorator<UnpackingInputStreamDecorator>()
		<< MakeDecorator<DecodingInputStreamDecorator>(888);

	std::vector<uint8_t> buffer(testData.size());
	EXPECT_NO_THROW(tripleProcessedStream->ReadBlock(buffer.data(), buffer.size()));
}

TEST_F(FileTransformerTest, OutputWithTripleProcessingWorks)
{
	std::vector<uint8_t> testData = { 0x35, 0x36, 0x37, 0x38, 0x38 };

	auto outputFile = std::make_unique<std::ofstream>("test_files/output5.bin", std::ios::binary);
	auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));

	auto tripleProcessedStream = std::move(outputStream)
		<< MakeDecorator<EncodingOutputStreamDecorator>(999)
		<< MakeDecorator<PackingOutputStreamDecorator>()
		<< MakeDecorator<EncodingOutputStreamDecorator>(1010);

	EXPECT_NO_THROW(tripleProcessedStream->WriteBlock(testData.data(), testData.size()));
	tripleProcessedStream->Close();
}

TEST_F(FileTransformerTest, CompressionDecompressionRestoresOriginalData)
{
	std::vector<uint8_t> originalData = { 0x41, 0x41, 0x41, 0x42, 0x42, 0x43 };
	CreateTestFile("test_files/original.bin", originalData);

	{
		auto outputFile = std::make_unique<std::ofstream>("test_files/compressed.bin", std::ios::binary);
		auto outputStream = std::make_unique<FileOutputStream>(std::move(outputFile));
		auto compressedStream = std::move(outputStream) << MakeDecorator<PackingOutputStreamDecorator>();
		compressedStream->WriteBlock(originalData.data(), originalData.size());
	}

	{
		auto inputFile = std::make_unique<std::ifstream>("test_files/compressed.bin", std::ios::binary);
		auto inputStream = std::make_unique<FileInputStream>(std::move(inputFile));
		auto decompressedStream = std::move(inputStream) << MakeDecorator<UnpackingInputStreamDecorator>();

		std::vector<uint8_t> result(originalData.size());
		std::streamsize bytesRead = decompressedStream->ReadBlock(result.data(), result.size());
		EXPECT_EQ(bytesRead, originalData.size());
		EXPECT_EQ(result, originalData);
	}
}
} // namespace functional_tests
