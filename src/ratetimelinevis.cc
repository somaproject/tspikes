/* Conversion to gtkglextmm by Naofumi Yasufuku */

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>

#define GL_GLEXT_PROTOTYPES


#include <gtkglmm.h>


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "glspikes.h"
#include "ratetimeline.h"


void viewParamUpdate(bool isLive, float activePos, float decayRate)
{
  if (isLive) {
    std::cout << "live"; 
  }
  else 
    {
      std::cout << "not live"; 
    }
  std::cout << " activePos=" << activePos << " decayRate=" << decayRate
	    << std::endl;
 
}

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
  Glib::Timer timer_; 
  Glib::Timer dtimer_; 
  Gtk::HBox controlHBox_; 
  Gtk::Button buttonQuit_;
  Gtk::CheckButton liveCheckButton_; 
  RateTimeline rateTimeline_; 
  void updateSpikePosFromAdj(); 
  sigc::connection m_ConnectionIdle;
  virtual bool on_key_press_event(GdkEventKey* event); 
  void on_live_clicked(); 
  reltime_t curtime_; 
};

Vis::Vis(bool is_sync)
  : m_VBox(false, 0), 
    buttonQuit_("Quit"), 
    liveCheckButton_("Live"),
    rateTimeline_(),
    curtime_(0.0)
{

  set_title("Vis");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);

  add(m_VBox);

  //
  // Vis scene.
  //
  rateTimeline_.set_size_request(600, 50);

//   for (int i = 0; i < 10000; i++)
//     {
//       rateTimeline_.appendRate(float(i)/1000.0); 
//     }
  

  //
  // Simple quit button.
  //

  buttonQuit_.signal_clicked().connect(
    sigc::mem_fun(*this, &Vis::on_button_quit_clicked));
  
  m_VBox.pack_start(rateTimeline_); 
  m_VBox.pack_start(controlHBox_); 
  controlHBox_.pack_start(buttonQuit_, Gtk::PACK_SHRINK, 0);
  controlHBox_.pack_start(liveCheckButton_); 
  liveCheckButton_.set_active(true); 
  show_all();

  Glib::signal_idle().connect( sigc::mem_fun(*this, &Vis::on_idle) );
  rateTimeline_.viewSignal().connect(&viewParamUpdate); 

  liveCheckButton_.signal_clicked().connect(sigc::mem_fun(*this,
							  &Vis::on_live_clicked)); 

  timer_.start(); 
  dtimer_.start(); 
}

Vis::~Vis()
{
}

void Vis::on_button_quit_clicked()
{
  // we should quit
  hide(); 
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

  double seconds = timer_.elapsed();
  
  if (seconds >= 0.1)
    {
      RatePoint_t rp; 
      rp.time = curtime_; 
      rp.rate = 50 * sin(curtime_ / 5.0) + 60.0; 
      rateTimeline_.appendRate(rp); 
      timer_.reset();
      curtime_ += 1.0; 
    }

   double dseconds = dtimer_.elapsed();
   if (dseconds >= 0.01)
     {
       dtimer_.reset(); 
     }
  
  return true;
}

void Vis::on_live_clicked()
{

  rateTimeline_.setLive(liveCheckButton_.get_active()); 
  
}

void spikeWaves(void)
{

  

} 

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  //
  // Init gtkglextmm.
  //

  Gtk::GL::init(argc, argv);

  
  Vis vis(true);

  kit.run(vis);

  return 0;
}
