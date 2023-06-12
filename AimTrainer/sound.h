#pragma once
#include <dsound.h>
#include <mmsystem.h>

class Sound {
public:
    struct WavData {
        WAVEFORMATEX wav_format;
        char *sound_buffer;
        DWORD size;
    };

    bool InitSound();
    void ReleaseSound();
    bool LoadSound(LPWSTR file_name);
    bool LoadWAV(LPWSTR file_name, WavData *out_wave_data);
    void PlaySoundFile();

private:
    LPDIRECTSOUND8 m_sound_interface;
    LPDIRECTSOUNDBUFFER m_sound_buffer;
};
