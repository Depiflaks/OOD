#include "Observer.h"
#include "Subject.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

using ::testing::_;

class MockObserver : public AbstractObserver<int>
{
public:
	MockObserver(const IObservable<int>& observable)
		: AbstractObserver<int>(observable)
	{
	}

	MOCK_METHOD(void, Update, (const IObservable<int>&), (override));
};

class MockSubject : public Subject<int>
{
public:
	MockSubject(int data)
		: data(data)
	{
	}

	int GetData() const override
	{
		return data;
	}

	void SetData(int newData)
	{
		data = newData;
	}

private:
	int data;
};

TEST(ObserverTest, MultipleObserversGetNotified)
{
	auto subject = std::make_shared<MockSubject>(42);

	auto observer1 = std::make_shared<MockObserver>(*subject);
	auto observer2 = std::make_shared<MockObserver>(*subject);
	auto observer3 = std::make_shared<MockObserver>(*subject);

	subject->RegisterObserver(observer1);
	subject->RegisterObserver(observer2);
	subject->RegisterObserver(observer3);

	EXPECT_CALL(*observer1, Update(_)).Times(3);
	EXPECT_CALL(*observer2, Update(_)).Times(3);
	EXPECT_CALL(*observer3, Update(_)).Times(3);

	subject->NotifyObservers();
	subject->NotifyObservers();
	subject->NotifyObservers();

}
