/*
 * PatternView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef PATTERNVIEW_H_
#define PATTERNVIEW_H_

#include <IView.h>
#include <IStepMemory.h>
#include <LEDRadioButtons.h>
#include <Switches.h>
#include <PlayerSettings.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"
#include <ILEDsAndButtonsHW.h>
#include <Player.h>

class PatternView  : public IView {
public:
	void init();
	virtual void update();
private:

	//Controls
	Switches instrumentSwitches_;
	LEDRadioButtons patternSelectRadioButtons_;
	LEDRadioButtons panSelectRadioButtons_;
	unsigned char currentPattern_;
	unsigned char currentPan_;

	void updatePan();
};

#endif /* PATTERNVIEW_H_ */
