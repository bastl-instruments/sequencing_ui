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

class PatternView  : public IView {
public:
	PatternView();
	~PatternView();
	void init(ILEDsAndButtonsHW * hw, PlayerSettings * settigns, IStepMemory * memory, InstrumentBar * instrumentBar, IButtonMap * buttonMap);
	virtual void update();
private:

	ILEDsAndButtonsHW * hw_;
	PlayerSettings * settings_;
	IStepMemory * memory_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	//Controls
	Switches instrumentSwitches_;
	LEDRadioButtons * patternSelectRadioButtons_;
	LEDRadioButtons * panSelectRadioButtons_;
	unsigned char currentPattern_;
	unsigned char currentPan_;

	void updatePan();
};

#endif /* PATTERNVIEW_H_ */
