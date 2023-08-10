#include "DigitraxBuffersModule.h"
#include "WifiSerialDebug.h"
#include "ConfigLoader.h"

IoTT_DigitraxBuffers* DigitraxBuffersModule::_digitraxBuffer = NULL;
IoTT_DigitraxBuffers* digitraxBuffer = NULL; // for exsting C code.

void DigitraxBuffersModule::setup(txFct lnOut)
{ 
    _digitraxBuffer = new IoTT_DigitraxBuffers(lnOut); 
    _digitraxBuffer->loadFromFile("/buffers.dat"); 

    // TODO: restore these from settings file.
    _digitraxBuffer->enableBushbyWatch(false);
    _digitraxBuffer->enableLissyMod(false); 
    _digitraxBuffer->enableFCRefresh(true, 75);

    _digitraxBuffer->setLocoNetMode(false);
    digitraxBuffer = _digitraxBuffer;
    Log::println("Digitrax buffer setup", LogLevel::INFO);
}

void DigitraxBuffersModule::begin()
{
}

void DigitraxBuffersModule::loop()
{
    _digitraxBuffer->processLoop();
}

void DigitraxBuffersModule::processLocoNetMsg(lnReceiveBuffer* newData)
{
    _digitraxBuffer->processLocoNetMsg(newData);
}