#include "cSoundManager.h"
#include <iostream>
#include "lfalog.h"


namespace LFA
{

    cSoundManager *cSoundManager::mInstance = 0;


    cSoundManager::cSoundManager()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        mSpeaker = 0;
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		mSpeaker = 0;
		FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_WARNING);
#endif
    }

    cSoundManager::~cSoundManager()
    {
        release();
        mInstance = 0;
    }

    /**
    *  Get the cSoundManager instance pointer.
    */
    cSoundManager *cSoundManager::getInstancePtr()
    {
		if(mInstance)
			return mInstance;
	    
		mInstance = new cSoundManager();
		return mInstance;
    }


	void cSoundManager::destroy( void )
	{
		if(mInstance)
		{
			delete mInstance;
			mInstance = 0;
		}
	}

    /**
    *  Init the sound engine. Must be called in order to start the sound playback.
    * \param frequency frequency to start the engine (44100, 22050, 11025 etc.).
    * \param bps bit per sample, 8 or 16 bit are supported.
    * \param stereo set mono or stereo output.
    * \return boolean success code.
    */
    bool cSoundManager::init(int frequency, int bps, bool stereo)
    {
#if LFA_AUDIO_ENGINE == LFA_NONE
		return 1;
#elif LFA_AUDIO_ENGINE == LFA_HSS
        // create a speaker if we don't have it
        if (!mSpeaker)
            mSpeaker = new hssSpeaker();

        // open it
        return (mSpeaker->open((u32)frequency, (u32)bps, stereo) == HSS_OK);
#elif LFA_AUDIO_ENGINE == LFA_FMOD
        FMOD_RESULT result = FMOD::System_Create(&mSpeaker); 
        if(result != FMOD_OK)
        {
            fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        }
		return (mSpeaker->init(32, FMOD_INIT_NORMAL, NULL) == FMOD_OK);
#endif
    }

    /**
    *  Create a new cSound instance.
    *  If there is already a cSound with the same name or the sound file can't be loaded it returns NULL (0).
    * \param name the name of this instance.
    * \param filePath the sound file to load.
    * \param stream actually not used.
    * \return our new cSound object.
    */
    cSound *cSoundManager::createSound(std::string name, std::string filePath, bool stream)
    {
        // if we have already a cSound with the same name quit
        cSound *sound = mSounds[name];
        if (sound)
            return NULL;

#if LFA_AUDIO_ENGINE != LFA_NONE
        // find out if we have already loaded the data file
        cSoundData *data = mSoundData[filePath];
        // no, create a data file and load it
        if (data == 0)
        {
            data = new cSoundData();
            
#if LFA_AUDIO_ENGINE == LFA_HSS            
            data->mSound = new hssSound();
            		
			if (data->mSound->load(filePath.c_str()) != HSS_OK)     
#elif LFA_AUDIO_ENGINE == LFA_FMOD  
            FMOD_RESULT result = mSpeaker->createSound(filePath.c_str(), FMOD_SOFTWARE, NULL, &data->mSound);
            if(result != FMOD_OK)
            {
                fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            }
            
			if (result != FMOD_OK) 
#endif
            {	// try to load the file, if there is an errore deallocate memory and quit
                delete data->mSound;
                delete data;
                return NULL;
            }
            data->mPathFile = filePath;
            data->mInstanceCounter = 1;
            mSoundData[filePath] = data;
        }
        // yes, increase its counter
        else
        {            
            data->mInstanceCounter++;
        }

		// create a new cSound
		sound = new cSound(name, generateNewSoundID());
		sound->mData = data;
		mSounds[name] = sound;
#else
        // create a new cSound
        sound = new cSound(name, generateNewSoundID());
        mSounds[name] = sound;
#endif
        return sound;
    }

	cSoundPool * cSoundManager::createSoundPool( std::string name, std::string filePath, unsigned int poolSize /*= 2*/ )
	{		
		std::vector<cSound*> pool;
		for (unsigned int x = 0; x < poolSize; x++)
		{
			std::stringstream ss;
			ss << x;
			pool.push_back(createSound(name + "_" + ss.str(), filePath));
		}

		// create a new cSoundPool
		cSoundPool * sp = 0;
		sp = new cSoundPool(name, pool);
		mSoundPools[name] = sp;

		return sp;
	}

	cSoundVariant * cSoundManager::createSoundVariant( const std::string &name, const std::string &filePathToBaseVariant, unsigned int variantCount, const std::string &prefix /*= ""*/ )
	{
		std::vector<cSound*> pool;
		pool.push_back(createSound(name + "_0", filePathToBaseVariant));
		for (unsigned int x = 1; x <= variantCount; x++) // 1, 2, ... variantCount
		{ 
			std::stringstream ss;
			ss << x;
			std::string baseVariant = filePathToBaseVariant;
			baseVariant.resize(baseVariant.size() - 4);
			std::string baseEnding = filePathToBaseVariant.substr(filePathToBaseVariant.size() - 4, filePathToBaseVariant.size());
			std::string variantName =  baseVariant + prefix + ss.str() + baseEnding;
			pool.push_back(createSound(name + "_" + ss.str(), variantName));
	//		printf(variantName.c_str());
		}

		// create a new cSoundPool (cSoundVariant)
		cSoundVariant * sp = 0;
		sp = new cSoundVariant(name, pool);
		mSoundPools[name] = sp;

		return sp;
	}



    /**
    *  Release a cSound instance.
    *  if the sound is playing it will be stopped.
    *  the manager will destroy the object and deallocate the sound data if it
    *  isn't used by other cSound instances.
    * \param sound the cSound to release
    */
    void cSoundManager::releaseSound(cSound *sound)
    {
        if (sound == 0)
            return;
#if LFA_AUDIO_ENGINE != LFA_NONE
        // stop it before the deallocation
        sound->stop();

        cSoundData *data = sound->mData;
        // decrease the data counter associated with the sound, if it is 0 we can deallocate it
        data->mInstanceCounter--;
        if (data->mInstanceCounter == 0)
        {
            mSoundData.erase(data->mPathFile);
#if LFA_AUDIO_ENGINE == LFA_HSS
            if (data->mSound)
                delete data->mSound;
#elif LFA_AUDIO_ENGINE == LFA_FMOD
            data->mSound->release();
#endif

            delete data;
        }
#endif
        // remove from our list and delete it
        mSounds.erase(sound->mName);
        delete sound;
		sound = 0;
    }

	void cSoundManager::releaseSoundPool( cSoundPool * soundPool )
	{
		if (soundPool == 0)
			return;

		soundPool->release();

		mSoundPools.erase(soundPool->mName);
		delete soundPool;
		soundPool = 0;
	}

    /**
    *  Get a previously created cSound instance.
    * \param name the name of the cSound object.
    * \return our cSound object or NULL.
    */
    cSound *cSoundManager::getSound(std::string name)
    {
        return mSounds[name];
    }

	cSound * cSoundManager::getSound( unsigned int soundID )
	{
		if (soundID == 0)  //ID 0 is used as a null, and never designated to an actual sound.
			return 0;

		for (sndItr = mSounds.begin(); sndItr != mSounds.end(); sndItr++) 
		{
			if (sndItr->second) //the entry may be half created if this method is being called during creation of a sound.
			{
				if (sndItr->second->ID == soundID)	
					return sndItr->second;
			}
		}
		return 0;
	}

	cSoundPool * cSoundManager::getSoundPool( std::string name )
	{
		 return mSoundPools[name];
	}

    /**
    *  Play a cSound instance.
    * \param sound the cSound instance to play.
    * \param loop set the looping flag.
    */
#if LFA_AUDIO_ENGINE == LFA_NONE
	void cSoundManager::playSound2d(cSound *sound, bool loop){}
#else
    void cSoundManager::playSound2d(cSound *sound, bool loop)
    {


        if (!sound)
            return;

        // if it is already playing stop it first
        if (sound->mChannel)
            sound->mChannel->stop();

#if LFA_AUDIO_ENGINE == LFA_HSS
        // get our hssSound so we can play it
        hssSound *_hssSound = sound->mData->mSound;

        // set looping, must be set on the object before playing it
        sound->mLooping = loop;
        _hssSound->loop(sound->mLooping);
        // play it and save the channel object
        s32 chlnum = mSpeaker->playSound(_hssSound, HSS_PLAY_FREE | HSS_PLAY_PAUSED);
        sound->mChannel = mSpeaker->channel(chlnum);

        // if we have a channel set our default values
        if (sound->mChannel)
        {
            // this callback is necessary to notify our cSound when it have finished playing
            sound->mChannel->callback(cSound::channelCallback, sound);

            sound->setPan(sound->mPan);
            sound->setVolume(sound->mVolume);
            sound->setPitch(sound->mPitch);

            // resume the channel so it starts playing
            sound->mChannel->resume();
        }
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		FMOD::Sound *_hssSound = sound->mData->mSound;

		// set looping, must be set on the object before playing it
		sound->mLooping = loop;

		if(loop)
			_hssSound->setLoopCount(-1);
		else
			_hssSound->setLoopCount(0);

		// play it and save the channel object
		mSpeaker->playSound(FMOD_CHANNEL_FREE, _hssSound, false, &sound->mChannel);
#endif
    }
#endif

    /**
    *  Suspend the sound playback.
    *  this should be used when the underling OS need to regain the sound control.
    */
    void cSoundManager::suspend()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        if (mSpeaker)
            mSpeaker->suspend();
#endif

    }

    /**
    *  Resume the sound playback.
    *  resume the sound playback after a suspend.
    */    void cSoundManager::resume()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        if (mSpeaker)
            mSpeaker->resume();
#endif
    }

    /**
    *  Update the sound engine. This should be called in your application once every frame.
    */    void cSoundManager::update()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
		if (mSpeaker)
            mSpeaker->update();
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		if (mSpeaker)
            mSpeaker->update();
#endif
    }

    /**
    *  Release all the created cSounds and their sound data.
    */
    void cSoundManager::releaseSounds()
    {
        // destroy all data and clear out container
        for (std::map<std::string, cSoundData*>::iterator it = mSoundData.begin(); it != mSoundData.end(); ++it)
        {
            cSoundData *data = (cSoundData*)it->second;
            if (data)
            {
#if LFA_AUDIO_ENGINE == LFA_HSS
                if (data->mSound)
                    delete data->mSound;
#elif LFA_AUDIO_ENGINE == LFA_FMOD
                data->mSound->release();
#endif
                delete data;
            }          
        }
        mSoundData.clear();

		
		for (std::map<std::string, cSoundPool*>::iterator it = mSoundPools.begin(); it != mSoundPools.end(); ++it)
		{
			cSoundPool *p = (cSoundPool*)it->second;
			if (p)
			{
				delete p;
			}          
		}
		mSoundPools.clear();


        // destroy all sounds and clear out container
        for (std::map<std::string, cSound*>::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
        {
            cSound *sound = (cSound*)it->second;
            if (sound)
            {
                delete sound;
            }          
        }
        mSounds.clear();
    }

    /**
    *  Release the cSoundManager object.
    *  use this function to release all data and to close the sound playback.
    */
    void cSoundManager::release()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        if (mSpeaker)
            delete mSpeaker;
        mSpeaker = 0;
        
        releaseSounds();
#elif LFA_AUDIO_ENGINE == LFA_FMOD
        releaseSounds();
        mSpeaker->release();
        mSpeaker = 0;
#endif
    }



	void cSoundManager::stopAllSounds( void )
	{
#if LFA_AUDIO_ENGINE == LFA_HSS
		mSpeaker->stopSounds();
#elif LFA_AUDIO_ENGINE == LFA_FMOD
        for (std::map<std::string, cSound*>::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
        {
            cSound *sound = (cSound*)it->second;
            if(sound != 0)
                sound->mChannel->stop();
        }
#endif
	}


	bool cSoundManager::playSound2d( unsigned int soundID, bool loop /*= false*/ )
	{
		cSound * s = getSound(soundID);

		if (!s)
			return false;

		playSound2d(s, loop);
		return true;
	}


	bool cSoundManager::stopSound( unsigned int soundID )
	{
		cSound * s = getSound(soundID);

		if (!s)
			return false;

		s->stop();
		return true;
	}

	unsigned int cSoundManager::generateNewSoundID( void )
	{
		lastID ++;
		if (lastID > 65530)
			lastID = 1;  //ID 0 is used as a null, and never represents a true sound ID

		while (getSound(lastID)) //make sure the new ID is not already in use.
		{
			lastID++;
			if (lastID > 65530)
			{
				if (mSounds.size() > 65530) //sanity check
				{

		        	LOGERR("cSoundManager.cpp Too many sounds: "+lastID);
		            LOGERR("Going to DIE...");
		            exit(-1);


					//NO exceptions on Android: throw; //wtf are you crazy you have  more than 65530 sounds?!  crashing.
				    //TODO: use assert(0)? 
				}
							
							
				lastID = 1;
			}
		}

		return lastID;
	}



}
