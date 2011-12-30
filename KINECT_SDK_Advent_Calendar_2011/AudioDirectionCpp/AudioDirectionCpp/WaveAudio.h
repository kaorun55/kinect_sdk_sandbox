#pragma once 

#include <Windows.h>

#include <mmsystem.h>
#pragma comment( lib, "winmm.lib" )

#include <vector>

class WaveAudio
{
public:

    WaveAudio()
    {
    }

    virtual ~WaveAudio()
    {
        waveOutPause( hWaveOut );

        for ( size_t i = 0; i < waveHeaders_.size(); ++i ) {
            delete[] waveHeaders_[i].lpData;
        }
    }

    void Initialize( int nSampleRate, int nBitsPerSample, int nChannels, int audioBufferCount = 100 )
    {
        audioBufferCount_ = audioBufferCount;
        audioBufferNextIndex_ = 0;

        // WAVEデータの設定
        WAVEFORMATEX wf;
        wf.wFormatTag = 0x0001; // PCM
        wf.nChannels = nChannels;
        wf.nSamplesPerSec = nSampleRate;
        wf.wBitsPerSample = nBitsPerSample;
        wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;
        wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;

        MMRESULT mmRes = ::waveOutOpen( &hWaveOut, WAVE_MAPPER, &wf, NULL, NULL, CALLBACK_NULL );
        if ( mmRes != MMSYSERR_NOERROR ) {
            throw std::runtime_error( "waveOutOpen failed\n" );
        }

        // 音声データ用のバッファの作成と初期化
        waveHeaders_.resize( audioBufferCount_ );
        memset( &waveHeaders_[0], 0, sizeof(WAVEHDR) * waveHeaders_.size() );

        for ( size_t i = 0; i < waveHeaders_.size(); ++i ) {
            waveHeaders_[i].lpData = new char[MAX_BUFFER_SIZE];
            waveHeaders_[i].dwUser = i;
            waveHeaders_[i].dwFlags = WHDR_DONE;
        }
    }

    void Output( const void* buffer, DWORD length )
    {
        // バッファの取得
        WAVEHDR* pHeader = &waveHeaders_[audioBufferNextIndex_];
        if ( (pHeader->dwFlags & WHDR_DONE) == 0 ) {
            return;
        }

        // WAVEヘッダのクリーンアップ
        MMRESULT mmRes = ::waveOutUnprepareHeader( hWaveOut, pHeader, sizeof(WAVEHDR) );
        if ( mmRes != MMSYSERR_NOERROR ) {
        }

        // WAVEデータの取得
        pHeader->dwBufferLength = length;
        pHeader->dwFlags = 0;
        memcpy( pHeader->lpData, buffer, pHeader->dwBufferLength );

        // WAVEヘッダの初期化
        mmRes = ::waveOutPrepareHeader( hWaveOut, pHeader, sizeof(WAVEHDR) );
        if ( mmRes != MMSYSERR_NOERROR ) {
            return;
        }

        // WAVEデータを出力キューに入れる
        mmRes = ::waveOutWrite( hWaveOut, pHeader, sizeof(WAVEHDR) );
        if ( mmRes != MMSYSERR_NOERROR ) {
            return;
        }

        // 次のバッファインデックス
        audioBufferNextIndex_ = (audioBufferNextIndex_ + 1) % audioBufferCount_;
    }

private:

    WaveAudio( const WaveAudio& rhs );
    WaveAudio& operator = ( const WaveAudio& rhs );

private:

    static const int MAX_BUFFER_SIZE = 2 * 1024 * 1024;

    HWAVEOUT hWaveOut;
    std::vector<WAVEHDR>        waveHeaders_;

    int audioBufferCount_;
    int audioBufferNextIndex_;
};
