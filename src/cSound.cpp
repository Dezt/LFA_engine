#include "cSound.h"
#include "cSoundManager.h"
#include <iostream>
#include "lfalog.h"

#include "LFAMath.h"

namespace LFA
{

	cSound::cSound(std::string name, unsigned int soundID) : ID(soundID)
    {
#if LFA_AUDIO_ENGINE != LFA_NONE
		mChannel = 0;
#endif
        mName = name;
        mData = 0;

        // default values
        mPan = 0.0f;
        mVolume = 1.0f;
        mPitch = 1.0f;
    }

    cSound::~cSound()
    {
    }

    /**
    *  Get the name of this cSound instance.
    * \return the cSound object name.
    */
    std::string cSound::getName() 
    { 
        return mName;
    }

    /**
    *  Set the pitch of this cSound instance.
    * \param pitch the pitch to set (1.f is the normal pitch).
    */
    void cSound::setPitch(float pitch)
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        mPitch = pitch;
        if (mPitch < 0.f)
            mPitch = 0.f;
        
		if (mChannel)
            mChannel->speed((u32)(mPitch * 128));
#endif
    }

    /**
    *  Get the pitch of this cSound instance.
    * \return the actual pitch.
    */
    float cSound::getPitch()
    {
        return mPitch;
    }

    /**
    *  Set the volume of this cSound instance.
    * \param volume the volume to set (0.f silece it, 1.f full volume).
    */
    void cSound::setVolume(float volume)
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        mVolume = volume;
        if (mVolume < 0.f)
            mVolume = 0.f;

        if (mChannel)
            mChannel->volume((u32)(mVolume * 64));
#endif
    }

    /**
    *  Get the volume of this cSound instance.
    * \return the actual volume.
    */
    float cSound::getVolume()
    {
        return mVolume;
    }

    /**
    *  Set the pan of this cSound instance.
    * \param pan the pan to set (-1.f full left, 0.f center, 1.f full right).
    */
    void cSound::setPan(float pan)
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        mPan = pan;
        if (mPan < -1.f)
            mPan = -1.f;
        else if (mPan > 1.f)
            mPan = 1.f;

        if (mChannel)
            mChannel->pan((u32)((mPan + 1.f) * 128));
#endif
    }

    /**
    *  Get the pan of this cSound instance.
    * \return the pan volume.
    */
    float cSound::getPan()
    {
        return mPan;
    }

    /**
    *  Play this cSound instance.
    * \param loop set the looping flag.
    */
    void cSound::play2d(bool loop)
    {
        if (isPlaying())
			stop();
           // return;  // if it is already playing quit, if we want we can instead stop it

        // play it using the function from the manager
        cSoundManager *soundMgr = cSoundManager::getInstancePtr();
        if (soundMgr)
            soundMgr->playSound2d(this, loop);
    }

#if LFA_AUDIO_ENGINE != LFA_NONE
    /**
    *  Stop this cSound instance.
    */
    void cSound::stop()
    {
        if (mChannel)
        {
            mChannel->stop();
        }
    }

    /**
    *  Pause/Resume this cSound instance.
    */
    void cSound::pause()
    {
        if (mChannel)
        {
#if LFA_AUDIO_ENGINE == LFA_HSS
            if (mChannel->paused())
                mChannel->resume();
            else
                mChannel->pause();
#elif LFA_AUDIO_ENGINE == LFA_FMOD
			bool paused = false;
			mChannel->getPaused(&paused);

			if (paused)
				mChannel->setPaused(false);
			else
				mChannel->setPaused(true);
#endif
        }
    }

    /**
    *  Check if this cSound instance is playing.
    * \return boolean return code.
    */
    bool cSound::isPlaying()
    {
        if(this == 0)
            return false;
        
        if (mChannel == 0)
            return false;
#if LFA_AUDIO_ENGINE == LFA_HSS
        return (mChannel->playing());
        
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		bool isPlaying = false;
		mChannel->isPlaying(&isPlaying);

		return isPlaying;
#endif
    }

#else
	void cSound::stop(){}
	void cSound::pause(){}
	bool cSound::isPlaying(){return 0;}
#endif


    /**
    *  Check if this cSound instance is stopped.
    * \return boolean return code.
    */
    bool cSound::isStopped()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        if (mChannel)
            return (mChannel->playing() == false);
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		if (mChannel)
		{
			bool isPlaying = false;
			mChannel->isPlaying(&isPlaying);

			return isPlaying;
		}
#endif
        return true;
    }

    /**
    *  Check if this cSound instance is paused.
    * \return boolean return code.
    */
    bool cSound::isPaused()
    {
#if LFA_AUDIO_ENGINE == LFA_HSS
        if (mChannel)
            return (mChannel->paused());
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		if (mChannel)
		{
			bool paused = false;

			mChannel->getPaused(&paused);
			return paused;
		}
#endif
        return false;
    }

    /**
    *  Check if this cSound instance is looping.
    * \return boolean return code.
    */

    bool cSound::isLooping()
    {
        return mLooping;
    }

    /**
    *  Release this cSound instance.
    */
    void cSound::release()
    {
        // release it using the function from the manager
        cSoundManager *soundMgr = cSoundManager::getInstancePtr();
        if (soundMgr)
            soundMgr->releaseSound(this);
    }

#if LFA_AUDIO_ENGINE == LFA_HSS
    s32 cSound::channelCallback(hssChannel *channel, hssChannelCallbackType type, void *userdata)
    {
        cSound *sound = (cSound*)userdata;
        if (sound)
        {
            if (type == HSS_CHANNEL_CALLBACKTYPE_END)
                sound->mChannel = 0;
        }

        return HSS_OK;
    }
#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SOUND POOL
	cSoundPool::cSoundPool( std::string name, std::vector<cSound*> _pool) :
	mName(name),
	pool(_pool),
	poolSize(_pool.size())
	{
		currentSound = pool.begin();

		// default values
		mPan = 0.0f;
		mVolume = 1.0f;
		mPitch = 1.0f;
	}
	cSoundPool::~cSoundPool()
	{

	}

	unsigned int cSoundPool::play2d()
	{
		++currentSound;

		if (currentSound == pool.end())
			currentSound = pool.begin();

        if (*currentSound) //this should never be false
        {
            (*currentSound)->setPitch(mPitch);
            (*currentSound)->setVolume(mVolume);
            (*currentSound)->setPan(mPan);
            (*currentSound)->play2d(false);
        }
        else
		{
        	LOGERR("cSound.cpp Sound not found: "<<getName());
            LOGERR("Going to DIE...");

			//NO exceptions on Android when using libEGL:
			//throw;
            exit(-1);
		}

		return (*currentSound)->ID;
	}

	void cSoundPool::stopAll()
	{
		for(currentSound = pool.begin(); currentSound != pool.end(); ++currentSound)
			if ((*currentSound)->isPlaying())
				(*currentSound)->stop();
	}

	void cSoundPool::release()
	{
		for(currentSound = pool.begin(); currentSound != pool.end(); ++currentSound)
			(*currentSound)->release();

		pool.clear();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SOUND VARIANT

	cSoundVariant::cSoundVariant( std::string name, std::vector<cSound*> _pool ) :
	cSoundPool(name, _pool)
	{

	}


	unsigned int cSoundVariant::play2d()
	{
		currentSound = pool.begin();
		std::advance(currentSound, LFA::Math::randomInt(0, pool.size() - 1)); //push the iterator to a random part of the cSoundPool vector

		if (*currentSound) //this should never be false
		{
			(*currentSound)->setPitch(mPitch);
			(*currentSound)->setVolume(mVolume);
			(*currentSound)->setPan(mPan);
			(*currentSound)->play2d(false);
		}
		else
		{
			LOGERR("cSound.cpp Sound not found: "<<getName());
			LOGERR("Going to DIE...");

			//NO exceptions on Android when using libEGL:
			//throw;
			exit(-1);
		}

		return (*currentSound)->ID;
	}



}
