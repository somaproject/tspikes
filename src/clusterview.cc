#include "clusterview.h"
#include "shaders.h"


int ClusterView::getFrames()
{
  int f = m_Frames; 
  m_Frames = 0; 
  return f; 

}

ClusterView::ClusterView(GLSPVectpList_t * pspvl, CViewMode cvm)
  : pspvl_(pspvl), 
    decayRate_(0.01), 
    decayMode_(LOG), 
    viewChanged_(false), 
    m_Frames(0),
    viewMode_( cvm), 
    viewX1_(0), viewX2_(10), viewY1_(0), viewY2_(10), 
    pCurSPVect_(pspvl->begin())

{
  //assert (!pspvl->empty()); 

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

  //glEnable(GL_NORMALIZE);

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

  viewX1_ = x1; 
  viewX2_ = x2; 
  viewY1_ = y1; 
  viewY2_ = y2; 
  viewChanged_ = true; 
}

void ClusterView::updateViewingWindow()
{

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 

  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 

  glViewport(0, 0, get_width(), get_height());
  
  viewChanged_ = true; 

}

bool ClusterView::on_configure_event(GdkEventConfigure* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 


  updateViewingWindow(); 
  
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


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  if (viewChanged_) {
    updateViewingWindow(); 
    updateView(); 
    gldrawable->wait_gl(); 
    viewChanged_ = false; 
  }

  assert(pCurSPVect_ != pspvl_->end()); 
  // if buffer is empty do nothing
  if (viewEndIter_ == pspvl_->end() and
      viewStartIter_ == pspvl_->end())
    { 
      glClear(GL_COLOR_BUFFER_BIT); 
    } 
  else
    {

      glDrawBuffer(GL_BACK); 
      
      // copy things into current buffer
      glAccum(GL_RETURN, 1.0); 
      if (viewEndIter_ == pspvl_->end() )
	{
	  // we are always viewing the latest data 
	  GLSPVectpList_t::iterator lastp = pspvl_->end(); 
	  lastp--; 
	  if (pCurSPVect_ != lastp) // these are iterators
	    {
	      glClear(GL_COLOR_BUFFER_BIT); 
	  
	      // this is new data, render the old and go
	      renderSpikeVector(*pCurSPVect_); 
	      
	      switch(decayMode_) {
	      case LINEAR:
		glAccum(GL_ADD, -decayRate_); 
		break; 
	      case LOG:
		glAccum(GL_MULT, 1-decayRate_); 
		break; 
	      default:
		std::cerr << " should never get here" << std::endl; 
		break; 
	      }
	      
	      glAccum(GL_ACCUM, 1.0); 
	      
	      glAccum(GL_RETURN, 1.0); 
	      
	      pCurSPVect_ = lastp; 
	    }
	  
	} 
      renderSpikeVector(*pCurSPVect_, true); 

    }
  
  
  
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


void ClusterView::renderSpikeVector(const GLSPVect_t & spvect, bool live)
{
  // take the spikes in the SPvect and render them on the current
  // buffer; we assume viewport and whatnot are already configured

  glColor3f(1.0, 1.0, 0.0); 
  glVertexPointer(4, GL_FLOAT, sizeof(GLSpikePoint_t),
		  &spvect[0] ); 
  std::vector<CRGBA_t> colors(spvect.size()); 
  useGPUProgram(gpuProg_); 
  for(unsigned int i = 0; i < spvect.size(); i++)
    {

      CRGBA_t c = {0, 0, 0, 0}; 
      int tchan = spvect[i].tchan; 
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
	  c.R = 255; 
	  c.G = 255; 
	  c.B = 255; 
	  c.A = 255; 

	  break;
	}
      colors[i] = c; 
    }

  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA_t), 
 		 &colors[0]); 

  GLint vp = glGetUniformLocation(gpuProg_, "axes"); 
  glUniform1i(vp, viewMode_); 

  glDrawArrays(GL_POINTS, 0, spvect.size()); 
  if(live and ! spvect.empty()) {
    glColor4ubv((GLubyte*)&colors.back()); 
    glPointSize(4.0); 
    glBegin(GL_POINTS); 
    glVertex4f(spvect.back().p1, 
	       spvect.back().p2, 
	       spvect.back().p3, 
	       spvect.back().p4); 
    glEnd(); 
    glPointSize(1.0); 

  }
}

void ClusterView::resetAccumBuffer(GLSPVectpList_t::iterator sstart, 
			      GLSPVectpList_t::iterator send)
{
  // first clear accumulation buffer
  glClearAccum(0.0, 0.0, 0.0, 0.0); 
  glClearColor(0.0, 0.0, 0.0, 0.0); 
  
  glClear(GL_COLOR_BUFFER_BIT |  GL_ACCUM_BUFFER_BIT); 
  
  GLSPVectpList_t::iterator i; 
  glReadBuffer(GL_BACK); 
  int pos = 0; 
  for (i = sstart; i != send; i++)
    {
      pos++; 
      renderSpikeVector(*i); 
      glAccum(GL_ACCUM, 1.0); 
      
      switch(decayMode_) {
      case LINEAR:
	glAccum(GL_ADD, -decayRate_); 
	break; 
      case LOG:
	glAccum(GL_MULT, 1-decayRate_); 
	break; 
      default:
	std::cerr << " should never get here" << std::endl; 
	break; 
      }
	  
      glClear(GL_COLOR_BUFFER_BIT); 
      pCurSPVect_ = i; 
    }
  
}

void ClusterView::setView(GLSPVectpList_t::iterator sstart, 
			  GLSPVectpList_t::iterator send, 
			  float decayRate, DecayMode dm)
{

  viewStartIter_ = sstart; 
  viewEndIter_ = send; 
  decayRate_ = decayRate; 
  decayMode_ = dm;
  
  viewChanged_ = true; 

}
void ClusterView::updateView()
{
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 

  if (viewEndIter_ == pspvl_->end() )
    {
      pCurSPVect_ = --pspvl_->end(); 
    } else {
      pCurSPVect_ = viewEndIter_; 
    }

  //gldrawable->wait_gdk(); 
  

}


