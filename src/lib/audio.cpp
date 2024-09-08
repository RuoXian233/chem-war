#include "audio.h"

using namespace engine;
Mix_Music *AudioManager::currentBGM;
static engine::Logger logger("AudioManager");


void AudioManager::Initialize() {
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC);
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
    DEBUG("SDL_mixer initialized");
    int result = Mix_OpenAudio(
        MIX_DEFAULT_FREQUENCY, 
        MIX_DEFAULT_FORMAT,
        MIX_DEFAULT_CHANNELS,
        AUDIO_DEFUALT_CHUNK_SIZE
    );
    DEBUG_F("Audio engine initialized with parameters: freq={}, fmt={}, channels={}, chunk_size={}", MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, AUDIO_DEFUALT_CHUNK_SIZE);
    assert(!result && "Audio engine failed to initialize");
    INFO("Audio engine initialized");
}

void AudioManager::Finalize() {
    Mix_CloseAudio();
    DEBUG("SDL_mixer finalized");
    Mix_Quit();
    INFO("Audio engine finalized");
}

void AudioManager::SetBGM(Mix_Music *music) {
    AudioManager::currentBGM = music;
    INFO_F("BGM set to {}", music ? Mix_GetMusicTitle(music) : "nullptr");
}

void AudioManager::PlayBGM() {
    if (AudioManager::currentBGM && !Mix_PlayingMusic()) {
        INFO_F("Playing BGM: {}", Mix_GetMusicTitle(AudioManager::currentBGM));
        Mix_PlayMusic(AudioManager::currentBGM, -1);
    }
}

void AudioManager::PauseBGM() {
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        INFO("Pausing BGM");
        Mix_PauseMusic();
    }
}

void AudioManager::ResumeBGM() {
    if (Mix_PausedMusic()) {
        INFO("Resuming BGM");
        Mix_ResumeMusic();
    }
}

void AudioManager::StopBGM() {
    if (Mix_PlayingMusic()) {
        INFO("Stopping BGM");
        Mix_HaltMusic();
    }
}

bool AudioManager::HasBGM() {
    return AudioManager::currentBGM != nullptr;
}
