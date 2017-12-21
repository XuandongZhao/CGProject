#pragma once
#ifndef SHINA_LIB_KEYBOARD_H
#define SHINA_LIB_KEYBOARD_H
#include "main.h"
/** key map declare
* use ascii
* 
*/
enum keyMap {

	_a = 97,
	_s = 115,
	_d = 100,
	_w = 119
}ss;
class smKeyBoard {
private:
	enum{KEY_NUMER=1000};
	bool keys[KEY_NUMER];
	
	
public:
	smKeyBoard()
	{
		memset(keys, 0, sizeof(keys));
	}
	bool& getKey(const keyMap& key)
	{
		return keys[key];
	}

};


#endif