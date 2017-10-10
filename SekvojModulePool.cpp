/*
 * SekvojModulePool.cpp
 *
 *  Created on: Jan 11, 2015
 *      Author: martin
 */

#include "SekvojModulePool.h"

Player * SekvojModulePool::player_ = 0;
StepRecorder SekvojModulePool::recorder_;
IStepMemory * SekvojModulePool::memory_ = 0;
PlayerSettings * SekvojModulePool::settings_ = 0;
InstrumentBar * SekvojModulePool::instrumentBar_ = 0;
IButtonMap * SekvojModulePool::buttonMap_ = 0;
SekvojRackSDPreset SekvojModulePool::sd_;
StepSynchronizer SekvojModulePool::synchronizer_;
ITapper * SekvojModulePool::tapper_ = 0;

