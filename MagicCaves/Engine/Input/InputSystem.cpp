#include "InputSystem.h"

bool InputSystem::IsKeyPressedDown(KeyboardKeys Key)
{
    return KeyPressed.contains(Key);
}

void InputSystem::AddWinKeyDown(KeyboardKeys Key)
{
    KeyPressed.insert(Key);
}

void InputSystem::AddWinKeyUp(KeyboardKeys Key)
{
    KeyPressed.erase(Key);
}
