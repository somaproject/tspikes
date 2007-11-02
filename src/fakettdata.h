#ifndef FAKETTDATA_H
#define FAKETTDATA_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>
#include "somanetcodec.h" 
#include "ttreader.h"


class FakeTTData
{
 public:
  FakeTTData(std::string filename, int rate, 
	     FakeNetwork* pni); 
  DataPacket_t *  getSpikeDataPacket(); 
  bool appendToFakeNetwork(FakeNetwork* fn);
  std::vector<TSpike_t>  getManySpikes(int n); 
  void setTime(FakeNetwork * fn, unsigned long usec);

 private:
  ttreader ttreader_; 
  int rate_; 
  Glib::Timer timer_; 
  int spikeNum_;
  FakeNetwork * pni_; 
  void eventTXCallback(const EventTXList_t & el); 
  std::vector<TSpikeChannelState> fakestate_; 
  Event_t createStateResponse(int chan, STATEPARM param);
};



#endif // FAKETTDATA_H
