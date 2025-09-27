#include "Observer.h"
#include "Subject.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

using ::testing::_;

namespace observerTests
{

class MockObserver : public AbstractObserver<int>
{
public:
	MockObserver(const IObservable<int>& observable)
		: AbstractObserver<int>(observable)
	{
	}

	MOCK_METHOD(void, Update, (const IObservable<int>&), (override));
};

class MockMonoObserver : public MonoAbstractObserver<int>
{
public:
	MockMonoObserver(const IObservable<int>& observable)
		: MonoAbstractObserver<int>(observable)
	{
	}

	MOCK_METHOD(void, UpdateObserver, (const IObservable<int>& observable), (override));
};

class MockDuoObserver : public DuoAbstractObserver<int, std::string>
{
public:
	MockDuoObserver(const IObservable<int>& first, const IObservable<std::string>& second)
		: DuoAbstractObserver<int, std::string>(first, second)
	{
	}

	MOCK_METHOD(void, UpdateFirst, (const IObservable<int>& observable), (override));
	MOCK_METHOD(void, UpdateSecond, (const IObservable<std::string>& observable), (override));
};

class MockIntSubject : public Subject<int>
{
public:
	MockIntSubject(int data)
		: data(data)
	{
	}

	int GetData() const override
	{
		return data;
	}

	void SetData(int newData, SubscribeType type = SubscribeType::TEMPERATURE_CHANGE)
	{
		data = newData;
		Notify(type);
	}

private:
	int data;
};

class MockStringSubject : public Subject<std::string>
{
public:
	MockStringSubject(const std::string& data)
		: data(data)
	{
	}

	std::string GetData() const override { return data; }

	void SetData(const std::string& newData, SubscribeType type = SubscribeType::TEMPERATURE_CHANGE)
	{
		data = newData;
		Notify(type);
	}

private:
	std::string data;
};

const SubscribeType DEFAULT_TYPE = SubscribeType::TEMPERATURE_CHANGE;

TEST(ObserverTest, MultipleObserversGetNotified)
{
	auto subject = std::make_shared<MockIntSubject>(42);

	auto observer1 = std::make_shared<MockObserver>(*subject);
	auto observer2 = std::make_shared<MockObserver>(*subject);
	auto observer3 = std::make_shared<MockObserver>(*subject);

	subject->Subscribe(DEFAULT_TYPE, observer1);
	subject->Subscribe(DEFAULT_TYPE, observer2);
	subject->Subscribe(DEFAULT_TYPE, observer3);

	EXPECT_CALL(*observer1, Update(_)).Times(3);
	EXPECT_CALL(*observer2, Update(_)).Times(3);
	EXPECT_CALL(*observer3, Update(_)).Times(3);

	subject->Notify(DEFAULT_TYPE);
	subject->Notify(DEFAULT_TYPE);
	subject->Notify(DEFAULT_TYPE);
}

TEST(MonoObserverTest, OnlySubscribedObservableTriggersUpdate)
{
	auto subject1 = std::make_shared<MockIntSubject>(1);
	auto subject2 = std::make_shared<MockIntSubject>(2);

	auto observer = std::make_shared<MockMonoObserver>(*subject1);

	testing::StrictMock<MockMonoObserver>* mockObserver = static_cast<testing::StrictMock<MockMonoObserver>*>(observer.get());

	EXPECT_CALL(*mockObserver, UpdateObserver(testing::Ref(*subject1))).Times(1);

	subject1->Subscribe(DEFAULT_TYPE, observer);
	subject2->Subscribe(DEFAULT_TYPE, observer);

	subject1->Notify(DEFAULT_TYPE);
	subject2->Notify(DEFAULT_TYPE);
}

TEST(DuoObserverTest, BothSubjectsTriggerCorrectUpdates)
{
	auto intSubject = std::make_shared<MockIntSubject>(42);
	auto stringSubject = std::make_shared<MockStringSubject>("test");

	auto observer = std::make_shared<MockDuoObserver>(*intSubject, *stringSubject);

	testing::StrictMock<MockDuoObserver>* mockObserver = static_cast<testing::StrictMock<MockDuoObserver>*>(observer.get());

	testing::Sequence seq;
	EXPECT_CALL(*mockObserver, UpdateFirst(testing::Ref(*intSubject))).InSequence(seq);
	EXPECT_CALL(*mockObserver, UpdateSecond(testing::Ref(*stringSubject))).InSequence(seq);

	intSubject->Subscribe(DEFAULT_TYPE, observer);
	stringSubject->Subscribe(DEFAULT_TYPE, observer);

	intSubject->Notify(DEFAULT_TYPE);
	stringSubject->Notify(DEFAULT_TYPE);
}

TEST(DuoObserverTest, ChangeObservablesUpdatesSubscription)
{
	auto intSubject1 = std::make_shared<MockIntSubject>(1);
	auto intSubject2 = std::make_shared<MockIntSubject>(2);
	auto stringSubject1 = std::make_shared<MockStringSubject>("first");
	auto stringSubject2 = std::make_shared<MockStringSubject>("second");

	auto observer = std::make_shared<MockDuoObserver>(*intSubject1, *stringSubject1);

	testing::StrictMock<MockDuoObserver>* mockObserver = static_cast<testing::StrictMock<MockDuoObserver>*>(observer.get());

	observer->ChangeObservables(*intSubject2, *stringSubject2);

	EXPECT_CALL(*mockObserver, UpdateFirst(testing::Ref(*intSubject2))).Times(1);
	EXPECT_CALL(*mockObserver, UpdateSecond(testing::Ref(*stringSubject2))).Times(1);

	intSubject2->Subscribe(DEFAULT_TYPE, observer);
	stringSubject2->Subscribe(DEFAULT_TYPE, observer);

	intSubject2->Notify(DEFAULT_TYPE);
	stringSubject2->Notify(DEFAULT_TYPE);
}

TEST(DuoObserverTest, SurvivingSubjectWorksAfterOtherDestroyed)
{
	auto intSubject = std::make_shared<MockIntSubject>(42);
	auto stringSubject = std::make_shared<MockStringSubject>("test");

	auto observer = std::make_shared<MockDuoObserver>(*intSubject, *stringSubject);

	testing::StrictMock<MockDuoObserver>* mockObserver = static_cast<testing::StrictMock<MockDuoObserver>*>(observer.get());

	intSubject->Subscribe(DEFAULT_TYPE, observer);
	stringSubject->Subscribe(DEFAULT_TYPE, observer);

	stringSubject.reset();

	EXPECT_CALL(*mockObserver, UpdateFirst(testing::Ref(*intSubject))).Times(1);

	intSubject->Notify(DEFAULT_TYPE);
}

TEST(DuoObserverTest, OnlySubscribedSubjectsTriggerUpdates)
{
	auto intSubject1 = std::make_shared<MockIntSubject>(1);
	auto intSubject2 = std::make_shared<MockIntSubject>(2);
	auto stringSubject1 = std::make_shared<MockStringSubject>("first");
	auto stringSubject2 = std::make_shared<MockStringSubject>("second");
	auto stringSubject3 = std::make_shared<MockStringSubject>("third");

	auto observer = std::make_shared<MockDuoObserver>(*intSubject1, *stringSubject1);

	testing::StrictMock<MockDuoObserver>* mockObserver = static_cast<testing::StrictMock<MockDuoObserver>*>(observer.get());

	EXPECT_CALL(*mockObserver, UpdateFirst(testing::Ref(*intSubject1))).Times(1);
	EXPECT_CALL(*mockObserver, UpdateSecond(testing::Ref(*stringSubject1))).Times(1);

	observer->Update(*intSubject1);
	observer->Update(*stringSubject1);
	observer->Update(*stringSubject2);
}

TEST(DuoObserverTest, DifferentSubscribeTypesForDifferentSubjects)
{
	auto intSubject = std::make_shared<MockIntSubject>(42);
	auto stringSubject = std::make_shared<MockStringSubject>("test");

	auto observer = std::make_shared<MockDuoObserver>(*intSubject, *stringSubject);

	testing::StrictMock<MockDuoObserver>* mockObserver = static_cast<testing::StrictMock<MockDuoObserver>*>(observer.get());

	EXPECT_CALL(*mockObserver, UpdateFirst(testing::Ref(*intSubject))).Times(1);
	EXPECT_CALL(*mockObserver, UpdateSecond(testing::Ref(*stringSubject))).Times(1);

	intSubject->Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer);
	stringSubject->Subscribe(SubscribeType::HUMIDITY_CHANGE, observer);

	intSubject->Notify(SubscribeType::TEMPERATURE_CHANGE);
	stringSubject->Notify(SubscribeType::HUMIDITY_CHANGE);
}

TEST(MonoObserverTest, UnsubscribeFromSpecificType)
{
	auto subject = std::make_shared<MockIntSubject>(42);

	auto observer = std::make_shared<MockMonoObserver>(*subject);

	testing::StrictMock<MockMonoObserver>* mockObserver = static_cast<testing::StrictMock<MockMonoObserver>*>(observer.get());

	EXPECT_CALL(*mockObserver, UpdateObserver(testing::Ref(*subject))).Times(1);

	subject->Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer);
	subject->Subscribe(SubscribeType::HUMIDITY_CHANGE, observer);

	subject->Unsubscribe(SubscribeType::HUMIDITY_CHANGE, observer);

	subject->Notify(SubscribeType::TEMPERATURE_CHANGE);
	subject->Notify(SubscribeType::HUMIDITY_CHANGE);
}
} // namespace observerTests
