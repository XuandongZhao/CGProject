#pragma once
#ifndef SHINA_LIB_EXCEPTION_H
#define SHINA_LIB_EXCEPTION_H
#include <string>
#include <iostream>

class basicExcept {
private:
	std::string exceptWord;
public:
	basicExcept() {}
	basicExcept(std::string exceptWord)
	{
		this->exceptWord = exceptWord;
	}
	virtual ~basicExcept() {}
	virtual void handleExcept()
	{
		std::cerr << exceptWord << std::endl;
	}
};
class SLIOException :public basicExcept {
public:
	SLIOException(std::string exceptWord)
	{
		basicExcept(exception);
	}
};


#endif
