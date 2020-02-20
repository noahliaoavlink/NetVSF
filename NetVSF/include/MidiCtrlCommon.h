#pragma once

#ifndef _MidiCtrlCommon_H_
#define _MidiCtrlCommon_H_

enum MidiCtrlCommand
{
	MCC_STOP = 101,
	MCC_PLAY,
	MCC_PAUSE,
	MCC_UPDATE_TIME_CODE,
};

enum MSLauncherCommand
{
	MSLC_EXIT = 201,
	MSLC_INIT_PANNEL,   //Pannel
	MSLC_INIT_TIMELINE,
	MSLC_INIT_MEDIA_CTRL,
	MSLC_INIT_SHAPE,
	MSLC_INITIALIZE_HAS_FINISHED,  //initialize has finished
};

#endif
