#pragma once
#include <dsound.h>
#include <mmsystem.h>

class Sound {
public:
    Sound();
    bool LoadSound(char *aaa, DWORD);

private:
    LPDIRECTSOUND8 m_sound_interface;
};
