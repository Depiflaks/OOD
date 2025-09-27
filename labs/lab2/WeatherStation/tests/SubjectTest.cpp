
#include "Observer.h"
#include "Subject.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

using ::testing::_;

namespace subjectTests
{

class MockObserver : public IObserver<int>
{
public:
	MOCK_METHOD(void, Update, (const IObservable<int>&), (override));
};

class MockSubject : public Subject<int>
{
public:
	MOCK_METHOD(int, GetData, (), (const, override));
};

TEST(SubjectTest, SingleObserver)
{
	MockSubject subject;
	auto observer = std::make_shared<MockObserver>();

	EXPECT_CALL(*observer, Update(_)).Times(1);

	subject.RegisterObserver(observer);
	subject.NotifyObservers();
}

TEST(SubjectTest, TwoObservers)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();

	EXPECT_CALL(*observer1, Update(_)).Times(1);
	EXPECT_CALL(*observer2, Update(_)).Times(1);

	subject.RegisterObserver(observer1);
	subject.RegisterObserver(observer2);
	subject.NotifyObservers();
}

TEST(SubjectTest, RemoveObserver)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();

	EXPECT_CALL(*observer1, Update(_)).Times(1);
	EXPECT_CALL(*observer2, Update(_)).Times(0);

	subject.RegisterObserver(observer1);
	subject.RegisterObserver(observer2);
	subject.RemoveObserver(observer2);
	subject.NotifyObservers();
}

TEST(SubjectTest, ObserverLifetime)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();

	EXPECT_CALL(*observer1, Update(_)).Times(1);

	subject.RegisterObserver(observer1);

	{
		auto observer2 = std::make_shared<MockObserver>();
		subject.RegisterObserver(observer2);
	}

	subject.NotifyObservers();
}

TEST(SubjectTest, PriorityOrderBasic)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

	subject.RegisterObserver(observer1, 1);
	subject.RegisterObserver(observer2, 2);
	subject.NotifyObservers();
}

TEST(SubjectTest, PriorityOrderReverseRegistration)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

	subject.RegisterObserver(observer2, 2);
	subject.RegisterObserver(observer1, 1);
	subject.NotifyObservers();
}

TEST(SubjectTest, SamePrioritySequentialExecution)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();
	auto observer3 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer3, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

	subject.RegisterObserver(observer1, 1);
	subject.RegisterObserver(observer3, 1);
	subject.RegisterObserver(observer2, 1);
    // TODO: приоритеты перепролдумать
	subject.NotifyObservers();
}

TEST(SubjectTest, ChangePriority)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();
	auto observer3 = std::make_shared<MockObserver>();

	subject.RegisterObserver(observer1, 1);
	subject.RegisterObserver(observer2, 2);
	subject.RegisterObserver(observer3, 3);

	subject.RemoveObserver(observer2);
	subject.RegisterObserver(observer2, 0);

	testing::Sequence seq;
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer3, Update(_)).InSequence(seq);

	subject.NotifyObservers();
}

TEST(SubjectTest, DublicatedSubscribe)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();
	auto observer3 = std::make_shared<MockObserver>();

	subject.RegisterObserver(observer1, 1);
	subject.RegisterObserver(observer2, 2);
	subject.RegisterObserver(observer3, 3);

	subject.RegisterObserver(observer2, 0);

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer3, Update(_)).InSequence(seq);

	subject.NotifyObservers();
}

TEST(SubjectTest, MiddleObserverDestroyed)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer3 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer3, Update(_)).InSequence(seq);

	subject.RegisterObserver(observer1, 1);
	{
		auto observer2 = std::make_shared<MockObserver>();
		subject.RegisterObserver(observer2, 2);
	}
	subject.RegisterObserver(observer3, 3);
	subject.NotifyObservers();
}

TEST(SubjectTest, FirstObserverDestroyed)
{
	MockSubject subject;
	auto observer2 = std::make_shared<MockObserver>();
	auto observer3 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer3, Update(_)).InSequence(seq);

	{
		auto observer1 = std::make_shared<MockObserver>();
		subject.RegisterObserver(observer1, 1);
	}
	subject.RegisterObserver(observer2, 2);
	subject.RegisterObserver(observer3, 3);
	subject.NotifyObservers();
}

TEST(SubjectTest, NegativePriority)
{
	MockSubject subject;
	auto observer1 = std::make_shared<MockObserver>();
	auto observer2 = std::make_shared<MockObserver>();

	testing::Sequence seq;
	EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
	EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

	subject.RegisterObserver(observer2, 10);
	subject.RegisterObserver(observer1, -5);
	subject.NotifyObservers();
}
} // namespace subjectTests
