
#ifndef _CSOUND_H
#define _CSOUND_H

#include <string>
#include <vector>
#include "cSoundData.h"

#if LFA_AUDIO_ENGINE == LFA_HSS
	#include "HSS.h"
#elif LFA_AUDIO_ENGINE == LFA_FMOD
	#include "fmod.hpp"
	#include "fmod_errors.h"
#endif

namespace LFA
{

    /**
    *  cSound class.
    *  Use this class to control (play/pause/stop) a sound instance. \n
    *  You can modify on the fly the volume/pitch/pan of this sound. \n
    */
    class cSound
    {
        friend class cSoundManager;

    public:
        std::string getName();
		const unsigned int ID;  //automatically assigned by the manager. can be used as a more efficient alternative to the std::string name, and to protect underlying behavior by classes that manage cSounds

        void setPitch(float pitch);
        float getPitch();

        void setVolume(float volume);
        float getVolume();

        void setPan(float pan);
        float getPan();

        void play2d(bool loop = false);
        void stop();
        void pause();

        bool isPlaying();
        bool isStopped();
        bool isPaused();
        bool isLooping();

        void release();

	#if LFA_AUDIO_ENGINE == LFA_HSS
			static s32 channelCallback(hssChannel *channel, hssChannelCallbackType type, void *userdata);
			hssChannel *mChannel;
	#elif LFA_AUDIO_ENGINE == LFA_FMOD
			FMOD::Channel  *mChannel;
	#endif


    private:
        cSound(std::string name, unsigned int soundID);
        ~cSound();

        cSoundData *mData;

        std::string mName;
        float mPitch, mVolume, mPan;
        bool mLooping;
    };


    /**
    *  cSoundPool class.
    *  Use this class to control (play/pause/stop) a repetitive sound instance that you want to mix when playe2d() is called before the last play2d is done. \n
    *  Modifying the volume/pitch/pan of this soundPool will only affect the next sound played by it. \n
    */
	class cSoundPool
	{
		friend class cSoundManager;
	public:

		 std::string getName() {return mName;}

		 void setPitch(float pitch) {mPitch = pitch;}
		 float getPitch() {return mPitch;}

		void setVolume(float volume)  {mVolume = volume;}
		float getVolume() {return mVolume;}

		void setPan(float pan) {mPan = pan;}
		float getPan() {return mPan;}

		unsigned int play2d();
		void stopAll();

		void release();
	
		unsigned int getPoolSize() {return poolSize;}

	protected:
		
		cSoundPool(std::string name, std::vector<cSound*> _pool);
		~cSoundPool();

		const std::string mName;
		const unsigned int poolSize;	
		std::vector<cSound*> pool;
		std::vector<cSound*>::iterator currentSound;
		
		float mPitch, mVolume, mPan;
	};


    /**
    *  cSoundVariant class.
    *  Use this class to control (play/pause/stop) a repetitive sound instance that has variant sound assets that you want to mix when playe2d() is called before the last play2d is done. \n
    *  Modifying the volume/pitch/pan of this cSoundVariant will only affect the next sound played by it. \n
	*  The sound to play is chosen randomly from the given cSoundPool list in the constructor (chosen in cSoundManager)
    */

	class cSoundVariant :  cSoundPool
	{
		friend class cSoundManager;
	public:
		unsigned int play2d();

	private:
		cSoundVariant(std::string name, std::vector<cSound*> _pool);

	};

}


#endif
