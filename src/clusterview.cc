#include "clusterview.h"
#include "shaderutil/shaders.h"


int ClusterView::getFrames()
{
  int f = frameCount_;
  frameCount_ = 0; 
  return f; 

}

ClusterView::ClusterView(SpikePointVectDatabase & spvdb, CViewMode cvm)
  : 
  clusterRenderer_(spvdb, cvm), 
  frameCount_(0),
  viewMode_(cvm), 
  rangeX_(400e-6), 
  rangeY_(400e-6)
{

  //
  // Configure OpenGL-capable visual.
  //

  Glib::RefPtr<Gdk::GL::Config> glconfig;
  
  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA   |
                                     Gdk::GL::MODE_DOUBLE | 
				     Gdk::GL::MODE_ACCUM);
  if (!glconfig)
    {
      throw std::logic_error("Unable to acquire double-buffered visual"); 
    }

  //
  // Set OpenGL-capability to the widget.
  //

  set_gl_capability(glconfig);

  // Add events.
  add_events(Gdk::VISIBILITY_NOTIFY_MASK | 
	     Gdk::BUTTON1_MOTION_MASK    | 
	     Gdk::BUTTON2_MOTION_MASK    | 
	     Gdk::BUTTON_PRESS_MASK );

  signal_motion_notify_event().connect(sigc::mem_fun(*this, 
						     &ClusterView::on_motion_notify_event)); 

  signal_button_press_event().connect(sigc::mem_fun(*this, 
						    &ClusterView::on_button_press_event)); 
  
  clusterRenderer_.setRange(rangeX_, rangeY_); 

}

ClusterView::~ClusterView()
{
}

void ClusterView::on_realize()
{
  //std::cout << "ClusterView::on_realize " << viewMode_  << std::endl; 
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  clusterRenderer_.setup(); 
  glViewport(0, 0, get_width(), get_height()); 
  gldrawable->gl_end();
  // *** OpenGL END ***

}

void ClusterView::invalidate()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  Gdk::Rectangle r(0, 0, get_allocation().get_width(),
		   get_allocation().get_height());
  win->invalidate_rect(r, false);
  
}

bool ClusterView::setViewingWindow(float x1, float y1, 
				   float x2, float y2)
{


  x1_ = x1; 
  x2_ = x2; 
  y1_ = y1; 
  y2_ = y2; 

  clusterRenderer_.setViewingWindow(x1_, y1_, x2_, y2_); 

}

void ClusterView::updateViewingWindow()
{

  clusterRenderer_.updateViewingWindow(); 

}

bool ClusterView::on_configure_event(GdkEventConfigure* event)
{
  //  std::cout << "ClusterView::on_configure_event " << viewMode_  << std::endl; 

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 


  glViewport(0, 0, get_width(), get_height()); 
  clusterRenderer_.reset(); 

  gldrawable->wait_gl(); 
  
  gldrawable->gl_end();


  return true;
}

bool ClusterView::on_expose_event(GdkEventExpose* event)
{
  //std::cout << "ClusterView::on_expose_event " << viewMode_ << std::endl; 

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context())) {

    //assert(0 == 1);
      return false;
    }

  if (rangeX_ == 0  or rangeY_ == 0) {
    //clusterRenderer_.renderDisabled(); 
  } else { 
    clusterRenderer_.render(); 
  }
  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();
  frameCount_++; 

  
  return true;
}

bool ClusterView::on_map_event(GdkEventAny* event)
{

  //  std::cout << "ClusterView::on_map_event " << viewMode_ << std::endl; 

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  glViewport(0, 0, get_width(), get_height()); 
  clusterRenderer_.reset(); 

  gldrawable->wait_gl(); 
  
  gldrawable->gl_end();

  return true;
}

bool ClusterView::on_unmap_event(GdkEventAny* event)
{
  return true;
}

bool ClusterView::on_visibility_notify_event(GdkEventVisibility* event)
{
  //  std::cout << "ClusterView::on_visibility_notify_event" << viewMode_ << std::endl; 

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 


  glViewport(0, 0, get_width(), get_height()); 
  clusterRenderer_.reset(); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();

  return true;
}


void ClusterView::setView(GLSPVectMap_t::const_iterator sstart, 
			  GLSPVectMap_t::const_iterator send, 
			  float decayRate, DecayMode dm)
{

  clusterRenderer_.setView(sstart, send, decayRate, dm); 

}

void ClusterView::zoomX(float factor)
{
 

  float newx2 = x2_ * factor; 

  xViewChangeRequestSignal_.emit(newx2, newx2/rangeX_); 

}

void ClusterView::zoomY(float factor)
{
  float newy2 = y2_ * factor; 

  yViewChangeRequestSignal_.emit(newy2, newy2/rangeY_); 

}

void ClusterView::setGrid(float g)
{
  clusterRenderer_.setGrid(g); 
  
}

bool ClusterView::on_motion_notify_event(GdkEventMotion* event)
{

  float x = event->x;
  float y = event->y;


  if (event->state & GDK_BUTTON1_MASK) 
    {
      float pixXdelta = lastX_ - x; 
      float zoomXfact = pixXdelta/float(get_width()); 



      float pixYdelta = -(lastY_ - y); 
      float zoomYfact = pixYdelta/float(get_width()); 

      zoomX(1.0 + zoomXfact); 
      zoomY(1.0 + zoomYfact); 
      invalidate(); 
    } 
  else if (event->state & GDK_BUTTON3_MASK)
    {

    }

  lastX_ = x; 
  lastY_ = y; 

  return true;
}

bool ClusterView::on_button_press_event(GdkEventButton* event)
{
  
  float x = event->x;
  float y = event->y;

  lastX_ = event->x; 
  lastY_ = event->y; 
 
  return false;
}

void ClusterView::setXView(float x)
{
  /* Actually change the view */ 


  x2_ = x; 

  if (x2_ > rangeX_)
    x2_ = rangeX_; 
  setViewingWindow(x1_, y1_, x2_, y2_); 

}

void ClusterView::setXViewFraction(float x)
{
  setXView(rangeX_ * x); 
}

void ClusterView::setYView(float y)
{
  /* actually change the view */ 

  y2_ = y; 

  if (y2_ > rangeY_)
    y2_ = rangeY_; 
  
  setViewingWindow(x1_, y1_, x2_, y2_); 

}

void ClusterView::setYViewFraction(float y)
{
  setYView(rangeY_ * y); 
}


void ClusterView::updateState(bool X, const TSpikeChannelState & state)
{
  // X is true when the update is for the X (vertical) axis
  float max = 0.0;
  max = state.rangeMax / 1e9; 
  if (X) {
    rangeX_ = max; 
  } else {
    rangeY_ = max; 
  }
  clusterRenderer_.setRange(rangeX_, rangeY_); 

  invalidate(); 
}

void ClusterView::resetData()
{
  clusterRenderer_.resetData(); 


}

void ClusterView::resetView()
{

  setViewingWindow(0, 0, rangeX_, rangeY_);


}
