#include "../src/lib/DataProcessor.h"
#include "../src/lib/InputStream.h"
#include "../src/lib/OutputStream.h"
#include "../src/lib/StreamDecorator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <utility>

namespace operator_shift_tests
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


TEST(OperatorShiftTest, OutputStreamDecoration)
{
	auto mockOutputStream = std::make_unique<MockOutputStream>();
	auto mockProcessor1 = std::make_shared<MockDataProcessor>();
	auto mockProcessor2 = std::make_shared<MockDataProcessor>();

	EXPECT_CALL(*mockProcessor1, ProcessByte(0x88)).Times(1).WillOnce(testing::Return(0x99));
	EXPECT_CALL(*mockProcessor2, ProcessByte(0x77)).Times(1).WillOnce(testing::Return(0x88));
	EXPECT_CALL(*mockOutputStream, WriteByte(0x99)).Times(1);

	auto decorator = std::move(mockOutputStream)
		<< MakeDecorator<OutputStreamDecorator>(std::move(mockProcessor1))
		<< MakeDecorator<OutputStreamDecorator>(std::move(mockProcessor2));

	decorator->WriteByte(0x77);
}

TEST(OperatorShiftTest, InputStreamDecoration)
{
	auto mockInputStream = std::make_unique<MockInputStream>();
	auto mockProcessor1 = std::make_shared<MockDataProcessor>();
	auto mockProcessor2 = std::make_shared<MockDataProcessor>();

	EXPECT_CALL(*mockInputStream, ReadByte()).Times(1).WillOnce(testing::Return(0x11));
	EXPECT_CALL(*mockProcessor1, ProcessByte(0x11)).Times(1).WillOnce(testing::Return(0x22));
	EXPECT_CALL(*mockProcessor2, ProcessByte(0x22)).Times(1).WillOnce(testing::Return(0x33));

	auto decorator = std::move(mockInputStream)
		<< MakeDecorator<InputStreamDecorator>(std::move(mockProcessor1))
		<< MakeDecorator<InputStreamDecorator>(std::move(mockProcessor2));

	uint8_t result = decorator->ReadByte();
	EXPECT_EQ(result, 0x33);
}

} // namespace operator_shift_tests
