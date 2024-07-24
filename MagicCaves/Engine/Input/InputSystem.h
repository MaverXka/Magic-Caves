#pragma once

#include "pch.h"
#include "InputKeyBinds.h"
#include <set>

class InputSystem
{

public:

	bool IsKeyPressedDown(KeyboardKeys Key);

	void AddWinKeyDown(KeyboardKeys Key);
	void AddWinKeyUp(KeyboardKeys Key);

protected:



private:

	std::set<KeyboardKeys> KeyPressed;

};

