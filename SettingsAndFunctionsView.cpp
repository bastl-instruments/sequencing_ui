/*
 * SettingsAndFunctionsView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SettingsAndFunctionsView.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

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

#define SAVE_NOT_IN_ORDER 3

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

void SettingsAndFunctionsView::init(unsigned char selectedInstrument, unsigned char selectedBar) {
	SekvojModulePool::instrumentBar_->setActive(false);
	selectedInstrument_ = selectedInstrument;
	selectedBar_ = selectedBar;
	blinksToDo_ = 0;

	quantizationButtons_.init(SekvojModulePool::buttonMap_getSubStepButtonArray(), 4);
	multiplierButtons_.init(SekvojModulePool::buttonMap_->getStepButtonArray(), 4);
	instrumentButtons_.init(SekvojModulePool::buttonMap_getInstrumentButtonArray(), 6, true);
	playModeSwitch_.init(SekvojModulePool::buttonMap_->getMainMenuButtonArray() + 2, 1, true);
	bool isMaster = SekvojModulePool::settings_->getPlayerMode() == PlayerSettings::MASTER;
	playModeSwitch_.setStatus(0, isMaster);
	quantizationButtons_.setSelectedButton((char)(SekvojModulePool::settings_->getRecordQuantizationType()));
	multiplierButtons_.setSelectedButton((char)(SekvojModulePool::settings_->getMultiplication()));

	for (unsigned char i = 0; i < 6; i++) {
		bool isOn = SekvojModulePool::settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
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

	if (copyDefined) {
		if (copyPattern != SekvojModulePool::settings_->getCurrentPattern()) {
			SekvojModulePool::sd_.loadData(copyPattern, fromMemoryIndex, toMemoryIndex, size);
		} else {
			unsigned char * data = SekvojModulePool::memory_->getDataReference();
			for (unsigned int i = 0; i < size; i++) {
				data[toMemoryIndex + i] = data[fromMemoryIndex + i];
			}
		}
	}
}

void SettingsAndFunctionsView::update() {

	if (blinksToDo_ != 0) {
		cyclesFromLastBlinkSwitch_++;
		if (cyclesFromLastBlinkSwitch_ > 99) {
			blinksToDo_--;
		}
		cyclesFromLastBlinkSwitch_ = cyclesFromLastBlinkSwitch_ % 100;
		LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_->getRecordButtonIndex(),
		-				cyclesFromLastBlinkSwitch_ / 50 == 0 ? ILEDHW::OFF : ILEDHW::ON);
	}

	instrumentButtons_.update();
	quantizationButtons_.update();
	multiplierButtons_.update();
	playModeSwitch_.update();

	//update player mode
	bool settingsWasMaster = SekvojModulePool::settings_->getPlayerMode() == PlayerSettings::MASTER;
	if (playModeSwitch_.getStatus(0) != settingsWasMaster) {
		SekvojModulePool::settings_->setPlayerMode(settingsWasMaster ? PlayerSettings::SLAVE : PlayerSettings::MASTER);
	}



	//Quantization settings
	unsigned char quantizationIndex = 0;
	if (quantizationButtons_.getSelectedButton(quantizationIndex)) {
		SekvojModulePool::settings_->setRecordQuantizationType((PlayerSettings::QuantizationType)(quantizationIndex));
	} else {
		quantizationButtons_.setSelectedButton((char)(SekvojModulePool::settings_->getRecordQuantizationType()));
	}

	//Multiplication settings
	unsigned char multiplierIndex = 0;
	if (multiplierButtons_.getSelectedButton(multiplierIndex)) {
		SekvojModulePool::settings_->setMultiplication((PlayerSettings::MultiplicationType)(multiplierIndex));
	} else {
		multiplierButtons_.setSelectedButton((char)(SekvojModulePool::settings_->getMultiplication()));
	}

	// Update playing instruments settings
	for (unsigned char i = 0; i < 6; i++) {
		PlayerSettings::DrumInstrumentEventType currentType = SekvojModulePool::settings_->getDrumInstrumentEventType(i);
		PlayerSettings::DrumInstrumentEventType newType = instrumentButtons_.getStatus(i) ?
				PlayerSettings::GATE : PlayerSettings::TRIGGER;
		if (currentType != newType) {
			SekvojModulePool::settings_->setDrumInstrumentEventType(i, newType);
		}
	}

	//Other functions
	for (unsigned char button = 3; button < 16; button++) {
		bool buttonWasDown = GETBIT(buttonStatuses_, button);
		unsigned char buttonIndex = (button == 3) ? SekvojModulePool::buttonMap_->getRecordButtonIndex() :
													SekvojModulePool::buttonMap_->getStepButtonIndex(button);
		bool buttonIsDown = LEDsAndButtonsHWWrapper::isButtonDown(buttonIndex);
		if (!buttonWasDown && buttonIsDown)  {
			unsigned int currentBPM = SekvojModulePool::settings_->getBPM();
			switch (button) {
				case SAVE_NOT_IN_ORDER:
					SekvojModulePool::sd_.setPatternData(SekvojModulePool::settings_->getCurrentPattern());
					SekvojModulePool::sd_.save(SekvojModulePool::settings_->getManipulatedPatternsBitArray());
					SekvojModulePool::sd_.getPatternData(SekvojModulePool::settings_->getCurrentPattern());
					SekvojModulePool::settings_->resetManipulatedPatterns();
					blinksToDo_ = 2;
				break;
				case TEMPO_DOWN:
					if (currentBPM > 0) {
						SekvojModulePool::settings_->setBPM(currentBPM - 1);
					}
					break;
				case TEMPO_UP:
					SekvojModulePool::settings_->setBPM(currentBPM + 1);
					break;
				case TAP_TEMPO:
					SekvojModulePool::tapper_->tap(LEDsAndButtonsHWWrapper::hw_->getElapsedBastlCycles());
					break;
				case COPY:
					copyPattern = SekvojModulePool::settings_->getCurrentPattern();
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
					SekvojModulePool::sd_.discard(SekvojModulePool::settings_->getManipulatedPatternsBitArray());
					SekvojModulePool::sd_.getPatternData(SekvojModulePool::settings_->getCurrentPattern());
					SekvojModulePool::settings_->resetManipulatedPatterns();
					break;
				case CLEAR_STEPS_FOR_INSTRUMENT:		// put everything to default including actives
					SekvojModulePool::memory_->clearStepsForInstrument(selectedInstrument_);
					break;
				case CLEAR_STEPS_FOR_ALL_INSTRUMENTS: 	// delete just steps
					SekvojModulePool::memory_->clearStepsForAllInstruments();
					break;
				case CLEAR_ACTIVES_FOR_INSTRUMENT: 		// delete just steps
					SekvojModulePool::memory_->makeActiveUpTo(selectedInstrument_, 15);
					SekvojModulePool::player_->changeActivesForCurrentStep(selectedInstrument_, 16);
					break;
				case CLEAR_ACTIVES_FOR_ALL_INSTRUMENTS:
					SekvojModulePool::memory_->makeAllInstrumentsActiveUpTo(15);
					SekvojModulePool::player_->changeActivesForCurrentStepInAllInstrunents(16);
					break;
			}
		}
		setBit(buttonStatuses_, button, buttonIsDown);

	}
}



