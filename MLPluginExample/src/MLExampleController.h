
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __EXAMPLECONTROLLER_H__
#define __EXAMPLECONTROLLER_H__

#include "AppConfig.h"

#include "MLTime.h"

#include "MLUIBinaryData.h"
#include "MLPluginController.h"

#include "MLUIBinaryData.h"

#include "MLResponder.h"
#include "MLReporter.h"

#include "pa_ringbuffer.h"
#include "OscReceivedElements.h"
#include "OscPacketListener.h"

#if ML_MAC
#include "MLOSCListener.h"
#include "MLNetServiceHub.h"
#include "UdpSocket.h"
#endif

#include "MLProcInputToSignals.h"
#include "MLInputProtocols.h"

using namespace osc;

#define kOSCPort 3123

class MLExampleController  : 
	public MLPluginController,
	public Timer
{
public:
    MLExampleController(MLPluginProcessor* const ownerProcessor);
    ~MLExampleController();
	void initialize();
	void timerCallback();
	void doInfrequentTasks();
	void setInputProtocol(int p);

	// from MLButton::Listener
	void buttonClicked (MLButton*);

	void adaptUIToPatch();
	
private:	
    // (MLExampleController copy constructor and operator= being generated..)
    MLExampleController (const MLExampleController&);
    const MLExampleController& operator= (const MLExampleController&);
	
	MLPluginProcessor* const mpProcessor;
};



#endif // __EXAMPLECONTROLLER_H__