#include "clusterview.h"
#include "shaders.h"


int ClusterView::getFrames()
{
  int f = frameCount_;
  frameCount_ = 0; 
  return f; 

}

ClusterView::ClusterView(GLSPVectpList_t * pspvl, CViewMode cvm)
  : 
  clusterRenderer_(pspvl, cvm), 
  frameCount_(0),
  viewMode_(cvm)

{
  //assert (!pspvl->empty()); 

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

  // print frame buffer attributes.
  GLConfigUtil::examine_gl_attrib(glconfig);

  //
  // Set OpenGL-capability to the widget.
  //

  set_gl_capability(glconfig);

  // Add events.
  add_events(Gdk::VISIBILITY_NOTIFY_MASK);


}

ClusterView::~ClusterView()
{
}

void ClusterView::on_realize()
{
  std::cout << "ClusterView::on_realize " << viewMode_  << std::endl; 
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
  std::cout << "ClusterView::on_configure_event " << viewMode_  << std::endl; 

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
  clusterRenderer_.render(); 
  
  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();
  frameCount_++; 

  
  return true;
}

bool ClusterView::on_map_event(GdkEventAny* event)
{

  std::cout << "ClusterView::on_map_event " << viewMode_ << std::endl; 

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
  std::cout << "ClusterView::on_visibility_notify_event" << viewMode_ << std::endl; 

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


void ClusterView::setView(GLSPVectpList_t::iterator sstart, 
			  GLSPVectpList_t::iterator send, 
			  float decayRate, DecayMode dm)
{

  clusterRenderer_.setView(sstart, send, decayRate, dm); 

}

void ClusterView::zoomX(float factor)
{
  x2_ = x2_ * factor; 
  setViewingWindow(x1_, y1_, x2_, y2_); 
}

void ClusterView::zoomY(float factor)
{
  y2_ = y2_ * factor; 
  setViewingWindow(x1_, y1_, x2_, y2_); 

}

void ClusterView::setGrid(float g)
{
  clusterRenderer_.setGrid(g); 

}
