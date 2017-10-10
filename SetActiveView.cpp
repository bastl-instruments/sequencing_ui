/*
 * SetActiveView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetActiveView.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

void SetActiveView::init(unsigned char currentInstrumentIndex, unsigned char currentPanIndex) {

	currentInstrumentIndex_ = currentInstrumentIndex;
	currentPanIndex_ = currentPanIndex;
	panButtons_ .init(SekvojModulePool::buttonMap_getSubStepButtonArray(), 4);
	instrumentButtons_.init(SekvojModulePool::buttonMap_getInstrumentButtonArray(), 6);
	instrumentButtons_.setSelectedButton(currentInstrumentIndex);
	stepButtons_.init(SekvojModulePool::buttonMap_->getStepButtonArray(), 16);
	updateConfiguration();
}

void SetActiveView::updateConfiguration() {

	unsigned char instrument = 0;
	bool instrumentSelected = instrumentButtons_.getSelectedButton(instrument);

	unsigned char activeTillPan = (SekvojModulePool::memory_->getNumberOfActives(instrument) - 1) / 16;
	if (instrumentSelected) {
		for (unsigned char instrumentIndex = 0; instrumentIndex < 6; instrumentIndex++) {
			unsigned char instrumentActive = (SekvojModulePool::memory_->getNumberOfActives(instrument) - 1) / 16;
			if (instrumentActive > activeTillPan) {
				activeTillPan = instrumentActive;
			}
		}
	}
	for (unsigned char i = 0; i < 4; i++) {
		ILEDHW::LedState newLEDState = ILEDHW::ON;
		if (i != currentPanIndex_) {
			newLEDState = (i <= activeTillPan) ? ILEDHW::DULLON : ILEDHW::OFF;
		}
		LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_getSubStepButtonIndex(i), newLEDState);
	}

	for (unsigned char i = 0; i < 6; i++) {
		SekvojModulePool::instrumentBar_->setInstrumentSelected(i, ((i == instrument) || !instrumentSelected));
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

	unsigned char numberOfActives = SekvojModulePool::memory_->getNumberOfActives(currentInstrumentIndex_);

	IStepMemory::ActiveMultiStatus statuses[16];
	SekvojModulePool::memory_->getAllInstrumentActivesFor16Steps(currentPanIndex_ * 16, statuses);
	unsigned char panOffset = currentPanIndex_ * 16;

	for (unsigned char i = 0; i < 16; i++) {
		ILEDHW::LedState state = ((panOffset + i) < numberOfActives) ? ILEDHW::ON : ILEDHW::OFF;
		unsigned char instrument;
		if (!instrumentButtons_.getSelectedButton(instrument)) {
			state = getLEDStateFromActiveMultiStatus(statuses[i]);
		}
		LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_->getStepButtonIndex(i), state);
		stepButtons_.setStatus(i, state == ILEDHW::ON);
	}
}

void SetActiveView::setActiveUpTo(unsigned char stepTo, bool instrumentSelected) {
	if (instrumentSelected) {
		SekvojModulePool::memory_->makeActiveUpTo(currentInstrumentIndex_, stepTo);
		SekvojModulePool::player_->changeActivesForCurrentStep(currentInstrumentIndex_, stepTo + 1);
	} else {
		SekvojModulePool::memory_->makeAllInstrumentsActiveUpTo(stepTo);
		SekvojModulePool::player_->changeActivesForCurrentStepInAllInstrunents(stepTo + 1);
	}
}

void SetActiveView::update() {

	unsigned char newInstrument = 0;
	bool wasInstrumentSelected = instrumentButtons_.getSelectedButton(newInstrument);

	panButtons_.update();
	instrumentButtons_.update();
	stepButtons_.update();

	bool isInstrumentSelected = instrumentButtons_.getSelectedButton(newInstrument);
	if ((isInstrumentSelected != wasInstrumentSelected) ||
	    (isInstrumentSelected && (currentInstrumentIndex_ != newInstrument))) {
		currentInstrumentIndex_ = isInstrumentSelected ? newInstrument : currentInstrumentIndex_;
		currentPanIndex_ = 0;
		panButtons_.setSelectedButton(0);
		updateConfiguration();
		return;
	}
	bool shift = LEDsAndButtonsHWWrapper::isButtonDown(SekvojModulePool::buttonMap_->getFunctionButtonIndex());
	if (shift) {

		//We take the highest currently pressed pan
		//to increase number of active steps up to that pan
		for (unsigned char pan = 0; pan < 4; pan++) {
			if (LEDsAndButtonsHWWrapper::isButtonDown(SekvojModulePool::buttonMap_getSubStepButtonIndex(pan))) {
				setActiveUpTo(((pan + 1) * 16) - 1, isInstrumentSelected);
				updateConfiguration();
				break;
			}
		}
		//Change back selected pan since in shift mode changes are not allowed
		panButtons_.setSelectedButton(currentPanIndex_);
	} else {
		unsigned char newPan = 0;
		if (panButtons_.getSelectedButton(newPan) && currentPanIndex_ != newPan) {
			currentPanIndex_ = newPan;
			updateConfiguration();
			return;
		}
	}
	for (unsigned char i = 0; i < 16; i++) {

		// Check whether the status of the step button has been changed
		bool lastState = GETBIT(currentStatuses_, i);
		bool newState = LEDsAndButtonsHWWrapper::hw_->getButtonState(SekvojModulePool::buttonMap_->getStepButtonIndex(i));
		bool stateChanged =  newState != lastState;
		SETBIT(currentStatuses_, i, newState);

		unsigned char pressedStep = (currentPanIndex_ * 16) + i;
		if (stateChanged) {
			//Update
			setActiveUpTo(pressedStep, isInstrumentSelected);
			updateConfiguration();
		}
	}
}

