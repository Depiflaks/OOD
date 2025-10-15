#include "../src/lib/DataProcessor.h"
#include "../src/lib/InputStream.h"
#include "../src/lib/OutputStream.h"
#include "../src/lib/StreamDecorator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockInputStream : public IInputStream
{
public:
	MOCK_METHOD(bool, IsEOF, (), (override));
	MOCK_METHOD(uint8_t, ReadByte, (), (override));
	MOCK_METHOD(std::streamsize, ReadBlock, (void*, std::streamsize), (override));
};

class MockOutputStream : public IOutputStream
{
public:
	MOCK_METHOD(void, WriteByte, (uint8_t), (override));
	MOCK_METHOD(void, WriteBlock, (const void*, std::streamsize), (override));
	MOCK_METHOD(void, Close, (), (override));
};

class MockDataProcessor : public IDataProcessor
{
public:
	MOCK_METHOD(uint8_t, ProcessByte, (uint8_t), (override));
	MOCK_METHOD(std::streamsize, ProcessDataBlock, (void*, std::streamsize), (override));
};

TEST(InputStreamDecoratorTest, NoDecoration)
{
	auto mockInputStream = std::make_unique<MockInputStream>();
	auto mockProcessor = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockInputStream, ReadByte()).Times(1).WillOnce(testing::Return(0xAA));
	EXPECT_CALL(*mockInputStream, ReadBlock(nullptr, 10)).Times(1).WillOnce(testing::Return(5));

	EXPECT_CALL(*mockProcessor, ProcessByte(0xAA)).Times(1).WillOnce(testing::Return(0xBB));
	EXPECT_CALL(*mockProcessor, ProcessDataBlock(nullptr, 5)).Times(1).WillOnce(testing::Return(3));

	InputStreamDecorator decorator(std::move(mockInputStream), std::move(mockProcessor));

	uint8_t result = decorator.ReadByte();
	EXPECT_EQ(result, 0xBB);

	std::streamsize blockResult = decorator.ReadBlock(nullptr, 10);
	EXPECT_EQ(blockResult, 3);
}

TEST(InputStreamDecoratorTest, SingleDecoration)
{
	auto mockInputStream = std::make_unique<MockInputStream>();
	auto mockProcessor1 = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockInputStream, ReadByte()).Times(1).WillOnce(testing::Return(0x11));
	EXPECT_CALL(*mockProcessor1, ProcessByte(0x11)).Times(1).WillOnce(testing::Return(0x22));

	InputStreamDecorator decorator1(std::move(mockInputStream), std::move(mockProcessor1));

	uint8_t result = decorator1.ReadByte();
	EXPECT_EQ(result, 0x22);
}

TEST(InputStreamDecoratorTest, MultipleDecoration)
{
	auto mockInputStream = std::make_unique<MockInputStream>();
	auto mockProcessor1 = std::make_unique<MockDataProcessor>();
	auto mockProcessor2 = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockInputStream, ReadByte()).Times(1).WillOnce(testing::Return(0x11));
	EXPECT_CALL(*mockProcessor1, ProcessByte(0x11)).Times(1).WillOnce(testing::Return(0x22));
	EXPECT_CALL(*mockProcessor2, ProcessByte(0x22)).Times(1).WillOnce(testing::Return(0x33));

	auto firstDecorator = std::make_unique<InputStreamDecorator>(std::move(mockInputStream), std::move(mockProcessor1));
	InputStreamDecorator secondDecorator(std::move(firstDecorator), std::move(mockProcessor2));

	uint8_t result = secondDecorator.ReadByte();
	EXPECT_EQ(result, 0x33);
}

TEST(OutputStreamDecoratorTest, NoDecoration)
{
	auto mockOutputStream = std::make_unique<MockOutputStream>();
	auto mockProcessor = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockProcessor, ProcessByte(0xAA)).Times(1).WillOnce(testing::Return(0xBB));
	EXPECT_CALL(*mockOutputStream, WriteByte(0xBB)).Times(1);

	EXPECT_CALL(*mockProcessor, ProcessDataBlock(testing::_, 4)).Times(1).WillOnce(testing::Return(4));
	EXPECT_CALL(*mockOutputStream, WriteBlock(testing::_, 4)).Times(1);

	OutputStreamDecorator decorator(std::move(mockOutputStream), std::move(mockProcessor));

	decorator.WriteByte(0xAA);

	uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
	decorator.WriteBlock(data, 4);
}

TEST(OutputStreamDecoratorTest, SingleDecoration)
{
	auto mockOutputStream = std::make_unique<MockOutputStream>();
	auto mockProcessor = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockProcessor, ProcessByte(0x44)).Times(1).WillOnce(testing::Return(0x55));
	EXPECT_CALL(*mockOutputStream, WriteByte(0x55)).Times(1);

	OutputStreamDecorator decorator(std::move(mockOutputStream), std::move(mockProcessor));

	decorator.WriteByte(0x44);
}

TEST(OutputStreamDecoratorTest, MultipleDecoration)
{
	auto mockOutputStream = std::make_unique<MockOutputStream>();
	auto mockProcessor1 = std::make_unique<MockDataProcessor>();
	auto mockProcessor2 = std::make_unique<MockDataProcessor>();

	EXPECT_CALL(*mockProcessor1, ProcessByte(0x88)).Times(1).WillOnce(testing::Return(0x99));
    EXPECT_CALL(*mockProcessor2, ProcessByte(0x77)).Times(1).WillOnce(testing::Return(0x88));
	EXPECT_CALL(*mockOutputStream, WriteByte(0x99)).Times(1);

	auto firstDecorator = std::make_unique<OutputStreamDecorator>(std::move(mockOutputStream), std::move(mockProcessor1));
	auto secondDecorator = std::make_unique<OutputStreamDecorator>(std::move(firstDecorator), std::move(mockProcessor2));

	secondDecorator->WriteByte(0x77);
}
