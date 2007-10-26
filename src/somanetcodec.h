#ifndef SOMANETCODEC_H
#define SOMANETCODEC_H

#include <vector>
#include <somanetwork/network.h>
#include <somanetwork/tspike.h>
#include <sigc++/sigc++.h>

typedef uint64_t somatime_t; 

struct TSpikeChannelState 
{
  int gain; 
  int32_t threshold; 
  // other stuff
}; 

class SomaNetworkCodec 
{
  // The generic abstraction around the network 
  // interface, to handle decoding and encoding events and 
  // data

 public: 
  SomaNetworkCodec(Network * pNetwork); 
  
  // remember, we're just a fitler
  void processNewData(DataPacket_t* ); 
  void processNewEvents(EventList_t * ); 

  // state change interface
  void setChannelState(int channel, TSpikeChannelState); 
  TSpikeChannelState getChannelState(int channel); 
  sigc::signal<void, int, TSpikeChannelState> &  signalSourceStateChange(); 
  
  // timing update signal
  sigc::signal<void, somatime_t> & signalTimeUpdate(); 
  
  // get data
  sigc::signal<void, const TSpike_t &> & signalNewTSpike(); 
  
 private:
  Network * pNetwork_; 
  
  // private signal objects that we return
  sigc::signal<void, somatime_t> signalTimeUpdate_;
  sigc::signal<void, int, TSpikeChannelState> signalSourceStateChange_; 

  sigc::signal<void, const TSpike_t &> signalNewTSpike_; 

  // internal implementation

  // internal source state cache
  std::vector<TSpikeChannelState> channelStateCache_; 
  
  void refreshStateCache(); 

  void parseEvent(const Event_t & ); 
  
}; 


#endif // SOMANETCODEC_H
