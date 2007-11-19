#include "ratetimeline.h"
#include "glconfig.h"
#include "shaders.h"

RateTimeline::RateTimeline() 

{

  Glib::RefPtr<Gdk::GL::Config> glconfig;
  
  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA   |
                                     Gdk::GL::MODE_DOUBLE | 
				     Gdk::GL::MODE_ACCUM);
  if (!glconfig)
    {
      throw std::runtime_error("Could not acquire double-buffered visual"); 
    }

  // print frame buffer attributes.
  GLConfigUtil::examine_gl_attrib(glconfig);

  set_gl_capability(glconfig);

  // Add events.

  // Add events.
  add_events(Gdk::BUTTON1_MOTION_MASK    |
	     Gdk::BUTTON2_MOTION_MASK    |
	     Gdk::BUTTON3_MOTION_MASK | 
	     Gdk::POINTER_MOTION_MASK | 
	     Gdk::SCROLL_MASK |
	     Gdk::BUTTON_PRESS_MASK      |
	     Gdk::VISIBILITY_NOTIFY_MASK | 
	     Gdk::ENTER_NOTIFY_MASK | 
	     Gdk::LEAVE_NOTIFY_MASK );
  
  // View transformation signals.
  signal_button_press_event().connect(sigc::mem_fun(*this, 
						    &RateTimeline::on_button_press_event)); 
  
  signal_motion_notify_event().connect(sigc::mem_fun(*this, 
						     &RateTimeline::on_motion_notify_event)); 

  signal_scroll_event().connect(sigc::mem_fun(*this, 
					      &RateTimeline::on_scroll_event)); 
  signal_enter_notify_event().connect(sigc::mem_fun(*this,
						    &RateTimeline::on_enter_notify_event)); 
  
  signal_leave_notify_event().connect(sigc::mem_fun(*this,
						    &RateTimeline::on_leave_notify_event)); 
  

}

RateTimeline::~RateTimeline()
{
}


void RateTimeline::on_realize()
{

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;
  
  renderer_.setViewPort(get_width(), get_height()); 
  renderer_.setup(); 

  gldrawable->gl_end();
  // *** OpenGL END ***
  //std::cout << "RateTimeline::on_realize done " << std::endl; 
}


bool RateTimeline::on_configure_event(GdkEventConfigure* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  // FIXME : What to do here? 
  renderer_.setViewPort(get_width(), get_height()); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

  //invalidate(); 
  return true;
  //std::cout << "RateTimeline::on_configure done " << std::endl; 

}

void RateTimeline::appendRate(RatePoint_t rp)
{

  if (renderer_.appendRate(rp)) {
    invalidate(); 
  }
  
}


bool RateTimeline::on_expose_event(GdkEventExpose* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  renderer_.render(); 

  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  return true;
}

bool RateTimeline::on_map_event(GdkEventAny* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 


  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

  std::cout << "RateTimeline::on_map_event done" << std::endl;
  return true;
}

bool RateTimeline::on_unmap_event(GdkEventAny* event)
{

  return true;
}

bool RateTimeline::on_visibility_notify_event(GdkEventVisibility* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 


//   glDrawBuffer(GL_BACK); 

//   glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();

  return true;
}

bool RateTimeline::on_button_press_event(GdkEventButton* event)
{
  std::cout << "on_button_press_event" << std::endl; 
  float x = event->x;
  float y = event->y;

  if (event->button == 2)
    {
      float windowPos = float(x) / get_width() * (renderer_.getViewT2() - 
						  renderer_.getViewT1()); 

      float activePos = windowPos + renderer_.getViewT1(); 
      renderer_.setActivePos(activePos); 


      invalidate(); 
      
      
      update();
    }
  
  lastX_ = event->x; 
 
  return false;
}

bool RateTimeline::on_motion_notify_event(GdkEventMotion* event)
{

  float x = event->x;
  float y = event->y;

  if (event->state & GDK_BUTTON1_MASK) 
    {
      
      float viewT1 = renderer_.getViewT1(); 
      float viewT2 = renderer_.getViewT2(); 

      // horizontal scrolling
      if (! renderer_.getLive()) {
	float movePixDelta = float(x) - float(lastX_); 
	
      // how many GL units was this? 
	float windowDelta = movePixDelta / get_width() * (viewT2 - viewT1 ); 
	
	
	//if (! renderer_.ratesEmpty() ) {
	  // don't scroll past start
	  if ((viewT1 - windowDelta) <  renderer_.getStartTime()) {
	    windowDelta = viewT1 - renderer_.getStartTime(); 
	  }
	  
	  viewT1 -= windowDelta; 	  
	  //}
	
	viewT2 -= windowDelta;

	renderer_.setViewT(viewT1, viewT2); 

	invalidate(); 
	
	update();
      } 
    } 
  else if (event->state & GDK_BUTTON2_MASK)
    {
      float viewT1 = renderer_.getViewT1(); 
      float viewT2 = renderer_.getViewT2(); 

      if (! renderer_.getLive()) {
	float windowPos = float(x) / get_width() * (viewT2 - viewT1); 

	renderer_.setViewT(viewT1, viewT2); 
	renderer_.setActivePos(viewT1 + windowPos); 

	invalidate(); 
	
	update();
      }
    } 
  else 
    {

      float viewT1 = renderer_.getViewT1(); 
      float viewT2 = renderer_.getViewT2(); 


      float windowPos = float(x) / get_width() * (viewT2 - viewT1) + viewT1; 

      renderer_.setCursorTime(windowPos); 
      invalidate(); 
      
      update();

    }




  lastX_ = x; 

  
  // don't block
  return false;
}

bool RateTimeline::on_scroll_event(GdkEventScroll* event)
{

   float x = event->x;
   float y = event->y;
  
   if (event->state & GDK_BUTTON2_MASK){
    
     if (event->direction == 0) {
       renderer_.setDecayRate(renderer_.getDecayRate() * 1.1); 
     } else {
       renderer_.setDecayRate(renderer_.getDecayRate() * 0.8); 

     }


	  
    
     invalidate();
    
     update();

   } 
//   else 
//     {
      
//       float centerPos = (x / get_width()) * (viewT2_ - viewT1_)  +  viewT1_; 
      
//       float newzoom = 1.0; 
      
//       if (event->direction == 0) {
// 	// UP? zoom out
// 	//newzoom = zoomLevel_ *  1.41421356237;
	
//       } else {
// 	//newzoom = zoomLevel_ / 1.41421356237; 
	
//       }
      
//       //setZoom(newzoom, centerPos); 
      
//       invalidate(); 
      
//       update();
//     }

  return false; 
}

viewsignal_t &  RateTimeline::viewSignal()
{
  return renderer_.viewSignal(); 
  
}

void RateTimeline::setLive(bool v)
{
  renderer_.setLive(v); 

  invalidate(); 

}

void RateTimeline::setCursorTime(float time)
{
  renderer_.setCursorTime(time); 
  invalidate(); 
}

void RateTimeline::setCursorVisible(bool visible)				    
{
  renderer_.setCursorVisible(visible); 
  invalidate(); 

}

bool RateTimeline::on_enter_notify_event(GdkEventCrossing* event)
{
  setCursorVisible(true); 
  return true; 
}

bool RateTimeline::on_leave_notify_event(GdkEventCrossing* event)
{
  setCursorVisible(false); 
  return true; 
}

void RateTimeline::resetData()
{
  renderer_.resetData(); 
  invalidate(); 

}
