
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
	void init(unsigned char selectedInstrument, unsigned char selectedBar);
	void update();
private:

	LEDRadioButtons quantizationButtons_;
	LEDRadioButtons multiplierButtons_;

	Switches instrumentButtons_;

	unsigned char currentInstrumentEventTypes_;
	unsigned int buttonStatuses_;

	unsigned char selectedInstrument_;
	unsigned char selectedBar_;
	unsigned char blinksToDo_;
	unsigned char cyclesFromLastBlinkSwitch_;

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
