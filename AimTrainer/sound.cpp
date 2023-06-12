#include "sound.h"
#include "engine.h"
#include <stdio.h>

/// <summary>
/// DirectSoundを初期化
/// </summary>
/// <returns>true: 成功 false: 失敗</returns>
bool Sound::InitSound() {
    // DirectSoundの生成
    auto hresult = DirectSoundCreate8(NULL, &m_sound_interface, NULL);
    if (FAILED(hresult)) {
        printf("DirectSoundの生成に失敗");
        return false;
    }

    // 強調レベルの設定
    hresult = m_sound_interface->SetCooperativeLevel(gp_engine->GetHwnd(), DSSCL_NORMAL);
    if (FAILED(hresult)) {
        printf("強調レベルの設定に失敗");
        return false;
    }

    return true;
}

void Sound::ReleaseSound() {
    // セカンダリバッファの開放
    m_sound_buffer->Stop();
    m_sound_buffer->Release();
    m_sound_buffer = NULL;

    // DirectSoundインターフェースの開放
    m_sound_interface->Release();
    m_sound_interface = NULL;
}

bool Sound::LoadSound(LPWSTR file_name) {
    WavData wav_data{};
    if (LoadWAV(file_name, &wav_data) == false) {
        return false;
    }

    // バッファ情報の設定
    DSBUFFERDESC dsbd{};
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = wav_data.size;
    dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
    dsbd.lpwfxFormat = &wav_data.wav_format;

    // セカンダリバッファ作成
    if (FAILED(m_sound_interface->CreateSoundBuffer(
        &dsbd,
        &m_sound_buffer,
        NULL
    ))) {
        // 作成失敗
        printf("セカンダリバッファ作成失敗");
        delete[] wav_data.sound_buffer;
        return false;
    }

    // 波形データを書き込むためにセカンダリバッファをロックする
    void *buffer;
    DWORD buffer_size;
    if (FAILED(m_sound_buffer->Lock(
        0,
        wav_data.size,
        &buffer,
        &buffer_size,
        NULL,
        NULL,
        0
    ))) {
        // ロック失敗
        printf("ロック失敗");
        delete[] wav_data.sound_buffer;
        return false;
    }

    // バッファにデータを反映
    memcpy(buffer, wav_data.sound_buffer, buffer_size);

    // アンロック
    m_sound_buffer->Unlock(
        &buffer,
        buffer_size,
        NULL,
        NULL
    );

    delete[] wav_data.sound_buffer;

    return true;
}

bool Sound::LoadWAV(LPWSTR file_name, WavData *out_wave_data) {
    // チャンク情報
    MMCKINFO ck_info{};
    // RIFFチャンク用
    MMCKINFO riffck_info{};

    // WAVファイル内音サンプルのサイズ
    DWORD dw_wav_size = 0;

    const auto mmio_handle = mmioOpen(file_name, NULL, MMIO_READ);
    if (mmio_handle == NULL) {
        printf("WAVファイルのロードに失敗");
        return false;
    }

    // RIFFチャンクに進入するためにfccTypeにWAVEを設定する
    riffck_info.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    // RIFFチャンクに進入する
    if (MMSYSERR_NOERROR != mmioDescend(
        mmio_handle,
        &riffck_info,
        NULL,
        MMIO_FINDRIFF
    )) {
        // 失敗
        mmioClose(mmio_handle, MMIO_FHOPEN);
        printf("RIFFチャンクの進入に失敗");
        return false;
    }

    // 進入先のチャンクをfmt として設定する
    ck_info.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (MMSYSERR_NOERROR != mmioDescend(mmio_handle, &ck_info, &riffck_info, MMIO_FINDCHUNK)) {
        // fmtチャンクがない
        mmioClose(mmio_handle, MMIO_FHOPEN);
        printf("fmtチャンクの進入に失敗");
        return false;
    }

    // fmtデータの読み込み
    LONG read_size = mmioRead(
        mmio_handle,
        (HPSTR)&out_wave_data->wav_format,
        sizeof(out_wave_data->wav_format));
    if (read_size != sizeof(out_wave_data->wav_format)) {
        // 読み込みサイズが一致していないのでエラー
        mmioClose(mmio_handle, MMIO_FHOPEN);
        printf("fmtデータの読み込みに失敗");
        return false;
    }

    // fmtチャンクを退出する
    if (mmioAscend(mmio_handle, &ck_info, 0) != MMSYSERR_NOERROR) {
        mmioClose(mmio_handle, MMIO_FHOPEN);
        printf("fmtチャンクの退出に失敗");
        return false;
    }

    // dataチャンクに進入する
    ck_info.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(mmio_handle, &ck_info, &riffck_info, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
        // 進入失敗
        mmioClose(mmio_handle, MMIO_FHOPEN);
        printf("dataチャンクの進入に失敗");
        return false;
    }

    // サイズを保存
    out_wave_data->size = ck_info.cksize;

    // dataチャンク読み込み
    out_wave_data->sound_buffer = new char[ck_info.cksize];
    read_size = mmioRead(mmio_handle, (HPSTR)out_wave_data->sound_buffer, ck_info.cksize);
    if (read_size != ck_info.cksize) {
        mmioClose(mmio_handle, MMIO_FHOPEN);
        delete[] out_wave_data->sound_buffer;
        printf("dataチャンクの読み込みに失敗");
        return false;
    }

    // ファイルを閉じる
    mmioClose(mmio_handle, MMIO_FHOPEN);

    return true;
}

void Sound::PlaySoundFile() {
    m_sound_buffer->Play(0, 0, DSBPLAY_LOOPING & 0);
}
