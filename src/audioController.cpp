#include "audioController.hpp"
#include "stringUtils.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_filesystem.h>

/*
    TODO

-Pausing and unpausing audio streams
    -Pausing music
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

    clearSoundAssets();
}

/// @brief Initializes the audio device.
/// @return True on success, false on failure.
bool AudioController::initialize()
{
    audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    if (audio_device == 0)
    {
        SDL_Log("Couldn't open audio device: %s", SDL_GetError());

        return false;
    }

    return true;
}

bool AudioController::update()
{
    cleanUpFinishedSounds();

    if (playingMusic())
    {
        int queuedBytes;
        
        //Check if we need to queue another loop of music
        queuedBytes = SDL_GetAudioStreamQueued(musicStream);

        if (queuedBytes == -1)
        {
            SDL_Log("AudioController: Failed to get queued bytes for music: %s", currentMusic.c_str());

            return false;
        }

        //SDL_Log("Queue music bytes: %d", queuedBytes);

        //Loop if the remaining music to play is under 100 bytes
        if (queuedBytes <= 100)
        {
	    Sound *sound;

	    sound = musics.find(currentMusic)->second;

            if (!queueSound(sound, musicStream))
            {
                return false;
            }
        }
    }

    return true;
}

#pragma region Playback

/// @brief Plays the given music. If music is already playing, nothing happens. The music will automatically loop through the update event.
/// @param soundName The name of the music to play.
/// @return True on success, false on failure.
bool AudioController::playMusic(std::string soundName)
{
    soundName = stringToLower(soundName);

    if (currentMusic == soundName)
    {
	    //Already playing
	    return true;
    }

    if (musics.find(soundName) == musics.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AudioController: Failed to play music. Could not find sound name '%s' in our musics map.", soundName.c_str());

        return false;
    }

    if (!stopMusic())
    {
        return false;
    }

    Sound *sound;

    sound = musics.find(soundName)->second;
    musicStream = createAudioStream(&sound->spec);

    if (!queueSound(sound, musicStream))
    {
	freeAudioStream(musicStream);
	musicStream = NULL;

        return false;
    }

    currentMusic = soundName;

    return true;
}

bool AudioController::stopMusic()
{
    if (musicStream != NULL)
    {
	    freeAudioStream(musicStream);
	    musicStream = NULL;
    }

    currentMusic = "";

    return true;
}

/// @brief Plays a sound effect for the given sound.
/// @param soundName The name of the sound to play.
/// @return True on success, false on failure.
bool AudioController::playSound(std::string soundName)
{
    soundName = stringToLower(soundName);
    
    if (sounds.find(soundName) == sounds.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AudioController: Failed to play sound. Could not find sound name '%s' in our sounds map.", soundName.c_str());

        return false;
    }

    Sound *sound;
    SDL_AudioStream *stream;

    sound = sounds.find(soundName)->second;
    stream = createAudioStream(&sound->spec);

    if (!queueSound(sound, stream))
    {
	freeAudioStream(stream);

        return false;
    }

    soundStreams.push_back(stream);

    return true;
}

/// @brief Queues playback for the given Sound.
/// @param sound The Sound to queue.
/// @param stream The audio stream to queue the sound data to.
/// @return True on success, false on failure.
bool AudioController::queueSound(Sound *sound, SDL_AudioStream *stream)
{
     if (!SDL_PutAudioStreamData(stream, sound->wav_data, (int) sound->wav_data_len))
     {
        return false;
     }

     return true;
}

#pragma endregion Playback

#pragma region Getters

/// @return True if music is playing, false otherwise.
bool AudioController::playingMusic()
{
    return currentMusic != "";
}

/// @return The number of loaded music assets.
size_t AudioController::getMusicCount()
{
    return musics.size();
}

/// @return The number of loaded sound assets.
size_t AudioController::getSoundCount()
{
    return sounds.size();
}

#pragma endregion Getters

#pragma region SoundLoading

/// @brief Frees all of the sound assets.
void AudioController::clearSoundAssets()
{
    freeAudioStreams();

    if (musics.empty() && sounds.empty()) return;

    SDL_Log("AudioController: Freeing all sound assets...");

    for (auto it = musics.begin(); it != musics.end(); it++)
    {
        freeSound(it->second);
        delete it->second;
        it->second = NULL;
    }

    for (auto it = sounds.begin(); it != sounds.end(); it++)
    {
        freeSound(it->second);
        delete it->second;
        it->second = NULL;
    }

    musics.clear();
    sounds.clear();
}

/// @brief Loads a music sound asset.
/// @param fname The file path of the sound wav file to load.
/// @param soundName The name to give to the sound.
/// @return True on success, false on failure.
bool AudioController::loadMusic(const char *fname, std::string soundName)
{
    Sound *sound;

    soundName = stringToLower(soundName);

    //Make sure music doesn't already exist
    if (musics.find(soundName) != musics.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AudioController: Duplicate music asset. Music %s was added twice!", soundName.c_str());

        return false;
    }

    sound = new Sound();

    if (!initSound(fname, sound))
    {
        delete sound;

        return false;
    }

    musics[soundName] = sound;

    return true;
}

/// @brief Loads a sound effect sound asset.
/// @param fname The file path of the sound wav file to load.
/// @param soundName The name to give to the sound.
/// @return True on success, false on failure.
bool AudioController::loadSound(const char *fname, std::string soundName)
{
    Sound *sound;

    soundName = stringToLower(soundName);

    //Make sure sound doesn't already exist
    if (sounds.find(soundName) != sounds.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AudioController: Duplicate sound asset. Sound %s was added twice!", soundName.c_str());

        return false;
    }

    sound = new Sound();

    if (!initSound(fname, sound))
    {
        delete sound;
        
        return false;
    }

    sounds[soundName] = sound;

    return true;
}

/// @brief Initializes a Sound. Creates the audio stream and loads the .wav file.
/// @param fname The .wav file path, relative to the base SDL path.
/// @param sound The Sound instance to populate.
/// @return True on success, false on failure.
bool AudioController::initSound(const char *fname, Sound *sound)
{
    char *wav_path = NULL;
    bool success = true;

    SDL_asprintf(&wav_path, "%s%s", SDL_GetBasePath(), fname);

    if (!SDL_LoadWAV(wav_path, &sound->spec, &sound->wav_data, &sound->wav_data_len)) 
    {
        SDL_Log("AudioController: Couldn't load .wav file: %s", SDL_GetError());

        success = false;
    }

    SDL_free(wav_path);

    return success;
}

/// @brief Frees the given Sound resource.
/// @param sound The Sound to free.
void AudioController::freeSound(Sound *sound)
{
    /*
    if (sound->stream) 
    {
        SDL_DestroyAudioStream(sound->stream);

        sound->stream = NULL;
    }
    */

    SDL_free(sound->wav_data);

    sound->wav_data = NULL;
}

#pragma endregion SoundLoading

/// @brief Creates an audio stream.
/// @return The audio stream, or NULL if it failed to create.
SDL_AudioStream* AudioController::createAudioStream(SDL_AudioSpec *spec)
{
	SDL_AudioStream *stream;

	/* Create an audio stream. Set the source format to the wav's format (what
	       we'll input), leave the dest format NULL here (it'll change to what the
	       device wants once we bind it). */
	stream = SDL_CreateAudioStream(spec, NULL);

	if (!stream) 
	{
		SDL_Log("AudioController: Failed to create audio stream. Error: %s", SDL_GetError());

		stream = NULL;
	} 
	else if (!SDL_BindAudioStream(audio_device, stream)) 
	{  
		/* once bound, it'll start playing when there is data available! */
		SDL_Log("AudioController: Failed to bind audio stream to the audio device. Error: %s", SDL_GetError());

		freeAudioStream(stream);
		stream = NULL;
	} 

	return stream;

}

void AudioController::freeAudioStreams()
{
	if (musicStream != NULL)
	{
		freeAudioStream(musicStream);
		musicStream = NULL;
	}

	for (int i = 0; i < soundStreams.size() ; i++)
	{
		freeAudioStream(soundStreams[i]);
	}

	soundStreams.clear();
}

void AudioController::cleanUpFinishedSounds()
{
	if (soundStreams.size() == 0) return;

	//SDL_Log("AudioController: Cleaning up finished sounds...");

	for (int i = soundStreams.size() - 1; i >= 0; i--)
	{
		int queuedBytes;
        
		queuedBytes = SDL_GetAudioStreamQueued(soundStreams[i]);

		if (queuedBytes == -1)
		{
		    SDL_Log("AudioController: Failed to get queued bytes for sound stream. Error: %s", SDL_GetError());

		    continue;
		}
		
		if (queuedBytes <= 50)
		{
			freeAudioStream(soundStreams[i]);
			soundStreams.erase(soundStreams.begin() + i);

			SDL_Log("AudioController: Cleaned up sound");
			SDL_Log("AudioController: Number of sound streams that exist: %ld", soundStreams.size());
		}
	}
}

void AudioController::freeAudioStream(SDL_AudioStream *stream)
{
	SDL_DestroyAudioStream(stream);
}
