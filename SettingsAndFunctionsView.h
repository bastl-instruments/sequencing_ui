
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
#include <ITapper.h>
#include "SekvojRackSDPreset.h"

class SettingsAndFunctionsView : public IView {
public:
	SettingsAndFunctionsView();
	~SettingsAndFunctionsView();
	void init(ILEDsAndButtonsHW * hw, PlayerSettings * settings, InstrumentBar * instrumentBar,
			  IButtonMap * buttonMap, IStepMemory * memory, unsigned char selectedInstrument,
			  unsigned char selectedBar, Player * player, ITapper * tapper, SekvojRackSDPreset * sd);
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

	IStepMemory * memory_;
	unsigned char selectedInstrument_;
	unsigned char selectedBar_;

	Player * player_;
	ITapper * tapper_;
	SekvojRackSDPreset * sd_;

	Switches playModeSwitch_;
	void reflectQuantizationSettings();
	void paste(unsigned char fromInstrument,
			   unsigned char toInstrument,
			   unsigned char fromBar,
			   unsigned char toBar,
			   unsigned int size);
	static bool copyDefined;
	static unsigned char copyPattern;
	static unsigned char copyInstrument;
	static unsigned char copyBar;

};

#endif /* SETTINGSANDFUNCTIONSVIEW_H_ */
