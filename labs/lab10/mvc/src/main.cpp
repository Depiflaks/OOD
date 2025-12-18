#include <boost/signals2.hpp>
#include <iostream>

class Subject
{
public:
	boost::signals2::signal<void()> stateChangedSignal;
	boost::signals2::signal<void(int)> taskCompletedSignal;

	void changeState()
	{
		std::cout << "State has been changed!" << std::endl;
		stateChangedSignal();
	}

	void completeTask(int taskId)
	{
		std::cout << "Task " << taskId << " completed!" << std::endl;
		taskCompletedSignal(taskId);
	}
};

class StateObserver
{
public:
	void onStateChanged()
	{
		std::cout << "StateObserver: The state has changed!" << std::endl;
	}
};

class TaskObserver
{
public:
	void onTaskCompleted(int taskId)
	{
		std::cout << "TaskObserver: Task " << taskId << " is completed!"
				  << std::endl;
	}
};

int main()
{
	Subject subject;
	StateObserver stateObserver;
	TaskObserver taskObserver;

	subject.stateChangedSignal.connect(
		[&stateObserver]() { stateObserver.onStateChanged(); });

	subject.taskCompletedSignal.connect(
		[&taskObserver](int taskId) { taskObserver.onTaskCompleted(taskId); });

	subject.changeState();
	subject.completeTask(1);

	TaskObserver anotherTaskObserver;
	subject.taskCompletedSignal.connect([&anotherTaskObserver](int taskId) {
		anotherTaskObserver.onTaskCompleted(taskId);
	});

	subject.completeTask(2);

	return 0;
}