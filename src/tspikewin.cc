#include "tspikewin.h"

TSpikeWin::TSpikeWin(Network * pNetwork) : 

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
  
  spikeWaveView1_(), 
  spikeWaveView2_(), 
  spikeWaveView3_(), 
  spikeWaveView4_()    
    
{
  //
  // Top-level window.
  //
  
  set_title("Tetrode Spike Viewer");

  set_reallocate_redraws(true);
  spVectpList_.push_back(new GLSPVect); 
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

  mainHBox_.pack_start(spikeWaveVBox_); 
  mainHBox_.pack_start(clusterViewVBox_); 

  clusterViewVBox_.pack_start(clusterTable_); 
  spikeWaveVBox_.pack_start(spikeWaveTable_); 

  //
  // Show window.
  //

  show_all();

  Glib::signal_idle().connect( sigc::mem_fun(*this, &TSpikeWin::on_idle) );


}
TSpikeWin::~TSpikeWin()
{

}

bool TSpikeWin::on_idle()

{
  std::cout << "ON idle" << std::endl; 
  clusterView12_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView13_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView14_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView23_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView24_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView34_.get_window()->invalidate_rect(get_allocation(), true);

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


