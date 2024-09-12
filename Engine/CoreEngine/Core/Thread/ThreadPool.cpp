#include "common_pch.h"
#include "ThreadPool.h"

namespace Thread
{
	ThreadPool::ThreadPool(int32_t InThreadCount)
		: mThreadCount(InThreadCount)
	{
		for (int32_t i = 0; i < mThreadCount; ++i)
		{
			mThreads.emplace_back(&ThreadPool::WorkerThread, this);
		}
	}

	ThreadPool::~ThreadPool()
	{
		mTaskQueue.Stop();

		for (auto& thread : mThreads)
		{
			/// 자식 스레드가 종료되기를 기다림 (메인 스레드 말고)
			/// 만약 join을 호출하지 않으면 그러니까,
			/// 자식 스레드가 종료되지 않은 상태에서 메인 스레드가 종료되면??
			/// ERROR! 아마 abort()를 호출할 것임 (자식 스레드가 강제 종료됨)
			/// 그래서 join을 호출해서 자식 스레드가 종료되기를 기다림

			/// joinable()은 스레드가 실행 중인지 아닌지(활성)를 확인하는 메서드
			/// detach()를 호출하면 joinable()은 false를 반환함
			/// detach는 뭐냐? 아예 메인 스레드에서 분리해버려서
			/// 메인 스레드가 종료되어도 자식 스레드는 계속 돌아가게 하는 것
			if (thread.joinable())
				// 이렇게 해야 스레드를 안전하게 소멸시킬 수 있음
				// 메서드 이름이 join인 이유는... (POSIX에서는 pthread_join이라서 그런가?)
				// 메인에서 fork해서 자식 스레드가 생성되는데 ,
				// 다시 메인으로 join한다는 의미로 생각하면 됨 (윈도우의 wait이라고 생각하면 된다)
				thread.join();
		}
	}

	void ThreadPool::WorkerThread()
	{
		while (true)
		{
			// 스레드에 작업을 할당한다. (이 task는 ExecuteTask를 호출하기 전까지 대기한다)
			std::function<void()> task = mTaskQueue.Pop();

			if (!task)
				return;

			task();
		}
	}
}
