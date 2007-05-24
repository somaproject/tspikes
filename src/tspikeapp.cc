
TSpikeWin::TSpikeWin()
  : m_VBox(false, 0), 
    table_(2, 3), 
    clusterView12_(&spvl, VIEW12), 
    clusterView13_(&spvl, VIEW13), 
    clusterView14_(&spvl, VIEW14), 
    clusterView23_(&spvl, VIEW23), 
    clusterView24_(&spvl, VIEW24), 
    clusterView34_(&spvl, VIEW34), 
{
  //
  // Top-level window.
  //
  
  set_title("Tetrode Spike Viewer");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);

  add(m_VBox);

  //
  // TSpikeApp scene.
  //

  clusterView12_.set_size_request(200, 200);
  clusterView13_.set_size_request(200, 200);
  clusterView14_.set_size_request(200, 200);
  clusterView23_.set_size_request(200, 200);
  clusterView24_.set_size_request(200, 200);
  clusterView34_.set_size_request(200, 200);

  
  clusterTable_.attach(clusterView12_, 0, 1, 0, 1);
  clusterTable_.attach(clusterView13_, 1, 2, 0, 1);
  clusterTaable_.attach(clusterView14_, 2, 3, 0, 1);
  clusterTable_.attach(clusterView23_, 0, 1, 1, 2);
  clusterTable_.attach(clusterView24_, 1, 2, 1, 2);
  clusterTable_.attach(clusterView34_, 2, 3, 1, 2);

  m_VBox.pack_start(table_); 

  m_VBox.pack_start(spikePosScale_); 

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &TSpikeApp::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  

  //
  // Show window.
  //

  show_all();

  Glib::signal_idle().connect( sigc::mem_fun(*this, &TSpikeApp::on_idle) );
 timer_.start(); 
 dtimer_.start(); 
}
TSpikeApp::~TSpikeApp()
{
}

void TSpikeApp::on_button_quit_clicked()
{

}
void TSpikeApp::updateSpikePosFromAdj()
{
  std::cout << "updateSpikePosFromAdj: " << spikePosAdjustment_.get_value() << std::endl; 
  GLSPVectpList::iterator svpliter = spvl.begin(); 
  
  for (int i = 0; i < spikePosAdjustment_.get_value(); i++)
    {
      svpliter++; 
    }

//   GLSPVectpList::iterator svpliter2 = svpliter; 

//   clusterView12_.setHistView(spvl.begin(), svpliter, 
// 			     0.01, LOG); 
//   svpliter = svpliter2; 

//   clusterView13_.setHistView(spvl.begin(), svpliter, 
// 			   0.01, LOG); 
//   svpliter = svpliter2; 

//   clusterView14_.setHistView(spvl.begin(), svpliter, 
// 			   0.01, LOG); 
//   svpliter = svpliter2; 


//   clusterView23_.setHistView(spvl.begin(), svpliter, 
// 			   0.01, LOG); 
//   svpliter = svpliter2; 

//   clusterView24_.setHistView(spvl.begin(), svpliter, 
// 			   0.01, LOG); 
//   svpliter = svpliter2; 

//   clusterView34_.setHistView(spvl.begin(), svpliter, 
// 			   0.01, LOG); 
//   svpliter = svpliter2; 


}

bool TSpikeApp::on_key_press_event(GdkEventKey* event)
{
  std::cout << "Keypress" << std::endl; 
  switch (event->keyval)
    {
    case GDK_z:
      break;
    case GDK_Z:
      break;
    case GDK_Up:
      std::cout << "Changing zoom" << std::endl; 
      clusterView12_.setViewingWindow(0, 3, 0, 3); 
      break;
    case GDK_Down: 
      std::cout << "Changing zoom down" << std::endl; 
      clusterView12_.setViewingWindow(0, 1, 0, 1); 

      break; 
    default:
      break;
    }
  return true; 

}


bool TSpikeApp::on_idle()

{

  clusterView12_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView13_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView14_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView23_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView24_.get_window()->invalidate_rect(get_allocation(), true);
  clusterView34_.get_window()->invalidate_rect(get_allocation(), true);

  double seconds = timer_.elapsed();
  
  if (seconds >= 1.0)
    {
      std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(3);
      int m_Frames = clusterView12_.getFrames(); 
      std::cout << m_Frames << " frames in "
                << seconds << " seconds = "
                << (m_Frames / seconds) << " FPS\n";
      timer_.reset();

    }

  double dseconds = dtimer_.elapsed();
  if (dseconds >= 0.1)
    {

      if (spvlp != spvlsrc.end())
	{
	  spvl.push_back(*spvlp); 
	  spvlp++; 
	} else {
	  spvl.push_back(new GLSPVect); 
	}
      
      dtimer_.reset(); 
    }

  
  return true;
}


