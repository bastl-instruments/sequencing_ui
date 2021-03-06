/*
 * SetStepView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetStepView.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

void SetStepView::init(unsigned char pattern, unsigned char instrumentCount, unsigned char initialInstrument
					   /*, bool useVelocities*/, unsigned char selectedBar) {

	inSubStepMode_ = false;
	isPlaying_ = true;
	currentPattern_ = pattern;
	currentPanIndex_ = selectedBar;
	instrumentCount_ = instrumentCount;
	//useVelocities_ = useVelocities;
	currentInstrumentIndex_ = initialInstrument;
	panButtons_ .init(SekvojModulePool::buttonMap_getSubStepButtonArray(), 4);
	panButtons_.setSelectedButton(currentPanIndex_);
	subStepSwitches_.init(SekvojModulePool::buttonMap_getSubStepButtonArray(), 4);
	instrumentButtons_.init(SekvojModulePool::buttonMap_getInstrumentButtonArray(), instrumentCount_);
	//if (useVelocities_) {
	//	velocityRadio_ = new RadioButtons(hw_, buttonMap_->getVelocityButtonArray(), 2);
	//}
	drumStepView_.init(SekvojModulePool::buttonMap_);
	updateConfiguration();
	buttonStatuses_ = 0;
}

void SetStepView::updateConfiguration() {
	for (unsigned char i = 0; i < 4; i++) {
		LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		SekvojModulePool::instrumentBar_->setInstrumentSelected(i, i == currentInstrumentIndex_);
	}
	updateMutes();
}

void SetStepView::updateMutes() {
	unsigned char * data;
	SekvojModulePool::memory_->getMutesForNote(currentInstrumentIndex_, currentPanIndex_, data);
	currentStatuses_ = ~((((unsigned int)data[1]) << 8) | data[0]);
	drumStepView_.setStatus(currentStatuses_);
}

/*void SetStepView::updateVelocity() {
	if (useVelocities_) {
		velocityRadio_->update();
		unsigned char newVelocity = 0;
		if (currentVelocity_ != DrumStep::DOWN) hw_->setLED(buttonMap_->getVelocityButtonIndex(0), ILEDHW::OFF);
		if (currentVelocity_ != DrumStep::UP) hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::OFF);
		hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::OFF);

		if (velocityRadio_->getSelectedButton(newVelocity)) {
			if (newVelocity == 0) {
				currentVelocity_ = DrumStep::DOWN;
				hw_->setLED(buttonMap_->getVelocityButtonIndex(0), ILEDHW::ON);
			} else {
				currentVelocity_ = DrumStep::UP;
				hw_->setLED(buttonMap_->getVelocityButtonIndex(1), ILEDHW::ON);
			}
		} else {
			currentVelocity_ = DrumStep::NORMAL;
		}
	}
}*/

void SetStepView::updateCursor() {
	unsigned char nextStep = SekvojModulePool::player_->getCurrentInstrumentStep(currentInstrumentIndex_);
	if (nextStep / 16 == currentPanIndex_ && isPlaying_) {
		drumStepView_.setHighlightedButton(nextStep % 16);
	} else {
		drumStepView_.setHighlightedButton(-1);
	}
}

void SetStepView::update() {

	unsigned char newInstrument = 0;
	bool instrumentWasSelected = instrumentButtons_.getSelectedButton(newInstrument);

	instrumentButtons_.update();
	drumStepView_.update();
	bool jumpButtonDown = LEDsAndButtonsHWWrapper::isButtonDown(SekvojModulePool::buttonMap_->getJumpButtonIndex());
	if (jumpButtonDown) {
		drumStepView_.setIgnoreAll(true);
		unsigned char pressedButton = 0;
	    if (drumStepView_.getDownButton(pressedButton)) {
	    	SekvojModulePool::player_->startLoop(currentPanIndex_ * 4 + pressedButton);
	    	updateCursor();
	    	return;
	    }
	} else {
		drumStepView_.setIgnoreAll(false);
	}
	LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_->getJumpButtonIndex(), jumpButtonDown ? ILEDHW::ON : ILEDHW::OFF);
	SekvojModulePool::player_->stopLoop();

	//updateVelocity();

	newInstrument = 0;
	unsigned char oldInstrument = currentInstrumentIndex_;
	bool instrumentIsSelected = instrumentButtons_.getSelectedButton(newInstrument);

	if (!SekvojModulePool::player_->isPlaying()) {
		for (unsigned char i = 0; i < 6; i++) {
			bool wasButtonDown = getBit(buttonStatuses_,i);
			bool isButtonDown = LEDsAndButtonsHWWrapper::isButtonDown(SekvojModulePool::buttonMap_->getInstrumentButtonIndex(i));
			bool isInstrumentGay = SekvojModulePool::settings_->getDrumInstrumentEventType(i) == PlayerSettings::GATE;
			if (isButtonDown && (!wasButtonDown || isInstrumentGay)) {
				SekvojModulePool::player_->playNote(i, DrumStep::NORMAL, false);
			}
			setBit(buttonStatuses_, i, isButtonDown);
		}
	}

	if (instrumentIsSelected) {
		if (currentInstrumentIndex_ != newInstrument) {
			currentInstrumentIndex_ = newInstrument;
			currentPanIndex_ = 0;
			updateConfiguration();
			return;
		}
	}
	if ((instrumentWasSelected != instrumentIsSelected || currentInstrumentIndex_ != oldInstrument) && !SekvojModulePool::player_->isPlaying()) {
		SekvojModulePool::player_->playNote(currentInstrumentIndex_, DrumStep::NORMAL);
	}

	unsigned char currentButtonDown = 0;
	if (drumStepView_.getDownButton(currentButtonDown) != inSubStepMode_) {
		if (!inSubStepMode_) {
			inSubStepMode_ = true;
			drumStepView_.setIgnoreOffs(false);
			DrumStep step = SekvojModulePool::memory_->getDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown);
			bool anyOn = false;
			for (unsigned char i = 0; i < 4; i++) {
				bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
				anyOn = anyOn || substepHasNote;
				LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_getSubStepButtonIndex(i), substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				subStepSwitches_.setStatus(i, substepHasNote);
			}
			if (!anyOn) {
				for (unsigned char subStep = 0; subStep < 4; subStep++) {
					if (subStep == 0 || SekvojModulePool::settings_->getDrumInstrumentEventType(currentInstrumentIndex_) == PlayerSettings::GATE) {
						step.setSubStep(subStep, DrumStep::NORMAL);
						LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_getSubStepButtonIndex(subStep), ILEDHW::ON);
						subStepSwitches_.setStatus(subStep, true);
					}
				}
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown, step);
			}
		} else {
			inSubStepMode_ = false;
			for (unsigned char i = 0; i < 4; i++) {
				LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
			}
		}
	}
	if (inSubStepMode_) {
		subStepSwitches_.update();
		DrumStep step = SekvojModulePool::memory_->getDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown);

		// Bool to store information if any sub step is on after processing all sub steps in this cycle
		bool anyOn = false;
		for (unsigned char i = 0; i < 4; i++) {
			bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
			if (substepHasNote != subStepSwitches_.getStatus(i)) {
				drumStepView_.setIgnoreOffs(true);
				step.setSubStep(i, substepHasNote ? DrumStep::OFF : DrumStep::NORMAL);
				LEDsAndButtonsHWWrapper::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(i), !substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown, step);
				substepHasNote = !substepHasNote;
			}
			anyOn = anyOn || substepHasNote;
		}
		// Well there are no any sub steps set so step off shall not be ignored
		// because there is nothing to play
		if (!anyOn) {
			drumStepView_.setIgnoreOffs(false);
		}
	} else {
		panButtons_.update();
	}


	unsigned char newPan = 0;
	if (panButtons_.getSelectedButton(newPan)) {
		if (currentPanIndex_ != newPan) {
			currentPanIndex_ = newPan;
			updateConfiguration();
			return;
		}
	} else {
		newPan = SekvojModulePool::player_->getCurrentInstrumentStep(currentInstrumentIndex_) / 16;
		if (currentPanIndex_ != newPan) {
			currentPanIndex_ = newPan;
			updateConfiguration();
			return;
		}
	}

	unsigned int newOffs = drumStepView_.getNewOffs();
	unsigned int newOns = drumStepView_.getNewOns();
	if (newOffs != 0 || newOns != 0) {
		for (unsigned char i = 0; i < 16; i++) {
			bool changeToOn = GETBIT(newOns, i);
			bool changeToOff = GETBIT(newOffs, i);
			if (changeToOn || changeToOff) {
				unsigned char currentStepIndex = (currentPanIndex_ * 16) + i;
				DrumStep step = SekvojModulePool::memory_->getDrumStep(currentInstrumentIndex_, currentStepIndex);
				if (changeToOn) {
					bool anyOn = false;
					for (unsigned char i = 0; i < 4; i++) {
						bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
						anyOn = anyOn || substepHasNote;
					}
					if (!anyOn) {
						for (unsigned char subStep = 0; subStep < 4; subStep++) {
							if (subStep == 0 || SekvojModulePool::settings_->getDrumInstrumentEventType(currentInstrumentIndex_) == PlayerSettings::GATE) {
								step.setSubStep(subStep, DrumStep::NORMAL);
							}
						}
					}
				}
				step.setMuted(changeToOff);
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, currentStepIndex, step);
			}

		}
	}
	updateCursor();
}

