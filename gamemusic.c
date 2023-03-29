#include "gamemusic.h"
#include <c64/sid.h>

#pragma section( music, 0)

#pragma region( music, 0xa000, 0xc000, , , {music} )

#pragma data(music)

__export char music[] = {
	#embed 0x2000 0x7e "BunkerGame.sid" 
};

// 00 - Title
// 01 - Radio Recieved
// 02 - Blank
// 30:33 duration
// 03 - General Tune 1
// 04 - Under Attack
// 05 - Running low on water/food

#pragma data(data)

unsigned	tune_count, tune_length;
Tune		tune_queue, tune_current;
char		music_throttle;
bool		music_enabled = true;

#define TUNE_LENGTH(m, s, ms)	(3000 * (m) + 50 * (s) + (ms) / 20)

unsigned music_lengths[] = {
	TUNE_LENGTH(2, 33, 0),
	TUNE_LENGTH(2, 33, 0),
	TUNE_LENGTH(2, 33, 0),

	TUNE_LENGTH(0, 30, 700),
	TUNE_LENGTH(0, 30, 700),
	TUNE_LENGTH(0, 30, 700),

};

void music_init(Tune tune)
{
	tune_queue = tune_current = tune;
	tune_length = music_lengths[tune_current];
	tune_count = 0;

	__asm
	{
		lda		tune
		jsr		$a000
	}
}

void music_queue(Tune tune)
{
	tune_queue = tune;
}

void music_play(void)
{
	if (!music_enabled)
		return;
#if 0
	// Skip every sixth frame if in NTSC to adapt
	// playback speed
	if (ntsc)
	{
		music_throttle++;
		if (music_throttle == 6)
		{
			music_throttle = 0;
			return;
		}
	}
#endif
	// Count length of track and switch to next
	// track at end

	tune_count++;
	if (tune_count > tune_length)
	{
		if (tune_current != tune_queue)
		{
			tune_current = tune_queue;
			tune_length = music_lengths[tune_current];

			// Start next track
			__asm
			{
				lda		tune_queue
				jsr		$a000
			}
		}

		tune_count = 0;
	}
	else
	{
		__asm
		{
			jsr		$a003
		}	
	}
}

void music_patch_voice3(bool enable)
{
	*(char *)0xa14e = enable ? 0x20 : 0x4c;
}

void music_toggle(void)
{
	if (music_enabled)
	{
		music_enabled = false;
		sid.voices[0].ctrl = 0;
		sid.voices[1].ctrl = 0;
		sid.voices[2].ctrl = 0;
	}
	else
		music_enabled = true;
}
