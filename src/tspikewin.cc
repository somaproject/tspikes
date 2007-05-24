#include "tspikewin.h"
#include "glspikes.h"

TSpikeWin::TSpikeWin(NetworkInterface * pNetwork) : 

  pNetwork_(pNetwork), 
  
  spVectpList_(), 


  clusterTable_(2, 3), 
  spikeWaveTable_(2, 2), 
  spikeWaveVBox_(false, 0), 
  clusterViewVBox_(false, 0), 
  mainHBox_(false, 0), 

  clusterView12_(&spVectpList_, VIEW12), 
  clusterView13_(&spVectpList_, VIEW13), 
  clusterView14_(&spVectpList_, VIEW14), 
  clusterView23_(&spVectpList_, VIEW23), 
  clusterView24_(&spVectpList_, VIEW24), 
  clusterView34_(&spVectpList_, VIEW34), 
  
  spikeWaveViewX_(CHANX), 
  spikeWaveViewY_(CHANY), 
  spikeWaveViewA_(CHANA), 
  spikeWaveViewB_(CHANB)    
    
{
  //
  // Top-level window.
  //
  
  set_title("Tetrode Spike Viewer");

  set_reallocate_redraws(true);
  spVectpList_.push_back(new GLSPVect_t); 
  add(mainHBox_); 

  clusterView12_.set_size_request(150, 150);
  clusterView13_.set_size_request(150, 150);
  clusterView14_.set_size_request(150, 150);
  clusterView23_.set_size_request(150, 150);
  clusterView24_.set_size_request(150, 150);
  clusterView34_.set_size_request(150, 150);

  
  clusterTable_.attach(clusterView12_, 0, 1, 0, 1);
  clusterTable_.attach(clusterView13_, 1, 2, 0, 1);
  clusterTable_.attach(clusterView14_, 2, 3, 0, 1);
  clusterTable_.attach(clusterView23_, 0, 1, 1, 2);
  clusterTable_.attach(clusterView24_, 1, 2, 1, 2);
  clusterTable_.attach(clusterView34_, 2, 3, 1, 2);



  clusterViewVBox_.pack_start(clusterTable_) ;
  
  
  spikeWaveViewX_.set_size_request(150, 150); 
  spikeWaveViewY_.set_size_request(150, 150); 
  spikeWaveViewA_.set_size_request(150, 150); 
  spikeWaveViewB_.set_size_request(150, 150); 

  spikeWaveTable_.attach(spikeWaveViewX_, 0, 1, 0, 1); 
  spikeWaveTable_.attach(spikeWaveViewY_, 1, 2, 0, 1); 
  spikeWaveTable_.attach(spikeWaveViewA_, 0, 1, 1, 2); 
  spikeWaveTable_.attach(spikeWaveViewB_, 1, 2, 1, 2); 


  spikeWaveVBox_.pack_start(spikeWaveTable_); 
  
  mainHBox_.pack_start(spikeWaveVBox_); 
  mainHBox_.pack_start(clusterViewVBox_);   
  //
  // Show window.
  //

  show_all();

  Glib::signal_idle().connect( sigc::mem_fun(*this, &TSpikeWin::on_idle) );
  Glib::signal_io().connect(sigc::mem_fun(*this, &TSpikeWin::dataRXCallback), 
			    pNetwork_->getDataFifoPipe(), Glib::IO_IN); 
  
  
}

TSpikeWin::~TSpikeWin()
{

}

bool TSpikeWin::on_idle()

{
  clusterView12_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView13_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView14_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView23_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView24_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView34_.get_window()->invalidate_rect(get_allocation(), true);


  spikeWaveViewX_.get_window()->invalidate_rect(get_allocation(), true);
  spikeWaveViewY_.get_window()->invalidate_rect(get_allocation(), true);
  spikeWaveViewA_.get_window()->invalidate_rect(get_allocation(), true);
  spikeWaveViewB_.get_window()->invalidate_rect(get_allocation(), true);

//   double seconds = timer_.elapsed();
  
//   if (seconds >= 1.0)
//     {
//       std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
//       std::cout.setf(std::ios::fixed, std::ios::floatfield);
//       std::cout.precision(3);
//       int m_Frames = clusterView12_.getFrames(); 
//       std::cout << m_Frames << " frames in "
//                 << seconds << " seconds = "
//                 << (m_Frames / seconds) << " FPS\n";
//       timer_.reset();

//     }

//   double dseconds = dtimer_.elapsed();
//   if (dseconds >= 0.1)
//     {

//       if (spvlp != spvlsrc.end())
// 	{
// 	  spvl.push_back(*spvlp); 
// 	  spvlp++; 
// 	} else {
// 	  spvl.push_back(new GLSPVect); 
// 	}
      
//       dtimer_.reset(); 
//     }

  
  return true;
}


bool TSpikeWin::dataRXCallback(Glib::IOCondition io_condition)
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
      // is this a spike? 
      if (rdp->typ == TSPIKE)
	{
	  // convert to a real spike packet
	  TSpike_t ts = rawToTSpike(rdp); 
	  // now what do we do with it? 
	  appendTSpikeToSpikewaves(ts); 
	  appendTSpikeToSPL(ts); 
	}

    }
  return true; 
}


void TSpikeWin::appendTSpikeToSpikewaves(const TSpike_t & tspike)
{
  GLSpikeWave_t x, y, a, b; 
  x.ts = tspike.time; 
  y.ts = tspike.time; 
  a.ts = tspike.time; 
  b.ts = tspike.time; 
  
  
  for (int i = 0; i < TSPIKEWAVE_LEN; i++)
    {
      x.wave.push_back(tspike.x.wave[i]); 
      y.wave.push_back(tspike.y.wave[i]); 
      a.wave.push_back(tspike.a.wave[i]); 
      b.wave.push_back(tspike.b.wave[i]); 

    }

  spikeWaveViewX_.newSpikeWave(x); 
  spikeWaveViewY_.newSpikeWave(y); 
  spikeWaveViewA_.newSpikeWave(a); 
  spikeWaveViewB_.newSpikeWave(b); 

}

void TSpikeWin::appendTSpikeToSPL(const TSpike_t & tspike)
{
  GLSpikePoint_t sp; 
  sp.ts = tspike.time; 
  sp.p1 = -100e6; 
  sp.p2 = -100e6; 
  sp.p3 = -100e6; 
  sp.p4 = -100e6; 
  sp.tchan = - 1 ;
  
  for (int i = 0; i < TSPIKEWAVE_LEN; i++)
    {
      if (sp.p1 < tspike.x.wave[i])
	sp.p1 = tspike.x.wave[i]; 
      
      if (sp.tchan < 0 & tspike.x.wave[i] > tspike.x.threshold)
	{
	  sp.tchan = 0; 
	}

      if (sp.p2 < tspike.y.wave[i])
	sp.p2 = tspike.y.wave[i]; 
      
      if (sp.tchan < 0 & tspike.y.wave[i] > tspike.y.threshold)
	{
	  sp.tchan = 1; 
	}

      if (sp.p3 < tspike.a.wave[i])
	sp.p3 = tspike.a.wave[i]; 
      
      if (sp.tchan < 0 & tspike.a.wave[i] > tspike.a.threshold)
	{
	  sp.tchan = 2; 
	}

      if (sp.p4 < tspike.b.wave[i])
	sp.p4 = tspike.b.wave[i]; 
      
      if (sp.tchan < 0 & tspike.b.wave[i] > tspike.b.threshold)
	{
	  sp.tchan = 3; 
	}

    }
  spVectpList_.back()->push_back(sp); 

  
}
