#ifndef FAKETTDATA_H
#define FAKETTDATA_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>
#include  <somadspio/eventcodec.h>

#include "somanetcodec.h" 
#include "ttreader.h"

using namespace dspiolib::codec; 


class FakeTTData
{
 public:
  FakeTTData(std::string filename, int rate, 
	     FakeNetwork* pni); 
  pDataPacket_t  getSpikeDataPacket(); 
  bool appendToFakeNetwork(FakeNetwork* fn);
  std::vector<TSpike_t>  getManySpikes(int n); 
  void setTime(FakeNetwork * fn, uint64_t usec);
  bool appendToFakeNetworkWithRealTime(FakeNetwork* fn); 

 private:
  ttreader ttreader_; 
  int rate_; 
  Glib::Timer timer_; 
  int spikeNum_;
  double lastSpikeTime_; 
  double currentDataTime_; 
  FakeNetwork * pni_; 
  void eventTXCallback(const EventTXList_t & el); 
  std::vector<TSpikeChannelState> fakestate_; 
  Event_t createAcqResponse(int chan, AcqDataSource::PARAMETERS param);

};



#endif // FAKETTDATA_H
