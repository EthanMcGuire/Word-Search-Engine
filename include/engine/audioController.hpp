#pragma once

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include <SDL3/SDL_audio.h>
#include <string>

typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioStream *stream;
} Sound;

enum MusicSource
{
    MUSIC_SOURCE_NONE,
    MUSIC_SOURCE_MUSIC_1,
    MUSIC_SOURCE_MUSIC_2
};

enum SoundSource
{
    SOUND_SOURCE_NONE,
    SOUND_SOURCE_MUSIC_1,
    SOUND_SOURCE_MUSIC_2
};

class AudioController
{
    public:
        AudioController();
        ~AudioController();

        /// @brief Initializes the audio device and Sounds.
        /// @return True on success, false on failure.
        bool initialize();

        /// @brief Plays the given music source. If music is already playing, it cancels that audio stream and replaces it with this one. The music will automatically loop through the update event.
        /// @param musicSource The MusicSource to play.
        /// @return True on success, false on failure.
        bool playMusic(MusicSource musicSource);

        /// @brief Stops the current playing music by clearing the audio stream.
        /// @return True on success, false on failure.
        bool stopMusic();

        /// @return True if music is playing, false otherwise.
        bool playingMusic();

        /// @brief Plays a sound effect for the given sound source. If the sound is already playing, it will queue behind the current instance.
        /// @param soundSource The SoundSource to play.
        /// @return True on success, false on failure.
        bool playSound(SoundSource soundSource);

        bool update();

    private:

        /// @brief Initializes a Sound. Creates the audio stream and loads the .wav file.
        /// @param fname The .wav file path, relative to the base SDL path.
        /// @param sound The Sound instance to populate.
        /// @return True on success, false on failure.
        bool initSound(const char *fname, Sound *sound);

        /// @brief Frees the given Sound resource.
        /// @param sound The Sound to free.
        void freeSound(Sound *sound);

        /// @brief Queues playback for the  given Sound. If the sound is already playing, it will queue behind the current data.
        /// @param Sound The Sound to queue.
        /// @return True on success, false on failure.
        bool queueSound(Sound *sound);

        static const int MUSIC_COUNT = 2;
        static const int SOUND_COUNT = 2;
        const char* MUSIC_PATH[MUSIC_COUNT] = {"assets\\audio\\music\\calm_track.wav", "assets\\audio\\music\\crickets.wav"};
        const char* SOUND_PATH[SOUND_COUNT] = {"assets\\audio\\music\\calm_track.wav", "assets\\audio\\music\\crickets.wav"};

        SDL_AudioDeviceID audio_device;
        Sound music[MUSIC_COUNT];
        Sound sounds[SOUND_COUNT];

        MusicSource currentMusic = MUSIC_SOURCE_NONE;
};

#endif