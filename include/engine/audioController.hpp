#pragma once

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include <SDL3/SDL_audio.h>
#include <string>
#include <unordered_map>
#include <vector>

typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioSpec spec;
} Sound;

class AudioController
{
    public:
        AudioController();
        ~AudioController();

        /// @brief Initializes the audio device and Sounds.
        /// @return True on success, false on failure.
        bool initialize();

        bool update();

        #pragma region Playback

        /// @brief Plays the given music. If music is already playing, it cancels that audio stream and replaces it with this one. The music will automatically loop through the update event.
        /// @param soundName The name of the music to play.
        /// @return True on success, false on failure.
        bool playMusic(std::string soundName);

        /// @brief Stops the current playing music by clearing the audio stream.
        /// @return True on success, false on failure.
        bool stopMusic();

        /// @brief Plays a sound effect for the given sound.
        /// @param soundSource The SoundSource to play.
        /// @return True on success, false on failure.
        bool playSound(std::string soundName);

        #pragma endregion Playback

        #pragma region Getters

        /// @return True if music is playing, false otherwise.
        bool playingMusic();

        /// @return The number of loaded music assets.
        size_t getMusicCount();

        /// @return The number of loaded sound assets.
        size_t getSoundCount();

        #pragma endregion Getters

        #pragma region SoundLoading

        /// @brief Frees all of the sound assets.
        void clearSoundAssets();

        /// @brief Loads a music sound asset.
        /// @param fname The file path of the sound wav file to load.
        /// @param soundName The name to give to the sound.
        /// @return True on success, false on failure.
        bool loadMusic(const char *fname, std::string soundName);

        /// @brief Loads a sound effect sound asset.
        /// @param fname The file path of the sound wav file to load.
        /// @param soundName The name to give to the sound.
        /// @return True on success, false on failure.
        bool loadSound(const char *fname, std::string soundName);

        #pragma endregion SoundLoading

    private:

        #pragma region Playback

        /// @brief Queues playback for the given Sound.
        /// @param sound The Sound to queue.
        /// @param stream The audio stream to queue the sound data to.
        /// @return True on success, false on failure.
        bool queueSound(Sound *sound, SDL_AudioStream *stream);

        #pragma endregion Playback

        #pragma region SoundLoading

        /// @brief Initializes a Sound. Creates the audio stream and loads the .wav file.
        /// @param fname The .wav file path, relative to the base SDL path.
        /// @param sound The Sound instance to populate.
        /// @return True on success, false on failure.
        bool initSound(const char *fname, Sound *sound);

        /// @brief Frees the given Sound resource.
        /// @param sound The Sound to free.
        void freeSound(Sound *sound);

        #pragma endregion SoundLoading 

	SDL_AudioStream *createAudioStream(SDL_AudioSpec *spec);
	void freeAudioStreams();
	void cleanUpFinishedSounds();
	void freeAudioStream(SDL_AudioStream *stream);

        SDL_AudioDeviceID audio_device;

	SDL_AudioStream *musicStream = NULL;
	std::vector<SDL_AudioStream*> soundStreams;

        std::unordered_map<std::string, Sound*> musics;
        std::unordered_map<std::string, Sound*> sounds;

        std::string currentMusic = "";
};

#endif
