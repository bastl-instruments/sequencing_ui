/*
 * SetActiveView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetActiveView.h"
#include <BitArrayOperations.h>

SetActiveView::SetActiveView() : hw_(0),
 								 memory_(0),
								 player_(0),
								 instrumentBar_(0),
								 buttonMap_(0),
								 currentPanIndex_(0),
								 currentInstrumentIndex_(0),
								 currentStatuses_(0),
								 panButtons_(0),
								 instrumentButtons_(0){
}

SetActiveView::~SetActiveView() {
	delete panButtons_;
	delete instrumentButtons_;
}

void SetActiveView::init(ILEDsAndButtonsHW * hw, IStepMemory * memory, Player * player, InstrumentBar * instrumentBar,
						 IButtonMap * buttonMap, unsigned char currentInstrumentIndex) {

	hw_ = hw;
	memory_ = memory;
	player_ = player;
	instrumentBar_ = instrumentBar;
	buttonMap_ = buttonMap;
	currentInstrumentIndex_ = currentInstrumentIndex;
	panButtons_ = new RadioButtons(hw_, buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_ = new RadioButtons(hw_, buttonMap_->getInstrumentButtonArray(), 6);
	instrumentButtons_->setSelectedButton(currentInstrumentIndex);
	stepButtons_.init(hw_, buttonMap_->getStepButtonArray(), 16);
	updateConfiguration();
}

void SetActiveView::updateConfiguration() {

	unsigned char instrument = 0;
	bool instrumentSelected = instrumentButtons_->getSelectedButton(instrument);

	bool anyActive[4];
	if (instrumentSelected) {
		memory_->getActiveWindowBitArray(instrument, anyActive);
	} else {
		memory_->getAllInstrumentsActiveWindowBitArray(anyActive);
	}

	for (unsigned char i = 0; i < 4; i++) {
		if (i == currentPanIndex_) {
			hw_->setLED(buttonMap_->getSubStepButtonIndex(i), ILEDHW::ON);
		} else {
			hw_->setLED(buttonMap_->getSubStepButtonIndex(i), anyActive[i] ? ILEDHW::DULLON : ILEDHW::OFF);
		}
	}

	for (unsigned char i = 0; i < 6; i++) {
		instrumentBar_->setInstrumentSelected(i, ((i == instrument) || !instrumentSelected));
	}
	updateActives();
}

ILEDHW::LedState SetActiveView::getLEDStateFromActiveMultiStatus(IStepMemory::ActiveMultiStatus status) {
	switch (status) {
		case IStepMemory::ALLINACTIVE:
			return ILEDHW::OFF;
		case IStepMemory::ALLACTIVE:
			return ILEDHW::ON;
		case IStepMemory::MIXED:
			return ILEDHW::DULLON;
	}
}

void SetActiveView::updateActives() {
	unsigned char instrument;
	unsigned char * data;

	memory_->getActivesAndMutesForNote(currentInstrumentIndex_, currentPanIndex_,  data);

	IStepMemory::ActiveMultiStatus statuses[16];
	memory_->getAllInstrumentActivesFor16Steps(currentPanIndex_ * 16, statuses);

	for (unsigned char i = 0; i < 16; i++) {
		ILEDHW::LedState state = BitArrayOperations::getBit(data[i / 8], i % 8) ? ILEDHW::ON : ILEDHW::OFF;
		if (!instrumentButtons_->getSelectedButton(instrument)) {
			state = getLEDStateFromActiveMultiStatus(statuses[i]);
		}
		hw_->setLED(buttonMap_->getStepButtonIndex(i), state);
		stepButtons_.setStatus(i, state == ILEDHW::ON);
	}
}

void SetActiveView::setActiveUpTo(unsigned char stepTo, bool instrumentSelected) {
	if (instrumentSelected) {
		memory_->makeActiveUpTo(currentInstrumentIndex_, stepTo);
		player_->changeActivesForCurrentStep(currentInstrumentIndex_, stepTo + 1);
	} else {
		memory_->makeAllInstrumentsActiveUpTo(stepTo);
		player_->changeActivesForCurrentStepInAllInstrunents(stepTo + 1);
	}
}

void SetActiveView::update() {

	unsigned char newInstrument = 0;
	bool wasInstrumentSelected = instrumentButtons_->getSelectedButton(newInstrument);

	panButtons_->update();
	instrumentButtons_->update();
	stepButtons_.update();

	bool isInstrumentSelected = instrumentButtons_->getSelectedButton(newInstrument);
	if ((isInstrumentSelected != wasInstrumentSelected) ||
	    (isInstrumentSelected && (currentInstrumentIndex_ != newInstrument))) {
		currentInstrumentIndex_ = isInstrumentSelected ? newInstrument : currentInstrumentIndex_;
		currentPanIndex_ = 0;
		panButtons_->setSelectedButton(0);
		updateConfiguration();
		return;
	}
	bool shift = hw_->getButtonState(buttonMap_->getFunctionButtonIndex()) == IButtonHW::DOWN;
	if (shift) {

		//We take the highest currently pressed pan
		//to increase number of active steps up to that pan
		for (unsigned char pan = 0; pan < 4; pan++) {
			if (hw_->getButtonState(buttonMap_->getSubStepButtonIndex(pan)) == IButtonHW::DOWN) {
				setActiveUpTo(((pan + 1) * 16) - 1, isInstrumentSelected);
				updateConfiguration();
				break;
			}
		}
		//Change back selected pan since in shift mode changes are not allowed
		panButtons_->setSelectedButton(currentPanIndex_);
	} else {
		unsigned char newPan = 0;
		if (panButtons_->getSelectedButton(newPan) && currentPanIndex_ != newPan) {
			currentPanIndex_ = newPan;
			updateConfiguration();
			return;
		}
	}
	for (unsigned char i = 0; i < 16; i++) {

		// Check whether the status of the step button has been changed
		bool lastState = GETBIT(currentStatuses_, i);
		bool newState = hw_->getButtonState(buttonMap_->getStepButtonIndex(i));
		bool stateChanged =  newState != lastState;
		SETBIT(currentStatuses_, i, newState);

		unsigned char pressedStep = (currentPanIndex_ * 16) + i;
		if (stateChanged) {
			//Update
			if (shift) {
				for (unsigned char instrument = 0; instrument < 6; instrument++) {
					if (instrument == currentInstrumentIndex_ || !isInstrumentSelected) {
						DrumStep step = memory_->getDrumStep(instrument, pressedStep);
						step.setActive(stepButtons_.getStatus(i));
						memory_->setDrumStep(instrument, pressedStep, step);
					}
				}
			} else {
				setActiveUpTo(pressedStep, isInstrumentSelected);
			}
			updateConfiguration();
		}
	}
}

