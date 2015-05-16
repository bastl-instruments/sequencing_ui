/*
 * SetStepView.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: bastl
 */

#include "SetStepView.h"
#include <BitArrayOperations.h>
#include "SekvojModulePool.h"

SetStepView::SetStepView() :     currentPattern_(0),
								 currentPanIndex_(0),
								 currentInstrumentIndex_(0),
								 currentVelocity_(DrumStep::NORMAL),
								 currentStatuses_(0),
								 panButtons_(0),
								 instrumentButtons_(0),
								 //velocityRadio_(0),
								 drumStepView_(0),
								 inSubStepMode_(false),
								 instrumentCount_(10),
								 //useVelocities_(false),
								 isPlaying_(true){
}

SetStepView::~SetStepView() {
	delete panButtons_;
	delete instrumentButtons_;
	//if (useVelocities_) {
	//	delete velocityRadio_;
	//}
	delete drumStepView_;
}

void SetStepView::init(unsigned char pattern, unsigned char instrumentCount, unsigned char initialInstrument
					   /*, bool useVelocities*/, unsigned char selectedBar) {

	currentPattern_ = pattern;
	currentPanIndex_ = selectedBar;
	instrumentCount_ = instrumentCount;
	//useVelocities_ = useVelocities;
	currentInstrumentIndex_ = initialInstrument;
	panButtons_ = new RadioButtons(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getSubStepButtonArray(), 4);
	panButtons_->setSelectedButton(currentPanIndex_);
	subStepSwitches_.init(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getSubStepButtonArray(), 4);
	instrumentButtons_ = new RadioButtons(SekvojModulePool::hw_, SekvojModulePool::buttonMap_->getInstrumentButtonArray(), instrumentCount_);
	//if (useVelocities_) {
	//	velocityRadio_ = new RadioButtons(hw_, buttonMap_->getVelocityButtonArray(), 2);
	//}
	drumStepView_ = new DrumStepsView();
	drumStepView_->init(SekvojModulePool::hw_, SekvojModulePool::buttonMap_);
	updateConfiguration();
}

void SetStepView::updateConfiguration() {
	for (unsigned char i = 0; i < 4; i++) {
		SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
	}
	for (unsigned char i = 0; i < instrumentCount_; i++) {
		SekvojModulePool::instrumentBar_->setInstrumentSelected(i, i == currentInstrumentIndex_);
	}
	updateMutes();
}

void SetStepView::updateMutes() {
	unsigned char * data;
	SekvojModulePool::memory_->getActivesAndMutesForNote(currentInstrumentIndex_, currentPanIndex_, data);
	currentStatuses_ = ~((((unsigned int)data[3]) << 8) | data[2]);
	drumStepView_->setStatus(currentStatuses_);
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
		drumStepView_->setHighlightedButton(nextStep % 16);
	} else {
		drumStepView_->setHighlightedButton(-1);
	}
}

void SetStepView::update() {

	unsigned char newInstrument = 0;
	bool instrumentWasSelected = instrumentButtons_->getSelectedButton(newInstrument);

	instrumentButtons_->update();
	drumStepView_->update();
	bool jumpButtonDown = SekvojModulePool::hw_->isButtonDown(SekvojModulePool::buttonMap_->getJumpButtonIndex());
	if (jumpButtonDown) {
		drumStepView_->setIgnoreAll(true);
		unsigned char pressedButton = 0;
	    if (drumStepView_->getDownButton(pressedButton)) {
	    	SekvojModulePool::player_->startLoop(currentPanIndex_ * 4 + pressedButton);
	    	updateCursor();
	    	return;
	    }
	} else {
		drumStepView_->setIgnoreAll(false);
	}
	SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getJumpButtonIndex(), jumpButtonDown ? ILEDHW::ON : ILEDHW::OFF);
	SekvojModulePool::player_->stopLoop();

	//updateVelocity();

	newInstrument = 0;
	bool instrumentIsSelected = instrumentButtons_->getSelectedButton(newInstrument);

	if (instrumentIsSelected) {
		if (currentInstrumentIndex_ != newInstrument) {
			if (!SekvojModulePool::player_->isPlaying()) {
				SekvojModulePool::player_->playNote(newInstrument, DrumStep::NORMAL);
			}
			currentInstrumentIndex_ = newInstrument;
			currentPanIndex_ = 0;
			updateConfiguration();
			return;
		}
	} else if (instrumentWasSelected) {
		if (!SekvojModulePool::player_->isPlaying()) {
			SekvojModulePool::player_->playNote(newInstrument, DrumStep::NORMAL);
		}
	}

	unsigned char currentButtonDown = 0;
	if (drumStepView_->getDownButton(currentButtonDown) != inSubStepMode_) {
		if (!inSubStepMode_) {
			inSubStepMode_ = true;
			drumStepView_->setIgnoreOffs(false);
			DrumStep step = SekvojModulePool::memory_->getDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown);
			bool anyOn = false;
			for (unsigned char i = 0; i < 4; i++) {
				bool substepHasNote = step.getSubStep(i) != DrumStep::OFF;
				anyOn = anyOn || substepHasNote;
				SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(i), substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				subStepSwitches_.setStatus(i, substepHasNote);
			}
			if (!anyOn) {
				for (unsigned char subStep = 0; subStep < 4; subStep++) {
					if (subStep == 0 || SekvojModulePool::settings_->getDrumInstrumentEventType(currentInstrumentIndex_) == PlayerSettings::GATE) {
						step.setSubStep(subStep, currentVelocity_);
						SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(subStep), ILEDHW::ON);
						subStepSwitches_.setStatus(subStep, true);
					}
				}
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown, step);
			}
		} else {
			inSubStepMode_ = false;
			for (unsigned char i = 0; i < 4; i++) {
				SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(i), i == currentPanIndex_ ? ILEDHW::ON : ILEDHW::OFF);
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
				drumStepView_->setIgnoreOffs(true);
				step.setSubStep(i, substepHasNote ? DrumStep::OFF : currentVelocity_);
				SekvojModulePool::setLED(SekvojModulePool::buttonMap_->getSubStepButtonIndex(i), !substepHasNote ? ILEDHW::ON : ILEDHW::OFF);
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, (currentPanIndex_ * 16) + currentButtonDown, step);
				substepHasNote = !substepHasNote;
			}
			anyOn = anyOn || substepHasNote;
		}
		// Well there are no any sub steps set so step off shall not be ignored
		// because there is nothing to play
		if (!anyOn) {
			drumStepView_->setIgnoreOffs(false);
		}
	} else {
		panButtons_->update();
	}


	unsigned char newPan = 0;
	if (panButtons_->getSelectedButton(newPan)) {
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

	unsigned int newOffs = drumStepView_->getNewOffs();
	unsigned int newOns = drumStepView_->getNewOns();
	if (newOffs != 0 || newOns != 0) {
		for (unsigned char i = 0; i < 16; i++) {
			bool changeToOn = GETBIT(newOns, i);
			bool changeToOff = GETBIT(newOffs, i);
			if (changeToOn || changeToOff) {
				unsigned char currentStepIndex = (currentPanIndex_ * 16) + i;
				DrumStep step = SekvojModulePool::memory_->getDrumStep(currentInstrumentIndex_, currentStepIndex);
				step.setMuted(changeToOff);
				SekvojModulePool::memory_->setDrumStep(currentInstrumentIndex_, currentStepIndex, step);
			}

		}
	}
	updateCursor();
}

