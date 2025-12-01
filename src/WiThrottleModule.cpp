#include "WiThrottleModule.h"
#include "WifiSerialDebug.h"
#include "ConfigLoader.h"

IoTT_LBServer* WiThrottleModule::_lbClient = NULL;
IoTT_LBServer* wiServer = NULL; // for exsting IOTT code. NOT being used at present...

//this is the outgoing communication function for IoTT_DigitraxBuffers.h, routing the outgoing messages to the correct interface
// only supporting WiThrottle for now although the underlying IOTT code does have support
// support for others.
uint16_t  WiThrottleModule::sendMsg(lnTransmitMsg txData)
{ 
    Log::println("Send msg", LogLevel::INFO);    
    WiThrottleModule::_lbClient->lnWriteMsg(&txData);   
    return 0;
}

void callbackLocoNetMessage(lnReceiveBuffer * newData) //this is the landing point for incoming LocoNet messages
                                                       //from LocoNet interfaces
{
    // needed by exsting IOTT code. NOT being used at present...
     Log::println("New Data msg", LogLevel::INFO);    
}

void WiThrottleModule::setup()
{
    Log::println("Load WiThrottle Client", LogLevel::INFO);
    DynamicJsonDocument * jsonDataObj = NULL;  
    jsonDataObj = ConfigLoader::getDocPtr("/configdata/wiclient.cfg");

    if (jsonDataObj != NULL)
    {
        _lbClient = new IoTT_LBServer();
        _lbClient->loadLBServerCfgJSON(*jsonDataObj);
        _lbClient->initWIServer(false);
        digitraxBuffer->setLocoNetMode(false);
        delete(jsonDataObj);
    }
    Log::println("WiThrottle setup", LogLevel::INFO);   
}

void WiThrottleModule::begin()
{
}

bool WiThrottleModule::loop()
{
    Log::println("WiThrot Loop", LogLevel::LOOP);
    _lbClient->processLoop();

    // TODO: can this be optimized.
    return false;
}