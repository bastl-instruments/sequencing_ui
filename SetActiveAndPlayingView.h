
/*
 * SetActiveView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef SETACTIVEANDPLAYINGVIEW_H_
#define SETACTIVEANDPLAYINGVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <IStepMemory.h>
#include <RadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SetActiveAndPlayingView : public IView {
public:
	SetActiveAndPlayingView();
	~SetActiveAndPlayingView();
	void init(	ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, PlayerSettings * settings, InstrumentBar * instrumentBar,
			IButtonMap * buttonMap, unsigned char pattern);
	void update();
	void updateActives();
	void updateConfiguration();
private:
	ILEDsAndButtonsHW * hw_;
	IStepMemory * memory_;
	Player * player_;
	PlayerSettings * settings_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	unsigned char currentPattern_;
	unsigned char currentPanIndex_;
	unsigned char currentInstrumentIndex_;

	unsigned int currentStatuses_;

	RadioButtons panButtons_;
	Switches instrumentButtons_;

};

#endif /* SETACTIVEANDPLAYINGVIEW_H_ */
