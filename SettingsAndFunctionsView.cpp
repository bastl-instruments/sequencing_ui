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

#define CLOCK_INPUT_MULTIPLIER_4 0 // being set in the setup at the moment
#define CLOCK_INPUT_MULTIPLIER_8 1
#define CLOCK_INPUT_MULTIPLIER_16 2
#define CLOCK_INPUT_MULTIPLIER_32 3

//here instead of 5 tempo jumps might come something else when we have idea what else it could be
#define TEMPO_DOWN_5 4 	// jump by 5 BPM
#define TEMPO_DOWN 5 	// jump by 1 BPM
#define TEMPO_UP 6 		// jump by 1 BPM
#define TEMPO_UP_5 7 	// jump by 5 BPM

#define COPY 8				// remembers current pattern, instrument and pan
#define PASTE 9				// checks last action (selection of pattern, instrument or pan) and copies to the current one
#define SAVE 10				// saves to card and remembers  adresses of patterns
#define DISCARD_CHANGES 11	// not ready yet - but changes adresses of patterns

#define CLEAR_STEPS_FOR_INSTRUMENT 12 			// put everything to default including actives
#define CLEAR_STEPS_FOR_ALL_INSTRUMENTS 13 		// delete just steps
#define CLEAR_ACTIVES_FOR_INSTRUMENT 14 		// delete just steps
#define CLEAR_ACTIVES_FOR_ALL_INSTRUMENTS 15	// all actives to default state



SettingsAndFunctionsView::SettingsAndFunctionsView() : hw_(0),
								 settings_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 quantizationButtons_(0),
								 multiplierButtons_(0),
								 buttonStatuses_(0),
								 memory_(0),
								 selectedInstrument_(0),
								 player_(0) {
}

SettingsAndFunctionsView::~SettingsAndFunctionsView() {
	delete quantizationButtons_;
	delete multiplierButtons_;
	instrumentBar_->setActive(true);
}


void SettingsAndFunctionsView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settings,
						 	 	 	InstrumentBar * instrumentBar, IButtonMap * buttonMap,
						 	 	 	IStepMemory * memory, unsigned char selectedInstrument,
						 	 	 	Player * player) {
	hw_ = hw;
	settings_ = settings;
	instrumentBar_ = instrumentBar;
	instrumentBar_->setActive(false);
	buttonMap_ = buttonMap;
	memory_ = memory;
	selectedInstrument_ = selectedInstrument;
	player_ = player;
	quantizationButtons_ = new LEDRadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	multiplierButtons_ = new LEDRadioButtons(hw_, buttonMap_->getStepButtonArray(), 4);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	playModeSwitch_.init(hw_, buttonMap_->getMainMenuButtonArray() + 4, 1);
	bool isMaster = settings_->getPlayerMode() == PlayerSettings::MASTER;
	playModeSwitch_.setStatus(0, isMaster);
	hw_->setLED(buttonMap_->getMainMenuButtonIndex(4), isMaster ? ILEDHW::ON : ILEDHW::OFF);
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
	playModeSwitch_.update();

	//update player mode
	bool settingsWasMaster = settings_->getPlayerMode() == PlayerSettings::MASTER;
	if (playModeSwitch_.getStatus(0) != settingsWasMaster) {
		settings_->setPlayerMode(settingsWasMaster ? PlayerSettings::SLAVE : PlayerSettings::MASTER);
		hw_->setLED(buttonMap_->getMainMenuButtonIndex(4), settingsWasMaster ? ILEDHW::OFF : ILEDHW::ON);
	}

	//Quantization settings
	unsigned char quantizationIndex = 0;
	if (quantizationButtons_->getSelectedButton(quantizationIndex)) {
		settings_->setRecordQuantizationType((PlayerSettings::QuantizationType)(quantizationIndex));
	} else {
		quantizationButtons_->setSelectedButton((char)(settings_->getRecordQuantizationType()));
	}

	//Multiplication settings
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

	//Other functions
	for (unsigned char button = 4; button < 16; button++) {
		bool buttonWasDown = GETBIT(buttonStatuses_, button);
		bool buttonIsDown = hw_->getButtonState(buttonMap_->getStepButtonIndex(button)) == IButtonHW::DOWN;
		if (!buttonWasDown && buttonIsDown)  {
			unsigned int currentBPM = settings_->getBPM();
			switch (button) {
			case TEMPO_DOWN_5: 	//jump by 5 BPM
				settings_->setBPM(currentBPM - 5);
				break;
			case TEMPO_DOWN: 	//jump by 1 BPM
				settings_->setBPM(currentBPM - 1);
				break;
			case TEMPO_UP: 		//jump by 1 BPM
				settings_->setBPM(currentBPM + 1);
				break;
			case TEMPO_UP_5: 	//jump by 5 BPM
				settings_->setBPM(currentBPM + 5);
				break;
			case CLEAR_STEPS_FOR_INSTRUMENT:		// put everything to default including actives
				memory_->clearStepsForInstrument(selectedInstrument_);
				break;
			case CLEAR_STEPS_FOR_ALL_INSTRUMENTS: 	// delete just steps
				memory_->clearStepsForAllInstruments();
				break;
			case CLEAR_ACTIVES_FOR_INSTRUMENT: 		// delete just steps
				memory_->makeActiveUpTo(selectedInstrument_, 15);
				player_->changeActivesForCurrentStep(selectedInstrument_, 16);
				break;
			case CLEAR_ACTIVES_FOR_ALL_INSTRUMENTS:
				memory_->makeAllInstrumentsActiveUpTo(15);
				player_->changeActivesForCurrentStepInAllInstrunents(16);
				break;

			}
		}
		SETBIT(buttonStatuses_, button, buttonIsDown);

	}
}



