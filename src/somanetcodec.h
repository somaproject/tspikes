#ifndef SOMANETCODEC_H
#define SOMANETCODEC_H

#include <vector>
#include <somanetwork/networkinterface.h>
#include <somanetwork/tspike.h>
#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include <somatime.h>
#include  <somadspio/dspcontrol.h>
#include  <somadspio/types.h>

using namespace somanetwork; 

class TSpikeChannelState 
{
 public:
  int gain; 
  int32_t threshold; 
  bool hpf; 
  uint16_t filtid; 
  std::string filtstr; 

  // other stuff
  int32_t rangeMin; 
  int32_t rangeMax; 

  TSpikeChannelState(); 
}; 

struct chanprop_t
{
  int chan; 
  std::string name; 
}; 


typedef std::vector<chanprop_t> chanproplist_t; 

inline eventsource_t dsrc_to_esrc(datasource_t x ) {
  return x + 8; 
}

class SomaNetworkCodec 
{
  // The generic abstraction around the network 
  // interface, to handle decoding and encoding events and 
  // data

 public: 
//   SomaNetworkCodec(pNetworkInterface_t pNetwork, int src, 
// 		   chanproplist_t channels); 
  
  SomaNetworkCodec(pNetworkInterface_t pNetwork, int src); 
  ~SomaNetworkCodec(); 

  // state change interface
  void setChannelState(int channel, const TSpikeChannelState &) ; 
  TSpikeChannelState getChannelState(int channel); 

  sigc::signal<void, int, TSpikeChannelState> &  signalSourceStateChange(); 
  
  // timing update signal
  sigc::signal<void, somatime_t> & signalTimeUpdate(); 
  
  // get data
  sigc::signal<void, const TSpike_t &> & signalNewTSpike(); 
  
  // query state
  void refreshStateCache(); 
  chanproplist_t getChans() { return chanprops_; } ; 

  // filter settings
  std::map<uint32_t, std::string> getAvailableFilters(); 

  // gains
  std::map<int, std::string> getAvailableGains(); 
  
 private:
  pNetworkInterface_t pNetwork_; 
  int dsrc_;  /// data source # 
  chanproplist_t chanprops_; 
  
  std::vector<TSpikeChannelState> channelStateCache_; 

  // private signal objects that we return
  sigc::signal<void, somatime_t> signalTimeUpdate_;
  sigc::signal<void, int, TSpikeChannelState> signalSourceStateChange_; 

  sigc::signal<void, const TSpike_t &> signalNewTSpike_; 

  // internal implementation

  // internal source state cache
  


  void parseEvent(const Event_t & ); 

  // callbacks
  bool eventRXCallback(Glib::IOCondition io_condition); 
  bool dataRXCallback(Glib::IOCondition io_condition); 

  // remember, we're just a fitler
  void processNewData(pDataPacket_t ); 
  void processNewEvents(pEventList_t ); 

  void querySourceState(int chan); // request a source state update
  
  somadspio::StateProxy * dspStateProxy_; 
  void sendEvent(const EventTXList_t &); 

  // callbacks
  void dspLinkStatus(bool); 
  void dspMode(int mode); 
  void dspGain(int chan, int gain); 
  void dspHPFen(int chan, bool hpfen); 
  void dspRange(int chan, somadspio::range_t); 

  void dspThold(int chan, int thold); 
  void dspFilterID(int chan, somadspio::filterid_t fid); 
  
}; 

typedef boost::shared_ptr<SomaNetworkCodec> pSomaNetworkCodec_t; 


#endif // SOMANETCODEC_H
