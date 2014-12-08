/*
 * ISequencerDefinitions.h
 *
 *  Created on: Nov 9, 2014
 *      Author: bastl
 */

#ifndef ISEQUENCERDEFINITIONS_H_
#define ISEQUENCERDEFINITIONS_H_

class ISequencerDefinitions {
public:

	virtual unsigned char getStepsPerPattern() = 0;
	virtual unsigned char getNumberOfDrumInstruments() = 0;
	virtual unsigned char getNumberOfPatterns() = 0;
	virtual unsigned int getTotalNumberOfDrumSteps() = 0;
	virtual unsigned char getBitsPerStep() = 0;
	virtual unsigned int getTolalNumberOfDrumBytes() = 0;
	virtual unsigned char getNUmberOfBytesForInstrumentSwitches() = 0;

};

#endif /* ISEQUENCERDEFINITIONS_H_ */
