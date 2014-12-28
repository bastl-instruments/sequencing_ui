
/*
 * SetActiveView.h
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#ifndef SETTINGSANDFUNCTIONSVIEW_H_
#define SETTINGSANDFUNCTIONSVIEW_H_

#include <IView.h>
#include <ILEDsAndButtonsHW.h>
#include <IStepMemory.h>
#include <LEDRadioButtons.h>
#include <Switches.h>
#include <Player.h>
#include "InstrumentBar.h"
#include "IButtonMap.h"

class SettingsAndFunctionsView : public IView {
public:
	SettingsAndFunctionsView();
	~SettingsAndFunctionsView();
	void init(ILEDsAndButtonsHW * hw, PlayerSettings * settings, InstrumentBar * instrumentBar,
			  IButtonMap * buttonMap);
	void update();
private:
	ILEDsAndButtonsHW * hw_;
	PlayerSettings * settings_;
	InstrumentBar * instrumentBar_;
	IButtonMap * buttonMap_;

	LEDRadioButtons * quantizationButtons_;
	LEDRadioButtons * multiplierButtons_;

	Switches instrumentButtons_;

	unsigned char currentInstrumentEventTypes_;
	unsigned int buttonStatuses_;
	void reflectQuantizationSettings();
};

#endif /* SETTINGSANDFUNCTIONSVIEW_H_ */
