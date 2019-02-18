#include <thread>
#include<string>
#include <atomic>

class Reusable_Thread
{
public:
	Reusable_Thread()
		: m_thread_pause(true), m_thread_quit(false),
		m_thread(&Reusable_Thread::thread_worker, this)
	{ }

	~Reusable_Thread()
	{
		m_thread_quit = true;
		m_thread.join();
	}

	bool get_readiness() const { return m_thread_pause; }

	bool set_work(const std::function<void()>& work_func)
	{
		if (get_readiness())
		{
			m_work_func = work_func;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	std::atomic<bool> m_thread_pause;
	std::atomic<bool> m_thread_quit;
	std::thread m_thread;
	std::function<void()> m_work_func;

	void thread_worker()
	{
		while (!m_thread_quit)
		{
			if (!m_thread_pause)
			{
				m_work_func();
				m_thread_pause = true;
			}
		}
	}
};