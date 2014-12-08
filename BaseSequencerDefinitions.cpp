/*
 * BaseSequencerDefinitions.cpp
 *
 *  Created on: Nov 9, 2014
 *      Author: bastl
 */

#include "BaseSequencerDefinitions.h"

inline unsigned int BaseSequencerDefinitions::getTotalNumberOfDrumSteps() {
	 return getStepsPerPattern() * getNumberOfDrumInstruments() * getNumberOfPatterns();
 }

inline unsigned int BaseSequencerDefinitions::getTolalNumberOfDrumBytes() {
	 return (getTotalNumberOfDrumSteps() * getBitsPerStep()) / 8;
 }
