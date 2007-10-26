#include "somanetcodec.h"

SomaNetworkCodec::SomaNetworkCodec(Network * pNetwork) :
  pNetwork_(pNetwork)
{

}

sigc::signal<void, int, TSpikeChannelState> &  SomaNetworkCodec::signalSourceStateChange() {
  return signalSourceStateChange_; 

}

sigc::signal<void, somatime_t> & SomaNetworkCodec::signalTimeUpdate()
{
  return signalTimeUpdate_; 
}

sigc::signal<void, const TSpike_t &> & SomaNetworkCodec::signalNewTSpike()
{
  
  return signalNewTSpike_; 
  
}


void SomaNetworkCodec::processNewData(DataPacket_t * dp)
{
  if (dp->typ == TSPIKE ) {
    TSpike_t newTSpike = rawToTSpike(dp); 
    signalNewTSpike_.emit(newTSpike); 

  } else {
    // somehow we received a datapacket that wasn't for us
  }

  delete dp; 

}

void SomaNetworkCodec::processNewEvents(EventList_t * pEventList)
{
  EventList_t::iterator pe; 
  for (pe = pEventList->begin(); pe != pEventList->end(); pe++) {
   
      parseEvent(*pe); 
  }
  
  
  delete pEventList; 
  
}

void SomaNetworkCodec::parseEvent(const Event_t & evt)
{
  // if event is time event, extract out, update time
  // 

  // if event is a state update; we do the state-update-dance
  
  

}
