#include "clusterview.h"
#include "shaders.h"


int ClusterView::getFrames()
{
  int f = m_Frames; 
  m_Frames = 0; 
  return f; 

}

ClusterView::ClusterView(GLSPVectpList * pspvl, CViewMode cvm)
  : pspvl_(pspvl), 
    decayVal_(0.01), 
    decayMode_(LOG), 
    m_Frames(0),
    viewMode_( cvm), 
    pCurSPVect_(pspvl->front())

{
  assert (!pspvl->empty()); 

  // configure view pointers
  viewStartIter_ = pspvl_->begin(); 
  viewEndIter_ = pspvl_->end(); 

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
      raise 
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
  std::cout << "ClusterView::on_realize()" << std::endl; 

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  //glEnable(GL_NORMALIZE);
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  glEnableClientState(GL_COLOR_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  GLuint vshdr = loadGPUShader("test.vert", GL_VERTEX_SHADER); 
  GLuint fshdr = loadGPUShader("test.frag", GL_FRAGMENT_SHADER); 
  std::list<GLuint> shaders; 
  shaders.push_back(vshdr); 
  shaders.push_back(fshdr); 
  gpuProg_ = createGPUProgram(shaders); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

}

bool ClusterView::setViewingWindow(float x1, float y1, 
				   float x2, float y2)
{
  glLoadIdentity(); 

  glOrtho(x1, y1, x2, y2, -3, 3); 

  glViewport(0, 0, get_width(), get_height());
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
     return false;
  gldrawable->wait_gdk(); 

  resetAccumBuffer(viewStartIter_, viewEndIter_); 
  gldrawable->gl_end();
  
  return true; 

}

bool ClusterView::on_configure_event(GdkEventConfigure* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 
  glOrtho(0, 3, 0, 3, -3, 3);   
  glViewport(0, 0, get_width(), get_height());
  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 
  

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

  //invalidate(); 
  return true;
}

bool ClusterView::on_expose_event(GdkEventExpose* event)
{

  //std::cout << "Expose event"  << std::endl; 

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glDrawBuffer(GL_BACK); 
  
  // copy things into current buffer
  glAccum(GL_RETURN, 1.0); 
  if (viewEndIter_ == pspvl_->end() )
    {
      // we are always viewing the latest data 
      GLSPVectpList::iterator lastp = viewEndIter_; 
      lastp--; 
      if (*(lastp) != pCurSPVect_)
	{
	  glClear(GL_COLOR_BUFFER_BIT); 

	  // this is new data, render the old and go
	  renderSpikeVector(pCurSPVect_); 
	  
	  switch(decayMode_) {
	  case LINEAR:
	    glAccum(GL_ADD, -decayVal_); 
	    break; 
	  case LOG:
	    glAccum(GL_MULT, 1-decayVal_); 
	    break; 
	  default:
	    std::cerr << " should never get here" << std::endl; 
	    break; 
	  }
	  
	  glAccum(GL_ACCUM, 1.0); 
	  
	  glAccum(GL_RETURN, 1.0); 

	  pCurSPVect_ = *(lastp); 
	}
      
    } 
  
  renderSpikeVector(pCurSPVect_); 
  
  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  ++m_Frames;
  
  
  
  return true;
}

bool ClusterView::on_map_event(GdkEventAny* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 
  

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***


  return true;
}

bool ClusterView::on_unmap_event(GdkEventAny* event)
{
  std::cout << "ClusterView::on_unmap_event" << std::endl; 

  return true;
}

bool ClusterView::on_visibility_notify_event(GdkEventVisibility* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 
  

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


void ClusterView::renderSpikeVector(const GLSPVect * spvect)
{
  // take the spikes in the SPvect and render them on the current
  // buffer; we assume viewport and whatnot are already configured

  glColor3f(1.0, 1.0, 0.0); 
  glVertexPointer(4, GL_FLOAT, sizeof(GLSpikePoint),
		  &((*spvect)[0])); 
  std::vector<CRGBA> colors(spvect->size()); 
  for(unsigned int i = 0; i < spvect->size(); i++)
    {
      CRGBA c = {0, 0, 0, 0}; 
      int tchan = (*spvect)[i].tchan; 
      switch (tchan) 
	{
	case 0:
	  c.R = 255; 
	  c.G = 0;
	  c.B = 0; 
	  c.A = 0; 
	  break; 

	case 1:
	  c.R = 0; 
	  c.G = 255; 
	  c.B = 0; 
	  c.A = 0; 
	  break; 

	case 2:
	  c.R = 0; 
	  c.G = 0; 
	  c.B = 255; 
	  c.A = 0; 
	  break; 

	case 3:
	  c.R = 255; 
	  c.G = 255; 
	  c.B = 0; 
	  c.A = 0; 
	  break; 

	default:
	  break;
	}
      colors[i] = c; 
    }
  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
 		 &colors[0]); 

  GLint vp = glGetUniformLocation(gpuProg_, "axes"); 
  glUniform1i(vp, viewMode_); 

  glDrawArrays(GL_POINTS, 0, spvect->size()); 


}

void ClusterView::resetAccumBuffer(GLSPVectpList::iterator sstart, 
			      GLSPVectpList::iterator send)
{
  // first clear accumulation buffer
  glClearAccum(0.0, 0.0, 0.0, 0.0); 
  glClearColor(0.0, 0.0, 0.0, 0.0); 
  
  glClear(GL_COLOR_BUFFER_BIT |  GL_ACCUM_BUFFER_BIT); 
  
  GLSPVectpList::iterator i; 
  glReadBuffer(GL_BACK); 
  int pos = 0; 
  for (i = sstart; i != send; i++)
    {
      pos++; 
      renderSpikeVector(*i); 
      glAccum(GL_ACCUM, 1.0); 
      
      switch(decayMode_) {
      case LINEAR:
	glAccum(GL_ADD, -decayVal_); 
	break; 
      case LOG:
	glAccum(GL_MULT, 1-decayVal_); 
	break; 
      default:
	std::cerr << " should never get here" << std::endl; 
	break; 
      }
	  
      glClear(GL_COLOR_BUFFER_BIT); 
      pCurSPVect_ = *i; 
    }
  
}

void ClusterView::setView(GLSPVectpList::iterator sstart, 
			  GLSPVectpList::iterator send, 
			  float decayVal, DecayMode dm)
{

  viewStartIter_ = sstart; 
  viewEndIter_ = send; 
  decayVal_ = decayVal; 
  decayMode_ = dm;
  


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***

  gldrawable->gl_begin(get_gl_context()); 
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 

  if (viewEndIter_ == pspvl_->end() )
    {
      pCurSPVect_ = pspvl_->back(); 
    } else {
      pCurSPVect_ = *viewEndIter_; 
    }

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

}

void ClusterView::inv() {
  get_window()->invalidate_rect(get_allocation(), true);
}
