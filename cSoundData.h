
#ifndef _CSOUNDINTERNALDATA_H
#define _CSOUNDINTERNALDATA_H
#include "Config.h"

#if LFA_AUDIO_ENGINE == LFA_HSS
#include "HSS.h"
#elif LFA_AUDIO_ENGINE == LFA_FMOD
#include "fmod.hpp"
#include "fmod_errors.h"
#endif

namespace LFA
{

    /**
    *  cSoundData class.
    *  This class is used by the cSoundManager to link a Hekkus sound object \n
    *  to a cSound object. It basically store the sound data. \n\n
    *  Don't use it directly.
    */
    class cSoundData
    {
    public:
#if LFA_AUDIO_ENGINE == LFA_HSS
        hssSound *mSound;
#elif LFA_AUDIO_ENGINE == LFA_FMOD
		FMOD::Sound *mSound;
#endif
        std::string mPathFile;
        int mInstanceCounter;
    };

}


#endif
