/*
 * SetStepView.h
 *
 *  Created on: Jul 27, 2014
 *      Author: Martin Baar
 */

#ifndef SETSTEPVIEW_H_
#define SETSTEPVIEW_H_

#include <IView.h>
#include <IHWLayer.h>
#include <IStepMemory.h>
#include <RadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "DrumStepsView.h"
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SetStepView : public IView {
public:
	SetStepView();
	~SetStepView();
	void init(IHWLayer * hw, IStepMemory * memory, Player * player, InstrumentBar * instrumentBar,
			IButtonMap * buttonMap, unsigned char pattern, unsigned char instrumentCount, bool useVelocities);
	void update();
	void updateMutes();
	void updateConfiguration();
	void updateVelocity();
private:
	IHWLayer * hw_;
	IStepMemory * memory_;
	Player * player_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	unsigned char currentPattern_;
	unsigned char currentPanIndex_;
	unsigned char currentInstrumentIndex_;
	DrumStep::DrumVelocityType currentVelocity_;

	unsigned int currentStatuses_;

	RadioButtons * panButtons_;
	RadioButtons * instrumentButtons_;
	RadioButtons * velocityRadio_;
	Switches subStepSwitches_;
	DrumStepsView * drumStepView_;

	bool inSubStepMode_;

	unsigned char instrumentCount_;
	bool useVelocities_;

};

#endif /* SETSTEPVIEW_H_ */
