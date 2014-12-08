/*
 * BaseSequencerDefinitions.h
 *
 *  Created on: Nov 9, 2014
 *      Author: bastl
 */

#ifndef BASESEQUENCERDEFINITIONS_H_
#define BASESEQUENCERDEFINITIONS_H_

#include "ISequencerDefinitions.h"

class BaseSequencerDefinitions : public ISequencerDefinitions{
public:
	virtual unsigned int getTotalNumberOfDrumSteps();
	virtual unsigned int getTolalNumberOfDrumBytes();
};

#endif /* BASESEQUENCERDEFINITIONS_H_ */
