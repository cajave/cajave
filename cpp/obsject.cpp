#include <unordered_set>
#include <map>
#include <iostream>
#include <functional>

class IObserver;
class State;

class ISubject{
public:
	virtual ~ISubject(){}
	virtual void attach(IObserver*) = 0;
	virtual void detach(IObserver*) = 0;
	virtual void notify()/*  const  */= 0;
	virtual State* get_state() const = 0;
};

class IObserver{
public:
	virtual ~IObserver(){}
	virtual void update(ISubject* sub) = 0;
protected:
	IObserver(){}
};

class State{
public:
	State(ISubject* sub) : m_sub(sub){}
	void id(int i){
		if (m_id != i){
			m_id = i;
			std::cout << std::hex << m_sub 
				<< " subject's state is changed" << std::endl;
			if (m_sub){
				m_sub->notify();
			}
		}
	}
	int id() const { return m_id; }

private:
	ISubject* m_sub = nullptr;
	int m_id = -1;
};

class SubjectImpl : public ISubject{
public:
	virtual ~SubjectImpl() override {
		if (m_state){
			delete m_state;
			m_state = nullptr;
		}
	}
	virtual void attach(IObserver* obj) override {
		setObjs.insert(obj);
	}
	virtual void detach(IObserver* obj) override {
		setObjs.erase(obj);
	}
	virtual void notify() /* const */ override { //const will make this be const
		for (auto obj : setObjs){
			obj->update(this);
		}	
	}

	State* get_state() const override {
		return m_state;
	}
	
	SubjectImpl() : m_state(new State(this)){
	}
	
	void change_state(int id){
		if (m_state){
			m_state->id(id);
		}
	}
	
private:
	State* m_state = nullptr;
	std::unordered_set<IObserver*> setObjs;// 无序
};

class ObserverImpl : public IObserver{
public:
	/* ObserverImpl(){} */
	virtual void update(ISubject* sub) override {
		// sub pointers to which subject is changed
		State* state = sub->get_state();
		if (state){
			std::cout << std::hex << this 
				<< " observer get the changing, id = "
				<< state->id() << std::endl;
		}
	}
};

class NonCopyable{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

template <class F>
class Events : NonCopyable{
public:
	Events(){}
	~Events(){}
	int connect(F&& f){
		return assign(f);
	}
	int connect(const F& f){
		return assign(f);
	}
	void disconnect(int key){
		m_connections.erase(key);
	}
	template<class... Args>
	void notify(Args&&... args){
		for (const auto& f : m_connections){
			f.second(std::forward<Args>(args)...);
		}
	}
private:
	template<class F>
	int assign(F&& f){
		m_connections.emplace(m_observer_id, std::forward<F>(f));
		return ++m_observer_id;
	}

	int m_observer_id = 0;
	std::map<int, F> m_connections;
};

struct Observer{
	int m_mem1;
	int m_mem2;
	int m_mem3;
	void update(int mem1, int mem2, int mem3){
		std::cout << "Observer : ";
		std::cout << mem1 << " : " << mem2 << " : " << mem3 << std::endl;
	}
};

void update(int a, int b, int c){
	std::cout << "update : ";
	std::cout << a << " : " << b << " : " << c << std::endl;
}

int main(){
	ObserverImpl obj, obs;
	SubjectImpl sub;
	sub.attach(&obj);
	sub.attach(&obs);
	sub.change_state(1);
	sub.detach(&obj);
	sub.change_state(2);
	sub.detach(&obs);
	sub.change_state(3);

	Events<std::function<void(int, int, int)>> events;
	auto key = events.connect(update);
	Observer observer;
	auto lambdakey = events.connect([](int a, int b, double c){
		std::cout << "lambda : ";
		std::cout << a << " : " << b << " : " << c << std::endl;});
	std::function<void(int, int, int)> f = std::bind(&Observer::update, &observer, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	auto fkey = events.connect(f);
	events.notify(1, 2, 3);
	events.disconnect(key);
	events.disconnect(lambdakey);
	events.disconnect(fkey);

	return 0;
}
