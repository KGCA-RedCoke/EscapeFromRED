#pragma once
#include <future>

#include "ThreadQueue.h"

namespace Thread
{
	/**
	 * 
	 */
	class ThreadPool
	{
	public:
		ThreadPool(int32_t InThreadCount);
		~ThreadPool();

		/// 반환타입이 auto인 이유는 Pred(args...)의 반환타입을 알 수 없기 때문
		/// std::future<decltype(Pred(args...))>이 반환타입이라고 생각하면 된다.
		/// 여기에 어떤 작업(메서드)를 넣는지 컴파일 타임에 알 수 없기때문
		template <typename Func, typename... Args>
		auto ExecuteTask(Func &&Pred, Args &&... args) -> std::future<decltype(Pred(args...))>
		{
			using returnType = decltype(Pred(args...));

			/// packaged_task?
			/// 함수 객체를 나중에 실행하기 위해 캡슐화하는 역할
			/// 이 개체를 통해 함수를 실행
			/// 내부 함수가 실행되면 결과를 저장하고, 이 결과를 future로 반환
			/// (future는 js의 promise와 비슷한 개념인데 잘 모르겠다면 레퍼런스 예제를 확인...)
			auto task = std::make_shared<std::packaged_task<returnType()>>(
			std::bind(std::forward<Func>(Pred), std::forward<Args>(args)...));

			std::future<returnType> res = task->get_future();

			mTaskQueue.Push([task]() { (*task)(); });

			return res;
		}

	private:
		void WorkerThread();

	private:
		ThreadQueue<std::function<void()>> mTaskQueue;
		std::vector<std::jthread>          mThreads;

		int32_t mThreadCount = 0;
	};
}
