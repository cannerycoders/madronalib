
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "AppConfig.h"
#include "MLExampleProcessor.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	MLPluginProcessor* pFilter = new MLExampleProcessor();
	
	if (!READ_PLUGIN_FROM_FILE)
	{
		// initialize filter with Example description
		pFilter->loadPluginDescription((const char*) MLExampleBinaryData::mlexample_xml);
	}
	else 
	{
		MLError() << "NOTE: loading Processor from disk file!\n";
		File xmlFile("~/Dev/madronalib/MLPluginExample/PluginData/BinarySrc/MLExample.xml");

		if (xmlFile.exists())
		{
			String xmlStr = xmlFile.loadFileAsString();
			pFilter->loadPluginDescription((const char*)xmlStr.toUTF8());
		}
		else
		{
			debug() << "couldn't read plugin description file!\n";
		}
	}
	
    return pFilter;	
}

// set the default preset.
void MLExampleProcessor::loadDefaultPreset()
{
	setDefaultParameters();
	//debug() << "Example: loading default preset\n";
	//const String d((const char*) MLExampleBinaryData::default_xml);
	//setStateFromText(d);
}
