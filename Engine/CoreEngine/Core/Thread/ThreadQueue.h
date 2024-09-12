#pragma once
#include <condition_variable>
#include <queue>

// 덜 떨어진 OS때문에 여기서 사용되는 C++ 스레드 라이브러리는 
// OS단 스레드보다 몇몇 기능이 없다.

namespace Thread
{
	template <typename T>
	class ThreadQueue
	{
	public:
		void Push(T const &data)
		{
			// 임계 영역에 진입하기 전에 뮤텍스를 잠금
			std::scoped_lock<std::mutex> lock(mMutex);

			// Task를 큐에 추가하고 스레드에게 알림을 보냄
			mQueue.push(data);
			mConditionVariable.notify_one();
		}

		T Pop()
		{
			// 임계 영역에 진입하기 전에 뮤텍스를 잠금
			std::unique_lock<std::mutex> lock(mMutex);

			/// Task 큐가 비어있는 경우 스레드를 대기시킴
			/// 뒤에 오는 Pred는 아래와 같음
			/// while (mQueue.empty()) // 비어있는 경우에만 대기
			///		wait(lock);
			mConditionVariable.wait(lock, [&]() { return !mQueue.empty() || bStop; });

			if (bStop && mQueue.empty())
				return nullptr;

			// Task 큐에서 Task를 가져옴
			T result = mQueue.front();
			mQueue.pop();

			return result;
		}

		void Stop()
		{
			std::scoped_lock<std::mutex> lock(mMutex);
			bStop = true;
			mConditionVariable.notify_all();
		}

	private:
		bool bStop = false;

		// 작업 큐
		// 이 컨테이너 큐에 function들이 들어간다.
		std::queue<T> mQueue; // Task 큐

		/// 이벤트 개체, notify를 받을 때까지 현재 스레드의 실행을 멈춘다.
		/// unique_lock을 사용해야 한다.
		std::condition_variable mConditionVariable; // 스레드에게 알림을 보내기 위한 조건 변수

		/// 공유 자원을 잠그기 위해서 필요하다 (화장실에 들어간다고 생각하면 됨)
		/// 뮤텍스는 임계 영역에 진입하기 전에 잠그고, 임계 영역을 빠져나올 때 해제한다
		/// 이건 raw포인터를 해제하는 것 처럼 실수를 할 여지가 많아서... 
		/// 여기서 뮤텍스를 잠글 때 사용하는 lock메서드는
		/// 뮤텍스를 잠그고, 뮤텍스가 해제될 때 자동으로 해제된다. (데드락 방지)
		/// (lock_guard, scoped_lock(lock_guard의 복수 버전), unique_lock)
		std::mutex mMutex; // Task큐 접근을 위한 뮤텍스

		/// unique_lock?
		/// 기본적으로 scoped_lock과 비슷하다.
		/// * 생성시에 lock을 잠그지 않을 수도 있다(생성자의 두 번째 인자로 std::defer_lock를 넘기면 된다)
		/// * std::recursive_mutex를 사용할 수 있다.
		/// * condition_variable에 쓸 수 있는 유일한 lock이다.

	};

}
