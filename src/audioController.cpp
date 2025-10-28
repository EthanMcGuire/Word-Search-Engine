#include "audioController.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_filesystem.h>

/*
    TODO

-Pausing and unpausing audio streams
    -Pausing music

-MAP to print music/sound names for debugging purposes
*/

AudioController::AudioController()
{
    audio_device = 0;
}

AudioController::~AudioController()
{
    if (audio_device != 0)
    {
        SDL_CloseAudioDevice(audio_device);
        
        audio_device = 0;
    }

    for (int i = 0; i < MUSIC_COUNT; i++) 
    {
        freeSound(&music[i]);
    }

    for (int i = 0; i < SOUND_COUNT; i++) 
    {
        freeSound(&sounds[i]);
    }
}

/// @brief Initializes the audio device and Sounds.
/// @return True on success, false on failure.
bool AudioController::initialize()
{
    audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    if (audio_device == 0)
    {
        SDL_Log("Couldn't open audio device: %s", SDL_GetError());

        return false;
    }

    for (int i = 0; i < MUSIC_COUNT; i++)
    {
        if (!initSound(MUSIC_PATH[i], &music[i])) 
        {
            SDL_Log("Failed to load music: %s", MUSIC_PATH[i]);

            return false;
        }
    }

    for (int i = 0; i < SOUND_COUNT; i++)
    {
        if (!initSound(SOUND_PATH[i], &sounds[i])) 
        {
            SDL_Log("Failed to load sound: %s", SOUND_PATH[i]);

            return false;
        }
    }

    return true;
}

/// @brief Plays the given music source. If music is already playing, it cancels that audio stream and replaces it with this one. The music will automatically loop through the update event.
/// @param musicSource The MusicSource to play.
/// @return True on success, false on failure.
bool AudioController::playMusic(MusicSource musicSource)
{
    if (musicSource == MUSIC_SOURCE_NONE)
    {
        SDL_Log("Attempted to play invalid music source MUSIC_SOURCE_NONE!");

        return false;
    }

    if (!stopMusic())
    {
        return false;
    }

    if (!queueSound(&music[musicSource - 1]))
    {
        return false;
    }

    currentMusic = musicSource;

    return true;
}

bool AudioController::stopMusic()
{
    if (!playingMusic())
    {
        return true;
    }

    if (!SDL_ClearAudioStream(music[currentMusic - 1].stream))
    {
        SDL_Log("Failed to clear audio stream for music source: %d", currentMusic);

        return false;
    }

    currentMusic = MUSIC_SOURCE_NONE;

    return true;
}

/// @return True if music is playing, false otherwise.
bool AudioController::playingMusic()
{
    return currentMusic != MUSIC_SOURCE_NONE;
}

/// @brief Plays a sound effect for the given sound source. If the sound is already playing, it will queue behind the current instance.
/// @param soundSource The SoundSource to play.
/// @return True on success, false on failure.
bool AudioController::playSound(SoundSource soundSource)
{
    if (!queueSound(&sounds[soundSource - 1]))
    {
        return false;
    }

    return true;
}

bool AudioController::update()
{
    if (playingMusic())
    {
        int queuedBytes;

        //Check if we need to queue another loop of music
        queuedBytes = SDL_GetAudioStreamQueued(music[currentMusic - 1].stream);

        if (queuedBytes == -1)
        {
            SDL_Log("Failed to get queued bytes for music: %d", currentMusic);

            return false;
        }

        //SDL_Log("Queue music bytes: %d", queuedBytes);

        //Loop if the remaining music to play is under 100 bytes
        if (queuedBytes <= 100)
        {
            if (!queueSound(&music[currentMusic - 1]))
            {
                return false;
            }
        }
    }

    return true;
}

/// @brief Initializes a Sound. Creates the audio stream and loads the .wav file.
/// @param fname The .wav file path, relative to the base SDL path.
/// @param sound The Sound instance to populate.
/// @return True on success, false on failure.
bool AudioController::initSound(const char *fname, Sound *sound)
{
    SDL_AudioSpec spec;
    char *wav_path = NULL;
    bool success = false;

    SDL_asprintf(&wav_path, "%s%s", SDL_GetBasePath(), fname);

    if (!SDL_LoadWAV(wav_path, &spec, &sound->wav_data, &sound->wav_data_len)) 
    {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return false;
    }

    /* Create an audio stream. Set the source format to the wav's format (what
       we'll input), leave the dest format NULL here (it'll change to what the
       device wants once we bind it). */
    sound->stream = SDL_CreateAudioStream(&spec, NULL);

    if (!sound->stream) 
    {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    } 
    else if (!SDL_BindAudioStream(audio_device, sound->stream)) 
    {  
        /* once bound, it'll start playing when there is data available! */
        SDL_Log("Failed to bind '%s' stream to device: %s", fname, SDL_GetError());
    } 
    else 
    {
        success = true;
    }

    SDL_free(wav_path);

    return success;
}

/// @brief Frees the given Sound resource.
/// @param sound The Sound to free.
void AudioController::freeSound(Sound *sound)
{
    if (sound->stream) 
    {
        SDL_DestroyAudioStream(sound->stream);

        sound->stream = NULL;
    }

    SDL_free(sound->wav_data);

    sound->wav_data = NULL;
}

/// @brief Queues playback for the  given Sound. If the sound is already playing, it will queue behind the current data.
/// @param Sound The Sound to queue.
/// @return True on success, false on failure.
bool AudioController::queueSound(Sound *sound)
{
     if (!SDL_PutAudioStreamData(sound->stream, sound->wav_data, (int) sound->wav_data_len))
     {
        return false;
     }

     return true;
}