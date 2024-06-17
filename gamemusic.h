#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

enum Tune
{
	TUNE_TITLE,
	TUNE_RADIO_RECEIVED,
	TUNE_BLANK,

	TUNE_THEME_GENERAL_1,
	TUNE_ATTACK_1,
	TUNE_RESOURCES_1,
};

extern Tune		tune_queue, tune_current;
extern bool		music_enabled;

// Initialize playback of next track immediately
void music_init(Tune tune);

// Queue next track
void music_queue(Tune tune);

// Play music, called in interrupt
void music_play(void);

// Toggle music on/off
void music_toggle(void);

// Enable voice three for music
void music_patch_voice3(bool enable);

#pragma compile("gamemusic.c")

#endif
