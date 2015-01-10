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

#define UNDO 4
#define COPY 5
#define TAP_TEMPO 6
#define TEMPO_UP 7

#define PASTE_INSTRUMENT 8
#define PASTE_BAR 9
#define PASTE_PATTERN 10
#define TEMPO_DOWN 11

#define CLEAR_STEPS_FOR_INSTRUMENT 12
#define CLEAR_STEPS_FOR_ALL_INSTRUMENTS 13
#define CLEAR_ACTIVES_FOR_INSTRUMENT 14
#define CLEAR_ACTIVES_FOR_ALL_INSTRUMENTS 15

bool SettingsAndFunctionsView::copyDefined = false;
unsigned char SettingsAndFunctionsView::copyPattern  = 0;
unsigned char SettingsAndFunctionsView::copyInstrument = 0;
unsigned char SettingsAndFunctionsView::copyBar = 0;

SettingsAndFunctionsView::SettingsAndFunctionsView() : hw_(0),
								 settings_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 quantizationButtons_(0),
								 multiplierButtons_(0),
								 buttonStatuses_(0),
								 memory_(0),
								 selectedInstrument_(0),
								 selectedBar_(0),
								 player_(0),
								 tapper_(0),
								 sd_(0){
}

SettingsAndFunctionsView::~SettingsAndFunctionsView() {
	delete quantizationButtons_;
	delete multiplierButtons_;
	instrumentBar_->setActive(true);
}


void SettingsAndFunctionsView::init(ILEDsAndButtonsHW * hw, PlayerSettings * settings,
						 	 	 	InstrumentBar * instrumentBar, IButtonMap * buttonMap,
						 	 	 	IStepMemory * memory, unsigned char selectedInstrument,
						 	 	 	unsigned char selectedBar, Player * player, ITapper * tapper,
						 	 	 	SekvojRackSDPreset * sd) {
	hw_ = hw;
	sd_ = sd;
	tapper_ = tapper;
	settings_ = settings;
	instrumentBar_ = instrumentBar;
	instrumentBar_->setActive(false);
	buttonMap_ = buttonMap;
	memory_ = memory;
	selectedInstrument_ = selectedInstrument;
	selectedBar_ = selectedBar;
	player_ = player;

	quantizationButtons_ = new LEDRadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	multiplierButtons_ = new LEDRadioButtons(hw_, buttonMap_->getStepButtonArray(), 4);
	instrumentButtons_.init(hw_, buttonMap_->getInstrumentButtonArray(), 6, true);
	playModeSwitch_.init(hw_, buttonMap_->getMainMenuButtonArray() + 2, 1, true);
	bool isMaster = settings_->getPlayerMode() == PlayerSettings::MASTER;
	playModeSwitch_.setStatus(0, isMaster);
	quantizationButtons_->setSelectedButton((char)(settings_->getRecordQuantizationType()));
	multiplierButtons_->setSelectedButton((char)(settings_->getMultiplication()));

	for (unsigned char i = 0; i < 6; i++) {
		bool isOn = settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
		instrumentButtons_.setStatus(i, isOn);
	}
}

void SettingsAndFunctionsView::paste(unsigned char fromInstrument,
									 unsigned char toInstrument,
									 unsigned char fromBar,
									 unsigned char toBar,
									 unsigned int size) {
	unsigned int fromMemoryIndex = (unsigned int)fromInstrument * 48 + (unsigned int)fromBar * 12;
	unsigned int toMemoryIndex = (unsigned int)toInstrument * 48 + (unsigned int)toBar * 12;

	unsigned char * data = memory_->getDataReference();
	if (copyDefined) {
		if (copyPattern != settings_->getCurrentPattern()) {
				sd_->loadData(copyPattern, fromMemoryIndex, toMemoryIndex, data, size);
		} else {
			for (unsigned int i = 0; i < size; i++) {
				data[toMemoryIndex + i] = data[fromMemoryIndex + i];
			}
		}
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
			settings_->setDrumInstrumentEventType(i, newType);
		}
	}

	bool saveButtonDown = hw_->isButtonDown(buttonMap_->getRecordButtonIndex());
	if (!saveWasDown_ && saveButtonDown) {
		sd_->setPatternData(settings_->getCurrentPattern(), memory_->getDataReference());
		sd_->save();
	}
	saveWasDown_ = saveButtonDown;

	//Other functions
	for (unsigned char button = 4; button < 16; button++) {
		bool buttonWasDown = GETBIT(buttonStatuses_, button);
		bool buttonIsDown = hw_->isButtonDown(buttonMap_->getStepButtonIndex(button));
		if (!buttonWasDown && buttonIsDown)  {
			unsigned char currentBPM = settings_->getBPM();
			switch (button) {
				case TEMPO_DOWN:
					settings_->setBPM(currentBPM - 1);
					break;
				case TEMPO_UP:
					settings_->setBPM(currentBPM + 1);
					break;
				case TAP_TEMPO:
					tapper_->tap(hw_->getElapsedBastlCycles());
					break;
				case COPY:
					copyPattern = settings_->getCurrentPattern();
					copyInstrument = selectedInstrument_;
					copyBar = selectedBar_;
					copyDefined = true;
					break;
				case PASTE_INSTRUMENT:
					paste(copyInstrument, selectedInstrument_,0 , 0, 48);
					break;
				case PASTE_BAR:
					paste(copyInstrument, selectedInstrument_, copyBar, selectedBar_, 12);
					break;
				case PASTE_PATTERN:
					paste(0, 0, 0, 0, 290);
					break;
				case UNDO:
					sd_->discard();
					sd_->getPatternData(settings_->getCurrentPattern(), memory_->getDataReference());
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



