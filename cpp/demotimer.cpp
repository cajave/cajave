//timer.h
#ifndef _OS_TIMER_H_
#define _OS_TIMER_H_
namespace os {
class OsTimer {
public:
	using OsTimerCb = void (*)(const OsTimer *which_timer, void *usr_data);
	OsTimer(OsTimerCb cb, void *usr_data);
	int Init();
	int Start(unsigned int micro_seconds, bool periodic);
	int Stop();
	~OsTimer();
private:
	class OsTimerImpl;
	OsTimerImpl *impl_;
};
}
#endif
//source
#include "timer.h"

#include <pthread.h>
#include <iostream>
#if defined(__QNX__) || defined(__QNXNTO__)
#include <signal.h>
#include <time.h>
#else
#include <signal.h>
#include <time.h>
#endif

#define NANO_SECS ((unsigned long)1000000000)

namespace os {
class OsTimer::OsTimerImpl {
	using mutex_t = std::mutex;
public:
	OsTimerImpl(OsTimer::OsTimerCb cb, void *usr_data, const OsTimer *base_id)
		: initialized_(false), cb_(cb), usr_data_(usr_data), base_id_(base_id) {}
	int Init() {
		int result = 0;
		mutex_.Lock();
		if (!initialized_) {
			struct sigevent signal_event;
			signal_event.sigev_notify = SIGEV_THREAD;
			signal_event.sigev_value.sival_ptr = this;
			signal_event.sigev_notify_function = OsTimerImpl::TimerCb;
			signal_event.sigev_notify_attributes = nullptr;
			//timer_create posix定时器
			result = timer_create(clock_REALTIME, &signal_event, &timer_t_);
			initialized_ = true;
		}
		mutex_.Unlock();
		return result;
	}
	
	int Start(unsigned int nano_seconds, bool periodic) {
		struct itimerspec ts;
		int result = 0;
		if (!initialized_) {
			result = Init();
			if (0 != result) {
			return result;
			}
		}
		ts.it_value.tv_sec = nano_seconds / NANO_SECS;
		ts.it_value.tv_nsec = nano_seconds % NANO_SECS;
		if (periodic) {
			ts.it_interval.tv_sec = nano_seconds / NANO_SECS;
			ts.it_interval.tv_nsec = nano_seconds % NANO_SECS;
		} else {
			ts.it_interval.tv_sec = 0;
			ts.it_interval.tv_nsec = 0;
		}
		//timer_settime posix定时器
		result = timer_settime(timer_t_, 0, &ts, nullptr);
		return result;
	}
	
	int Stop() {
		struct itimerspec ts = {0};
		timer_settime(timer_t_, 0, &ts, nullptr);
		return 0;
	}
	
private:
	bool initialized_;
	OsTimer::OsTimerCb cb_;
	void *usr_data_;
	const OsTimer *base_id_;
	timer_t timer_t_;
	mutex_t mutex_;
	static void TimerCb(union sigval sv) {// callback
		auto *me = static_cast<OsTimerImpl *>(sv.sival_ptr);
		me->cb_(me->base_id_, me->usr_data_);
	}
};

OsTimer::OsTimer(OsTimerCb timer_cb, void *usr_data)
    : impl_(new OsTimerImpl(timer_cb, usr_data, this)) {}
OsTimer::~OsTimer() { delete impl_; }
int OsTimer::Start(unsigned int micro_seconds, bool periodic/*是否是周期性的*/) {
	return impl_->Start(micro_seconds * 1000, periodic);
}
int OsTimer::Stop() { return impl_->Stop(); }
}


#ifndef _TIMER_H
#define _TIMER_H
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <set>
#include <stack>
#include <thread>
#include <vector>
#include <atomic>//std::atomic_flag

namespace timer_unit
{
using t_id_t = std::size_t;
using t_handler_t = std::function<void(t_id_t)>;
using t_clk_t = std::chrono::steady_clock;
using t_tp_t = std::chrono::time_point<t_clk_t>;
using t_duration_t = std::chrono::microseconds;
using t_mutex_t = std::mutex;
using t_uni_lk_t = std::unique_lock<t_mutex_t>;

namespace detail
{
struct Event { // 可以传给定时器处理，也可以传给其他handler处理
	t_id_t id;
	t_tp_t start;
	t_duration_t period;
	t_handler_t handler;
	bool valid;//timer是否释放
	Event() : id(0), start(t_duration_t::zero()),
		period(t_duration_t::zero()), handler(nullptr), valid(false) {}
	template <typename Func>
	Event(t_id_t id, t_tp_t start, t_duration_t period, Func &&handler)
		: id(id), start(start), period(period),
		  handler(std::forward<Func>(handler)), valid(true) {}
	Event(Event &&r) = default;
	Event &operator=(Event &&ev) = default;
	Event(const Event &r) = delete;
	Event &operator=(const Event &r) = delete;
};

struct TimeEvent {
	t_tp_t next; // 下一次触发的时间
	t_id_t id; // 为什么不用Event*或者Event&
};

inline bool operator<(const TimeEvent &l, const TimeEvent &r) {return l.next < r.next;}
}

class Timer
{
	using t_worker_t = std::thread;
	using t_safe_flag_t = std::atomic_flag;// 无锁
	using t_cv_t = std::condition_variable;

public:
	Timer() : mutex_{}, cond_{}, worker_{}, events_{}, time_events_{}, free_ids_{}
		, stop_(ATOMIC_FLAG_INIT) //编译通过,将std::atomic_flag 初始化为false
	{
		//stop_ = ATOMIC_FLAG_INIT;//只能用operator=运算符//编译失败
		while(stop_.test_and_set(std::memory_order_acquire));//原子地设置标志为true并获得其先前值
		t_uni_lk_t lock(mutex_);
		worker_ = t_worker_t(std::bind(&Timer::Run, this));
		//std::cout << "Timer construct! ";
	}

	~Timer()
	{
		stop_.clear(std::memory_order_release); // 原子地设置标志为false
		t_uni_lk_t lock(mutex_);
		worker_.join();
		events_.clear();
		time_events_.clear();
		while (!free_ids_.empty()) {free_ids_.pop();}
		cond_.notify_all();//notify_all只是发通知，释放锁操作由局部锁执行。
	}
	
	t_id_t Add(const t_tp_t &when, t_handler_t &&handler,
	           const t_duration_t &period = t_duration_t::zero())
	{
		t_uni_lk_t lock(mutex_);
		t_id_t id = 0;
		if (free_ids_.empty()) {
			id = events_.size();
			detail::Event e(id, when, period, std::move(handler));
			events_.push_back(std::move(e));
		} else {
			id = free_ids_.top();
			free_ids_.pop();
			detail::Event e(id, when, period, std::move(handler));
			events_[id] = std::move(e);
		}
		time_events_.insert(detail::TimeEvent{when, id});
		cond_.notify_all();
		return id;
	}
	template <class Rep, class Period>
	t_id_t Add(const std::chrono::duration<Rep, Period> &when,
	           t_handler_t &&handler,
	           const t_duration_t &period = t_duration_t::zero())
	{
		return Add(t_clk_t::now() +
		           std::chrono::duration_cast<std::chrono::microseconds>(when),
		           std::move(handler), period);
	}
	bool Remove(t_id_t id)
	{
		t_uni_lk_t lock(mutex_);
		if (events_.empty() || events_.size() < id) {
			return false;
		}
		events_[id].valid = false;
		auto it = std::find_if(time_events_.begin(), time_events_.end(),
		[&](const detail::TimeEvent &te) {return te.id == id;});
		if (it != time_events_.end()) {
			free_ids_.push(it->id);
			time_events_.erase(it);
		}
		cond_.notify_all();
		return true;
	}

private:
	void Run()
	{
		//std::cout << "timer runs! ";
		while (stop_.test_and_set(std::memory_order_acquire)) {
			t_uni_lk_t lock(mutex_);
			//std::cout << "time events pass! ";
			if (time_events_.empty()) {
				cond_.wait(lock);
			} else {
				detail::TimeEvent te = *time_events_.begin();
				if (t_clk_t::now() >= te.next) {
					time_events_.erase(time_events_.begin());
					if (events_[te.id].valid && events_[te.id].period.count() > 0) {
						te.next += events_[te.id].period;
						time_events_.emplace(te);
					} else {
						events_[te.id].valid = false;
						free_ids_.push(te.id);
					}
					lock.unlock();
					events_[te.id].handler(te.id);
				} else {
					cond_.wait_until(lock, te.next);
				}
			}
		}
	}
private:
	t_mutex_t mutex_;
	t_cv_t cond_;
	t_worker_t worker_;
	t_safe_flag_t stop_;// = ATOMIC_FLAG_INIT;编译通过
	std::vector<detail::Event> events_;//事件池，存储有效和失效的事件
	std::set<detail::TimeEvent> time_events_;//定时器事件池
	std::stack<t_id_t> free_ids_;//存储已释放的timer的id
};
}
#endif
int main(int argc, char **argv)
{
	timer_unit::Timer timer;
	timer.Add(timer_unit::t_clk_t::now(), [](timer_unit::t_id_t id) {std::cout << "id:" << id << std::endl;}, std::chrono::seconds(5));
	timer.Add(timer_unit::t_clk_t::now(), [](timer_unit::t_id_t id) {std::cout << "id:" << id << std::endl;}, std::chrono::seconds(10));
	timer.Add(timer_unit::t_clk_t::now(), [](timer_unit::t_id_t id) {std::cout << "id:" << id << std::endl;}, std::chrono::seconds(15));
	timer.Add(std::chrono::minutes(1), [&timer](timer_unit::t_id_t id) {timer.Remove(0); timer.Remove(1); timer.Remove(2);});
	std::cout << "Hello World" << std::endl;
	while(true);
	//getchar();
	return 0;
}
