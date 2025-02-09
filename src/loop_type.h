//
// Created by Nicholas Newdigate on 25/04/2021.
//

#ifndef TEENSY_RESAMPLING_LOOP_TYPE_H
#define TEENSY_RESAMPLING_LOOP_TYPE_H

typedef enum loop_type {
    looptype_none,
    looptype_repeat,
    looptype_pingpong
} loop_type;

typedef enum play_start {
    play_start_sample,  	// play starts at sample start
    play_start_loop,		// play starts at loop start
	play_start_arbitrary,	// play starts at arbitrary point set by user
} play_start;

#endif //TEENSY_RESAMPLING_LOOP_TYPE_H
