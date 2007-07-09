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
  frameCount_(0)

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
  clusterRenderer_.setViewingWindow(x1, y1, x2, y2); 

}

void ClusterView::updateViewingWindow()
{

  clusterRenderer_.updateViewingWindow(); 

}

bool ClusterView::on_configure_event(GdkEventConfigure* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  clusterRenderer_.reset(); 
  glViewport(0, 0, get_width(), get_height()); 

  gldrawable->wait_gl(); 
  
  gldrawable->gl_end();


  return true;
}

bool ClusterView::on_expose_event(GdkEventExpose* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  clusterRenderer_.render(); 
  
  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();
  frameCount_++; 

  
  return true;
}

bool ClusterView::on_map_event(GdkEventAny* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  clusterRenderer_.reset(); 
  glViewport(0, 0, get_width(), get_height()); 

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

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  clusterRenderer_.reset(); 
  glViewport(0, 0, get_width(), get_height()); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();

  return true;
}

bool ClusterView::on_idle()
{

  // Invalidate the whole window.

  return true;
}



void ClusterView::setView(GLSPVectpList_t::iterator sstart, 
			  GLSPVectpList_t::iterator send, 
			  float decayRate, DecayMode dm)
{

  clusterRenderer_.setView(sstart, send, decayRate, dm); 

}
