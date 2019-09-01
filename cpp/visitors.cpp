
#include<iostream>
#include<typeinfo>

class IElement;
class IVisitor1{
public:
	virtual ~IVisitor1(){}
	virtual void visit(IElement& ie) = 0;
};

class Element1;
class Element2;
class IVisitor2{
public:
	virtual ~IVisitor2(){}
	virtual void visit(Element1& e1) = 0;
	virtual void visit(Element2& e2) = 0;
};

class IElement{
public:
	virtual ~IElement(){}
	virtual void accept(IVisitor1& iv) = 0;
	virtual void accept(IVisitor2& iv) = 0;
};

class Element1 : public IElement{
public:
	virtual void accept(IVisitor1& iv){
		iv.visit(*this);
	}
	virtual void accept(IVisitor2& iv){
		iv.visit(*this);
	}
	int id() const {
		return m_id;
	}
private:
	int m_id = 10;
};

class Element2 : public IElement{
public:
	virtual void accept(IVisitor1& iv){
		iv.visit(*this);
	}
	virtual void accept(IVisitor2& iv){
		iv.visit(*this);
	}
	int id() const {
		return m_id;
	}
private:
	int m_id = 20;
};

class Visitor1 : public IVisitor1{
public:
	virtual ~Visitor1(){}
	virtual void visit(IElement& ie){
		try {
			Element1& e1 = dynamic_cast<Element1&>(ie);
			std::cout << e1.id() << std::endl;
			return;
		} catch (const std::bad_cast& e){
			
		}
		
		try {
			Element2& e2 = dynamic_cast<Element2&>(ie);
			std::cout << e2.id() << std::endl;
			return;
		} catch (const std::bad_cast& e){
			
		}
	}	
};

class Visitor2 : public IVisitor2{
public:
	Visitor2(){}
	virtual ~Visitor2(){}
	virtual void visit(Element1& e1){
		std::cout << e1.id() << std::endl;
	}
	virtual void visit(Element2& e2){
		std::cout << e2.id() << std::endl;
	}
};


template <typename... Args>
class Visitor3;

template <typename T, typename... Args>
class Visitor3<T, Args...> : Visitor3<Args...>{
public:
	using Visitor3<Args...>::visit;
	virtual void visit(const T&) = 0;
};

template <typename T>
class Visitor3<T>{
public:
	virtual void visit(const T&) = 0;
};

class Element3a;
class Element3b;
class Base{
public:
	using visitor3 = Visitor3<Element3a, Element3b>;
	virtual void accept(visitor3&) = 0;
};

class Element3a : public Base{
	double m_value = 5.5;
public:
	void accept(Base::visitor3& v){
		v.visit(*this);
	}
	double get_value() const {return m_value;}
};

class Element3b : public Base{
	int m_value = 3;
public:	
	void accept(Base::visitor3& v){
		v.visit(*this);
	}
	int get_value() const {return m_value;}
};

class Visitor3Impl : public Base::visitor3{
public:
	void visit(const Element3a& a){
		std::cout << "Element3a:" << a.get_value() << std::endl;
	}
	void visit(const Element3b& b){
		std::cout << "Element3b:" << b.get_value() << std::endl;
	}
};

int main(){
	Element1 e1;
	Element2 e2;
	Visitor2 v2;
	e1.accept(v2);
	e2.accept(v2);
	
	Visitor1 v1;
	e1.accept(v1);
	e2.accept(v2);
	
	Visitor3Impl v3;
	Element3a v3a;
	Element3b v3b;
	Base *p = nullptr;
	p = &v3a;
	p->accept(v3);
	p = &v3b;
	p->accept(v3);
	
	return 0;
}