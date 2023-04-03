#pragma once
#include <dinput.h>

class DInputHelper {
public:
    DInputHelper();
    ~DInputHelper();
    bool isLeftClick();

private:
    bool GetMouseState();

    LPDIRECTINPUT8 m_direct_input_interface;
    LPDIRECTINPUTDEVICE8 m_direct_input_device;
    DIMOUSESTATE m_current_mouse_state;
};
