/*
    Settings.cpp - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.
*/

#include "Settings.h"

namespace Element {

const char* Settings::checkForUpdatesKey = "checkForUpdates";
const char* Settings::pluginListKey = "plugin-list";
const char* Settings::pluginListKey64 = "pluginList64";

#if JUCE_MAC || JUCE_32BIT
 const char* Settings::lastPluginScanPathPrefix = "pluginScanPath_";
#else
 const char* Settings::lastPluginScanPathPrefix = "pluginScanPath64_";
#endif
    
Settings::Settings()
{
    PropertiesFile::Options opts;
    opts.applicationName     = "Element";
    opts.filenameSuffix      = "conf";
    opts.osxLibrarySubFolder = "Application Support";
    opts.storageFormat       = PropertiesFile::storeAsCompressedBinary;

   #if JUCE_DEBUG
    opts.applicationName     = "ElementDebug";
    opts.storageFormat       = PropertiesFile::storeAsXML;
   #endif
    
   #if JUCE_LINUX
    opts.folderName          = ".config/Element";
   #else
    opts.folderName          = "Element";
   #endif

    setStorageParameters (opts);
}

Settings::~Settings() { }

bool Settings::checkForUpdates() const
{
    if (auto* props = getProps())
        return props->getBoolValue (checkForUpdatesKey);
    return false;
}
    
PropertiesFile* Settings::getProps() const {
    return (const_cast<Settings*> (this))->getUserSettings();
}

XmlElement* Settings::getLastGraph() const
{
    if (auto* p = getProps())
        return p->getXmlValue ("lastGraph");
    return nullptr;
}

void Settings::setLastGraph (const ValueTree& data)
{
    jassert (data.hasType (Tags::node));
    if (! data.hasType (Tags::node))
        return;
    if (auto* p = getProps())
        if (ScopedXml xml = data.createXml())
            p->setValue ("lastGraph", xml);
}
}
