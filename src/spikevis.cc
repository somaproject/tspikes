/* Conversion to gtkglextmm by Naofumi Yasufuku */

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h> 
#define GL_GLEXT_PROTOTYPES


#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "glspikes.h"
#include "clusterview.h"

const int N = 400000;
const int M = 6000;  

GLfloat points[N][4]; 


GLSPVectpList_t spvl, spvlsrc; 
GLSPVectpList_t::iterator spvlp; 

const float GSCALE = 100.0 ; 

///////////////////////////////////////////////////////////////////////////////
//
// The application class.
//
///////////////////////////////////////////////////////////////////////////////

class Vis : public Gtk::Window
{
public:
  explicit Vis(bool is_sync = true);
  virtual ~Vis();

protected:
  // signal handlers:
  void on_button_quit_clicked();
  virtual bool on_idle();

protected:
  // member widgets:
  Gtk::VBox m_VBox;
  Gtk::Table table_; 
  ClusterView clusterView12_; 
  ClusterView clusterView13_; 
  ClusterView clusterView14_; 
  ClusterView clusterView23_; 
  ClusterView clusterView24_; 
  ClusterView clusterView34_; 
  Glib::Timer timer_; 
  Glib::Timer dtimer_; 
  Gtk::Button m_ButtonQuit;
  Gtk::Adjustment spikePosAdjustment_; 
  Gtk::HScale spikePosScale_; 
  void updateSpikePosFromAdj(); 
  sigc::connection m_ConnectionIdle;

  virtual bool on_key_press_event(GdkEventKey* event); 


};

Vis::Vis(bool is_sync)
  : m_VBox(false, 0), 
    table_(2, 3), 
    clusterView12_(&spvl, VIEW12), 
    clusterView13_(&spvl, VIEW13), 
    clusterView14_(&spvl, VIEW14), 
    clusterView23_(&spvl, VIEW23), 
    clusterView24_(&spvl, VIEW24), 
    clusterView34_(&spvl, VIEW34), 
    m_ButtonQuit("Quit"),
    spikePosAdjustment_(0.0, 0.0, spvl.size(), 10.0, 10.0, 0), 
    spikePosScale_(spikePosAdjustment_)
{
  //
  // Top-level window.
  //

  set_title("Vis");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);

  add(m_VBox);

  //
  // Vis scene.
  //

  clusterView12_.set_size_request(200, 200);
  clusterView13_.set_size_request(200, 200);
  clusterView14_.set_size_request(200, 200);
  clusterView23_.set_size_request(200, 200);
  clusterView24_.set_size_request(200, 200);
  clusterView34_.set_size_request(200, 200);

  
  table_.attach(clusterView12_, 0, 1, 0, 1);
  table_.attach(clusterView13_, 1, 2, 0, 1);
  table_.attach(clusterView14_, 2, 3, 0, 1);
  table_.attach(clusterView23_, 0, 1, 1, 2);
  table_.attach(clusterView24_, 1, 2, 1, 2);
  table_.attach(clusterView34_, 2, 3, 1, 2);

  m_VBox.pack_start(table_); 

  spikePosScale_.set_update_policy(Gtk::UPDATE_DELAYED);
  spikePosScale_.set_digits(1);
  spikePosScale_.set_value_pos(Gtk::POS_TOP);
  spikePosScale_.set_draw_value();
  spikePosScale_.set_size_request(200, 30);
  m_VBox.pack_start(spikePosScale_); 

      clusterView34_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView12_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView13_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView14_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView23_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView24_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &Vis::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  spikePosAdjustment_.signal_value_changed().connect(
					    sigc::mem_fun(*this, 
							  &Vis::updateSpikePosFromAdj ) ); 

  

  //
  // Show window.
  //

  show_all();

  Glib::signal_idle().connect( sigc::mem_fun(*this, &Vis::on_idle) );
  timer_.start(); 
  dtimer_.start(); 
}
Vis::~Vis()
{
}

void Vis::on_button_quit_clicked()
{

}
void Vis::updateSpikePosFromAdj()
{
  std::cout << "updateSpikePosFromAdj: " << spikePosAdjustment_.get_value() << std::endl; 
  GLSPVectpList_t::iterator svpliter = spvl.begin(); 
  
  for (int i = 0; i < spikePosAdjustment_.get_value(); i++)
    {
      svpliter++; 
    }

//   GLSPVectpList_t::iterator svpliter2 = svpliter; 

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

bool Vis::on_key_press_event(GdkEventKey* event)
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
      clusterView34_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView12_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView13_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView14_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView23_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 
      clusterView24_.setViewingWindow(0, 0, 10.0 * GSCALE, 10.0 * GSCALE); 

      break;
    case GDK_Down: 
      std::cout << "Changing zoom down" << std::endl; 

      break; 
    default:
      break;
    }
  return true; 

}


bool Vis::on_idle()

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
	  rtime_t rt = spvlp.key(); 
	  spvl.insert(rt, new GLSPVect_t); 
	  *(--spvl.end()) = *spvlp; 
	  spvlp++; 

	} else {
	  rtime_t rt = 1.0; 
	  spvl.insert(rt, new GLSPVect_t); 
	}
      
      dtimer_.reset(); 
    }

  
  return true;
}


void spikesquares(void)
{
  
  float scale = 0.007 * GSCALE; 
  const int MAXX = 30; 
  const int MAXY = 30; 
  
  const int SPACEX = 40; 
  const int SPACEY = 40; 
  
  for (int i = 0; i < 20; i++)
    { 
      for (int j = 0 ; j < 20; j++)
	{
	  rtime_t rt = i * 20 + j;
	  spvlsrc.insert(rt, new GLSPVect_t); 

	  for (int x = 0; x < MAXX; x++) 
	    { 
	      for (int y = 0; y < MAXY; y++)
		{
		  GLSpikePoint_t sp1; 


		  sp1.p1 = float( x + j * SPACEX) * scale; 
		  sp1.p2 = float( y + i * SPACEY) * scale; 
		  sp1.p3 = float( x + j * SPACEX) * scale/2.0; 
		  sp1.p4 = float( y + i * SPACEY) * scale/2.0; 
		  sp1.ts = 1000; 
		  sp1.tchan = j % 4; 
		  (--spvlsrc.end())->push_back(sp1); 
		}
	    }

	}
    }
}

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  //
  // Init gtkglextmm.
  //
  

  Gtk::GL::init(argc, argv);
  spikesquares(); 

  spvlp = spvlsrc.begin(); 

  int testmode = 0;
  rtime_t t = 0.0; 
      
  switch (testmode)
    {
    case 0:
      // empty pointers
      spvl.insert(t, new GLSPVect_t); 
      *(--spvl.end()) = *spvlp; 
      spvlp++; 
      break; 
    case 1:
      // partly populated
//       for (int i =0; i < 207; i++)
// 	{
// 	  spvl.push_back(*spvlp); 
// 	  spvlp++; 
// 	}
      break ; 
    default:
      break; 
    }
  
  Vis vis(true);

  kit.run(vis);

  return 0;
}
