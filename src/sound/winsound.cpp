/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <windows.h>
#include "sound/ring_sound.h"
#include "sound/pm_sound.h"
#include "alsound.h"


ALsound::ALsound()
{
}

ALsound::~ALsound()
{
	PlaySound(NULL, 0, 0); // STOP ANY PLAYING SOUND
}

void ALsound::ring()
{
	PlaySound((LPCSTR)ring_sound_data, NULL, SND_MEMORY | SND_ASYNC);
}
void ALsound::pm()
{
	PlaySound((LPCSTR)pm_sound_data, NULL, SND_MEMORY | SND_ASYNC);
}

ALsound& slsound()
{
	static ALsound snd;
	return snd;
}
