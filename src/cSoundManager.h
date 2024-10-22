
#pragma once
#ifndef _CSOUNDMANAGER_H
#define _CSOUNDMANAGER_H

#include <string>
#include <sstream>
#include <map>
#include "Config.h"

#if LFA_AUDIO_ENGINE == LFA_HSS
	#include "HSS.h"
#endif

#include "cSoundData.h"
#include "cSound.h"


namespace LFA
{

    /**
    *  cSoundManager class \n.
    *  Use this class to control the sound engine and to create/manage cSound objects. \n
    *  This class is built as a Singleton. After you have created it you can request its \n
    *  pointer with the getInstancePtr function.
    */
    class cSoundManager
    {
    public:

        bool init(int frequency, int bps, bool stereo);
		static void destroy(void);

        cSound *createSound(std::string name, std::string filePath, bool stream = false);
		cSoundPool *createSoundPool(std::string name, std::string filePath, unsigned int poolSize = 2);

		// this is expecting an existing file "path/to/file.ogg" ... then will attempt "path/to/file[prefix]1.ogg" to "path/to/file[prefix][variantCount].ogg"
		// example: file.wav, file-v1.wav, file-v2.wav   would require filePathToBaseVariant = "path/to/file.wav" variantCount = 2  prefix = "-v"
		cSoundVariant * createSoundVariant(const std::string &name, const std::string &filePathToBaseVariant, unsigned int variantCount, const std::string &prefix = ""); 

		void releaseSound(cSound *sound);
		void releaseSoundPool(cSoundPool * soundPool);
        cSound *getSound(std::string name);
		cSoundPool *getSoundPool(std::string name);

        void playSound2d(cSound *sound, bool loop = false);
		bool playSound2d(unsigned int soundID, bool loop = false);
		bool stopSound(unsigned int soundID);

		void stopAllSounds(void);

        void suspend();
        void resume();
        void update();

        void releaseSounds();

        static cSoundManager *getInstancePtr();

		void release();

    private:
		cSoundManager();
		~cSoundManager();
		
		cSound * getSound(unsigned int soundID); // this is private to allow classes to use ID's to restrict the direct manipulation of its cSounds (ie soundPool)

        // forbide the use of copy constructor and operator==
        cSoundManager(const cSoundManager &);
        cSoundManager &operator=(const cSoundManager &);

        static cSoundManager *mInstance;

#if LFA_AUDIO_ENGINE == LFA_HSS
        hssSpeaker *mSpeaker;
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		FMOD::System *mSpeaker;
#endif

        std::map<std::string, cSoundData*> mSoundData;
        std::map<std::string, cSound*> mSounds;
		std::map<std::string, cSoundPool*> mSoundPools;

		std::map<std::string, cSound*>::iterator sndItr;

		unsigned int generateNewSoundID(void);
		unsigned int lastID;
    };

}


#endif
