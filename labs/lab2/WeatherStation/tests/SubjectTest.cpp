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

const SubscribeType DEFAULT_TYPE = SubscribeType::TEMPERATURE_CHANGE;

TEST(SubjectTest, SingleObserver)
{
    MockSubject subject;
    auto observer = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer, Update(_)).Times(1);

    subject.Subscribe(DEFAULT_TYPE, observer, 0);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, TwoObservers)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, RemoveObserver)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer1, Update(_)).Times(1);
    EXPECT_CALL(*observer2, Update(_)).Times(0);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Unsubscribe(DEFAULT_TYPE, observer2);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, ObserverLifetime)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer1, Update(_)).Times(1);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);

    {
        auto observer2 = std::make_shared<MockObserver>();
        subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    }

    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, PriorityOrderBasic)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, PriorityOrderReverseRegistration)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, SamePriority)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 0);
}

TEST(SubjectTest, DublicatesPriority)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();
    auto observer3 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer3, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Subscribe(DEFAULT_TYPE, observer3, 0);
}

TEST(SubjectTest, ChangePriority)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();
    auto observer3 = std::make_shared<MockObserver>();

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Subscribe(DEFAULT_TYPE, observer3, 2);

    subject.Unsubscribe(DEFAULT_TYPE, observer2);
    subject.Subscribe(DEFAULT_TYPE, observer2, 3);

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer3, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, DublicatedSubscribe)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();
    auto observer3 = std::make_shared<MockObserver>();

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Subscribe(DEFAULT_TYPE, observer3, 2);

    subject.Subscribe(DEFAULT_TYPE, observer2, 3);

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer3, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, MiddleObserverDestroyed)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer3 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer3, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    {
        auto observer2 = std::make_shared<MockObserver>();
        subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    }
    subject.Subscribe(DEFAULT_TYPE, observer3, 2);
    subject.Notify(DEFAULT_TYPE);
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
        subject.Subscribe(DEFAULT_TYPE, observer1, 0);
    }
    subject.Subscribe(DEFAULT_TYPE, observer2, 1);
    subject.Subscribe(DEFAULT_TYPE, observer3, 2);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, NegativePriority)
{
    MockSubject subject;
    auto observer1 = std::make_shared<MockObserver>();
    auto observer2 = std::make_shared<MockObserver>();

    testing::Sequence seq;
    EXPECT_CALL(*observer1, Update(_)).InSequence(seq);
    EXPECT_CALL(*observer2, Update(_)).InSequence(seq);

    subject.Subscribe(DEFAULT_TYPE, observer2, 10);
    subject.Subscribe(DEFAULT_TYPE, observer1, -5);
    subject.Notify(DEFAULT_TYPE);
}

TEST(SubjectTest, ObserverSubscribedToTwoTypes)
{
    MockSubject subject;
    auto observer = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer, Update(_)).Times(2);

    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer, 0);
    subject.Subscribe(SubscribeType::HUMIDITY_CHANGE, observer, 0);
    
    subject.Notify(SubscribeType::TEMPERATURE_CHANGE);
    subject.Notify(SubscribeType::HUMIDITY_CHANGE);
}

TEST(SubjectTest, ResubscribeToDifferentType)
{
    MockSubject subject;
    auto observer = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer, Update(_)).Times(1);

    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer, 0);
    subject.Unsubscribe(SubscribeType::TEMPERATURE_CHANGE, observer);
    subject.Subscribe(SubscribeType::HUMIDITY_CHANGE, observer, 0);
    
    subject.Notify(SubscribeType::TEMPERATURE_CHANGE);
    subject.Notify(SubscribeType::HUMIDITY_CHANGE);
}

TEST(SubjectTest, ReprioritizeFails)
{
    MockSubject subject;
    auto observer = std::make_shared<MockObserver>();

    EXPECT_CALL(*observer, Update(_)).Times(1);

    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer, 0);
    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer, 1);
    
    subject.Notify(SubscribeType::TEMPERATURE_CHANGE);
}

TEST(SubjectTest, ObserverDestroyedBeforeNotify)
{
    MockSubject subject;
    
    auto observer1 = std::make_shared<MockObserver>();
    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, observer1, 0);
    
    {
        auto observer2 = std::make_shared<MockObserver>();
        subject.Subscribe(SubscribeType::HUMIDITY_CHANGE, observer2, 0);
    }
    
    EXPECT_CALL(*observer1, Update(_)).Times(1);
    
    subject.Notify(SubscribeType::TEMPERATURE_CHANGE);
    subject.Notify(SubscribeType::HUMIDITY_CHANGE);
}

TEST(SubjectTest, AllSubscriptionTypes)
{
    MockSubject subject;
    auto tempObserver = std::make_shared<MockObserver>();
    auto humidityObserver = std::make_shared<MockObserver>();
    auto pressureObserver = std::make_shared<MockObserver>();

    EXPECT_CALL(*tempObserver, Update(_)).Times(1);
    EXPECT_CALL(*humidityObserver, Update(_)).Times(1);
    EXPECT_CALL(*pressureObserver, Update(_)).Times(1);

    subject.Subscribe(SubscribeType::TEMPERATURE_CHANGE, tempObserver, 0);
    subject.Subscribe(SubscribeType::HUMIDITY_CHANGE, humidityObserver, 0);
    subject.Subscribe(SubscribeType::PRESSURE_CHANGE, pressureObserver, 0);
    
    subject.Notify(SubscribeType::TEMPERATURE_CHANGE);
    subject.Notify(SubscribeType::HUMIDITY_CHANGE);
    subject.Notify(SubscribeType::PRESSURE_CHANGE);
}} // namespace subjectTests
