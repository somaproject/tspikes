#include "somanetcodec.h"

SomaNetworkCodec::SomaNetworkCodec(NetworkInterface * pNetwork) :
  pNetwork_(pNetwork)
{

  Glib::signal_io().connect(sigc::mem_fun(*this, &SomaNetworkCodec::dataRXCallback), 
			    pNetwork_->getDataFifoPipe(), Glib::IO_IN); 
  Glib::signal_io().connect(sigc::mem_fun(*this, &SomaNetworkCodec::eventRXCallback), 
			    pNetwork_->getEventFifoPipe(), Glib::IO_IN); 
  

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
  
  // emit relevant signals


}

bool SomaNetworkCodec::dataRXCallback(Glib::IOCondition io_condition)
{
  
  if ((io_condition & Glib::IO_IN) == 0) {
    std::cerr << "Invalid fifo response" << std::endl;
    return false; 
  }
  else 
    {
      char x; 
      read(pNetwork_->getDataFifoPipe(), &x, 1); 
      DataPacket_t * rdp = pNetwork_->getNewData(); 

      processNewData(rdp); 

      delete rdp; 
    
    }
  return true; 
}


bool SomaNetworkCodec::eventRXCallback(Glib::IOCondition io_condition)
{
  
  if ((io_condition & Glib::IO_IN) == 0) {
    std::cerr << "Invalid fifo response" << std::endl;
    return false; 
  }
  else 
    {
      char x; 
      read(pNetwork_->getEventFifoPipe(), &x, 1); 
      EventList_t * pel = pNetwork_->getNewEvents(); 
      processNewEvents(pel); 
      delete pel; 
    }
  return true; 
}

