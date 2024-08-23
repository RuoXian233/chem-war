#include "audio.h"

using namespace engine;
Mix_Music *AudioManager::currentBGM;

void AudioManager::Initialize() {
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC);
    int result = Mix_OpenAudio(
        MIX_DEFAULT_FREQUENCY, 
        MIX_DEFAULT_FORMAT,
        MIX_DEFAULT_CHANNELS,
        AUDIO_DEFUALT_CHUNK_SIZE
    );
    assert(!result && "Audio engine failed to initialize");
}

void AudioManager::Finalize() {
    Mix_CloseAudio();
    Mix_Quit();
}

void AudioManager::SetBGM(Mix_Music *music) {
    AudioManager::currentBGM = music;
}

void AudioManager::PlayBGM() {
    if (AudioManager::currentBGM && !Mix_PlayingMusic()) {
        Mix_PlayMusic(AudioManager::currentBGM, -1);
    }
}

void AudioManager::PauseBGM() {
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
    }
}

void AudioManager::ResumeBGM() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void AudioManager::StopBGM() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

bool AudioManager::HasBGM() {
    return AudioManager::currentBGM != nullptr;
}
