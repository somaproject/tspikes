#include "singlecluster.h"
#include "testspikes.h"
#include <sys/time.h>
#include <time.h>

using namespace std; 
SingleCluster::SingleCluster()
  : mainbox_(), 
    testBuffer_(40000), 
    clusterBox12_(testBuffer_, 1, 2),
    clusterBox13_(testBuffer_, 1, 3),
    clusterBox14_(testBuffer_, 1, 4),
    clusterBox23_(testBuffer_, 2, 3),
    clusterBox24_(testBuffer_, 2, 4),
    clusterBox34_(testBuffer_, 3, 4),
    clustTable_(2, 3, true), 
    adjTime_(0.0, 0.0, 1.0), 
    scaleTime_(adjTime_)
{
  set_title("Test Spikes Cluster"); 
  //set_border_width(10); 
  add(mainbox_); 

  // the party of populating the testbuffer:
  vector<soma::Spike> spikes;
  loadSpikes(&spikes); 
  unsigned int timeMin(0), timeMax(0);
  timeMin--;  // should use correct TRAIT!!

  for (int i = 0; i < spikes.size(); i++)
    {
      SpikePoint newSP(spikes[i]);  
      testBuffer_.push_back(newSP);
    }
  
  SpikeBuffer::iterator sp; 

  for (sp  = testBuffer_.begin(); sp != testBuffer_.end(); sp++)
    {
       if (sp->getTime() > timeMax)
	timeMax = sp->getTime(); 
      if (sp->getTime() < timeMin)
	timeMin = sp->getTime(); 
    }
  cout << "Spikes done" << endl; 

  cout << "Time ranges between " << timeMax << " and " << timeMin << endl; 
  // range setup: 
  adjTime_.set_lower(timeMin); 
  adjTime_.set_upper(timeMax); 
  scaleTime_.set_digits(1); 
  
  adjTime_.signal_value_changed().connect(SigC::slot(*this, &SingleCluster::onTimeChange));
  // debugging print:
  // add cluster boxen:
  clustTable_.attach(clusterBox12_, 0, 1, 0, 1); 
  clustTable_.attach(clusterBox13_, 1, 2, 0, 1); 
  clustTable_.attach(clusterBox14_, 2, 3, 0, 1); 
  clustTable_.attach(clusterBox23_, 0, 1, 1, 2); 
  clustTable_.attach(clusterBox24_, 1, 2, 1, 2); 
  clustTable_.attach(clusterBox34_, 2, 3, 1, 2); 
  
  
  mainbox_.pack_start(clustTable_);
  mainbox_.pack_start(scaleTime_); 
  WinView wv(0, 5.048e-4, 0, 5.048e-4);
  WinView cv(0, 300e-6, 0, 300e-6); 
  clusterBox12_.setTotalWin(wv); 
  clusterBox13_.setTotalWin(wv); 
  clusterBox14_.setTotalWin(wv); 
  clusterBox23_.setTotalWin(wv); 
  clusterBox24_.setTotalWin(wv); 
  clusterBox34_.setTotalWin(wv); 


  clusterBox12_.setClustWin(cv); 
  clusterBox13_.setClustWin(cv); 
  clusterBox14_.setClustWin(cv); 
  clusterBox23_.setClustWin(cv); 
  clusterBox24_.setClustWin(cv); 
  clusterBox34_.setClustWin(cv); 

  show_all_children();
  
}

SingleCluster::~SingleCluster()
{
}

void SingleCluster::onTimeChange()
{
  timeval t1, t2; 
  gettimeofday(&t1, NULL); 
  unsigned int time = (unsigned int)adjTime_.get_value(); 
  clusterBox12_.setTime(time);
  clusterBox13_.setTime(time);
  clusterBox14_.setTime(time);
  clusterBox23_.setTime(time);
  clusterBox24_.setTime(time);
  clusterBox34_.setTime(time);
 
  gettimeofday(&t2, NULL); 
  long t1l(t1.tv_sec * 1000000 + t1.tv_usec); 
  long t2l(t2.tv_sec * 1000000 + t2.tv_usec); 

  cout << "delta is " << t2l - t1l << " us" << endl; 
}
