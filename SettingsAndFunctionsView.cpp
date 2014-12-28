/*
 * SettingsAndFunctionsView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SettingsAndFunctionsView.h"
#include <BitArrayOperations.h>

#define MASTER_SLAVE //PLAY_BUTTON

#define QUANTISATION_SUBSTEPS_1 //pan button 1
#define QUANTISATION_SUBSTEPS_2 //pan button 2
#define QUANTISATION_SUBSTEPS_4 //pan button 3
#define QUANTISATION_SUBSTEPS_8 //pan button 4

//the numbers represent step number

#define CLOCK_INPUT_MULTIPLIER_4 1 // being set in the setup at the moment
#define CLOCK_INPUT_MULTIPLIER_8 2
#define CLOCK_INPUT_MULTIPLIER_16 3
#define CLOCK_INPUT_MULTIPLIER_32 4

//here instead of 5 tempo jumps might come something else when we have idea what else it could be
#define TEMPO_DOWN_5 5 	//jump by 5 BPM
#define TEMPO_DOWN 6 	//jump by 1 BPM
#define TEMPO_UP 7 		//jump by 1 BPM
#define TEMPO_UP_5 8 	//jump by 5 BPM

#define COPY 9				//remembers current pattern, instrument and pan
#define PASTE 10			// checks last action (selection of pattern, instrument or pan) and copies to the current one
#define SAVE 11				// saves to card and remembers  adresses of patterns
#define DISCARD_CHANGES 12	// not ready yet - but changes adresses of patterns

#define CLEAR_PATTERN 13 //put everything to default including actives
#define CLEAR_INSTRUMENT 14 // delete just steps
#define CLEAR_ACTIVES_FOR_INSTRUMENT 15 // delete just steps
#define CLEAR_ACTIVES_FOR_ALL_INSTRUMENTS 16//all actives to default state



SettingsAndFunctionsView::SettingsAndFunctionsView() : hw_(0),
								 settings_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 quantizationButtons_(0),
								 multiplierButtons_(0) {
}

SettingsAndFunctionsView::~SettingsAndFunctionsView() {
	delete quantizationButtons_;
	delete multiplierButtons_;
	instrumentBar_->setActive(true);
}


void SettingsAndFunctionsView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settings,
						 	 	 	 	InstrumentBar * instrumentBar, IButtonMap * buttonMap) {
	hw_ = hw;
	settings_ = settings;
	instrumentBar_ = instrumentBar;
	instrumentBar_->setActive(false);
	buttonMap_ = buttonMap;
	quantizationButtons_ = new LEDRadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	multiplierButtons_ = new LEDRadioButtons(hw_, buttonMap_->getStepButtonArray(), 4);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);

	quantizationButtons_->setSelectedButton((char)(settings_->getRecordQuantizationType()));
	multiplierButtons_->setSelectedButton((char)(settings_->getMultiplication()));

	for (unsigned char i = 0; i < 6; i++) {
		bool isOn = settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
		hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), isOn ? ILEDHW::ON : ILEDHW::OFF);
		instrumentButtons_.setStatus(i, isOn);
	}

}

void SettingsAndFunctionsView::update() {

	instrumentButtons_.update();
	quantizationButtons_->update();
	multiplierButtons_->update();

	unsigned char quantizationIndex = 0;
	if (quantizationButtons_->getSelectedButton(quantizationIndex)) {
		settings_->setRecordQuantizationType((PlayerSettings::QuantizationType)(quantizationIndex));
	} else {
		quantizationButtons_->setSelectedButton((char)(settings_->getRecordQuantizationType()));
	}

	unsigned char multiplierIndex = 0;
	if (multiplierButtons_->getSelectedButton(multiplierIndex)) {
		settings_->setMultiplication((PlayerSettings::MultiplicationType)(multiplierIndex));
	} else {
		multiplierButtons_->setSelectedButton((char)(settings_->getMultiplication()));
	}

	// Update playing instruments settings
	for (unsigned char i = 0; i < 6; i++) {
		PlayerSettings::DrumInstrumentEventType currentType = settings_->getDrumInstrumentEventType(i);
		PlayerSettings::DrumInstrumentEventType newType = instrumentButtons_.getStatus(i) ?
				PlayerSettings::GATE : PlayerSettings::TRIGGER;
		if (currentType != newType) {
			hw_->setLED(buttonMap_->getInstrumentButtonIndex(i), newType == PlayerSettings::GATE ? ILEDHW::ON : ILEDHW::OFF);
			settings_->setDrumInstrumentEventType(i, newType);
		}
	}
}



