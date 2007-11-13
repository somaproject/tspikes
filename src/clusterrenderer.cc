#include "clusterrenderer.h"
#include "shaders.h"
#include "voltage.h"

ClusterRenderer::ClusterRenderer(GLSPVectpList_t * pspvl, CViewMode cvm)
  : pspvl_(pspvl), 
    decayRate_(0.09), 
    decayMode_(LOG), 
    viewChanged_(false), 
    viewMode_( cvm), 
    viewX1_(0), viewX2_(10), viewY1_(0), viewY2_(10), 
    pCurSPVect_(pspvl->begin()), 
    isSetup_(false),
    gridSpacing_(50e-6),
    Xlabel_(""), 
    Ylabel_(""), 
    glString_("Bitstream Vera Sans", true, CENTER), 
    glHScaleString_("Bitstream Vera Sans", false, CENTER), 
    glVScaleString_("Bitstream Vera Sans", false, LEFT), 
    rangeBoxVisible_(true), 
    rangeX_(1e-3), 
    rangeY_(1e-3), 
    textAlpha_(0.0)
  
{

  // configure view pointers
  viewStartIter_ = pspvl_->begin(); 
  viewEndIter_ = pspvl_->end(); 
  
  switch(viewMode_) {
  case VIEW12:
    Xlabel_ = "X"; 
    Ylabel_ = "Y"; 
    break; 

  case VIEW13:
    Xlabel_ = "X"; 
    Ylabel_ = "A"; 
    break; 

  case VIEW14:
    Xlabel_ = "X"; 
    Ylabel_ = "B"; 
    break; 

  case VIEW23:
    Xlabel_ = "Y"; 
    Ylabel_ = "A"; 
    break; 

  case VIEW24:
    Xlabel_ = "Y"; 
    Ylabel_ = "B"; 
    break; 

  case VIEW34:
    Xlabel_ = "A"; 
    Ylabel_ = "B"; 
    break; 

  default:
    Xlabel_ = "OTHERX";
    Ylabel_ = "OTHERY";
  }
  // populate axis labels
  axisLabels_.push_back(0.0); 
  axisLabels_.push_back(100e-6); 
  axisLabels_.push_back(200e-6); 
  axisLabels_.push_back(500e-6); 
  axisLabels_.push_back(1e-3); 
  axisLabels_.push_back(2e-3); 
  axisLabels_.push_back(5e-3); 
  axisLabels_.push_back(10e-3); 
  axisLabels_.push_back(20e-3); 
  axisLabels_.push_back(50e-3); 
  axisLabels_.push_back(100e-3); 

    
}

ClusterRenderer::~ClusterRenderer()
{
}

void ClusterRenderer::setup()
{
  // this setup function must be called prior to any actual render
  // event

  glEnableClientState(GL_VERTEX_ARRAY); 
  glEnableClientState(GL_COLOR_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glEnable (GL_BLEND); 
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

  GLuint vshdr = loadGPUShader("cluster.vert", GL_VERTEX_SHADER); 
  GLuint fshdr = loadGPUShader("cluster.frag", GL_FRAGMENT_SHADER); 
  std::list<GLuint> shaders; 
  shaders.push_back(vshdr); 
  shaders.push_back(fshdr); 
  gpuProg_ = createGPUProgram(shaders); 

  isSetup_ = true; 
}

bool ClusterRenderer::setViewingWindow(float x1, float y1, 
				   float x2, float y2)
{

  viewX1_ = x1; 
  viewX2_ = x2; 
  viewY1_ = y1; 
  viewY2_ = y2; 

  viewChanged_ = true; 
}

void ClusterRenderer::updateViewingWindow()
{
  assert (isSetup_ == true); 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 

  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 
  viewChanged_ = true; 

}

void ClusterRenderer::render() 
{

  // this is the primary render event
  assert (isSetup_ == true); 
  
  if (viewChanged_) {
    updateViewingWindow(); 
    updateView(); 
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
	      renderSpikeVector(pCurSPVect_); 
	      
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
      renderGrid();   
      renderSpikeVector(pCurSPVect_, true); 
      
    }
  
  glColor4f(1.0, 1.0, 1.0, 0.4); 
  
    // render text for axes
  glString_.drawWinText(160, 11, Xlabel_, 20, 0.5); 
  glString_.drawWinText(4, 150, Ylabel_, 20, 0.5); 

  if (rangeBoxVisible_) {
    drawRangeBox(); 
  }

  
  GLenum g = glGetError(); 
  while (g != GL_NO_ERROR) {
    std::cerr << "There was a GL error " << g << " in " << 
      viewMode_ << std::endl; 
    g = glGetError(); 
  }
}

void ClusterRenderer::renderSpikeVector(GLSPVectpList_t::iterator i, bool live)
{
  // This is just syntactic sugar to make it easy for us to render
  // an iterator as well
  renderSpikeVector(*i->second, live); 
  
}

void ClusterRenderer::renderSpikeVector(const GLSPVect_t & spvect, bool live)
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
  if(live  &&  !spvect.empty()) {
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
  useGPUProgram(0); 
    
}

void ClusterRenderer::resetAccumBuffer(GLSPVectpList_t::iterator sstart, 
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
      renderSpikeVector(i); 
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

void ClusterRenderer::setView(GLSPVectpList_t::iterator sstart, 
			  GLSPVectpList_t::iterator send, 
			  float decayRate, DecayMode dm)
{

  viewStartIter_ = sstart; 
  viewEndIter_ = send; 
  decayRate_ = decayRate; 
  decayMode_ = dm;
  
  viewChanged_ = true; 

}
void ClusterRenderer::updateView()
{
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 

  if (viewEndIter_ == pspvl_->end() )
    {
      pCurSPVect_ = getLastIter(*pspvl_); 
    } else {
      pCurSPVect_ = viewEndIter_; 
    }

}


void ClusterRenderer::reset()
{

  glDrawBuffer(GL_BACK); 

  glClearColor(0.0, 0.0, 0.0, 1.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  resetAccumBuffer(viewStartIter_, viewEndIter_); 

}

void ClusterRenderer::renderGrid()
{

  renderHGrid(); 
  renderVGrid(); 


}

void ClusterRenderer::setGrid(float g)
{
  gridSpacing_ = g; 

}

void ClusterRenderer::renderHGrid()
{
  // render the horizontal grid; we need a "text schedule" that
  // will fade out the relevant bits. 
  // in some sense this is just going to have to be "policy" that
  // we hard-code and live with. 
  
  glColor4f(1.0, 1.0, 1.0, 0.2); 

  int N = int(round((viewX2_ - 0.0)/gridSpacing_)); 

  // scale 1
  if (N < 15) {
    glColor4f(1.0, 1.0, 1.0, 0.2); 
  } else {
    glColor4f(1.0, 1.0, 1.0, 0.2 * 15.0/N); 
  }

  if (N < 30) {
    
    for (int i = 1; (i*gridSpacing_) < viewX2_; i++) {
      
      float x = float(i) * gridSpacing_; 
	
      glBegin(GL_LINES); 
      glVertex2f(x, viewY1_); 
      glVertex2f(x, viewY2_); 
      glEnd(); 
    }
  } 
  
  // scale 2
  glColor4f(1.0, 1.0, 1.0, 0.8); 


  for (int i = 0; (i*gridSpacing_*10.0) < viewX2_; i++)
    {
      glBegin(GL_LINES); 
      float x = float(i) * gridSpacing_*10.0; 
      glVertex2f(x, viewY1_); 
      glVertex2f(x, viewY2_); 
      glEnd(); 
    }


  float range = viewX2_ - viewX1_; // which is basically zero

  // always render the three most recent ones; 
  std::vector<float>::iterator r = std::lower_bound(axisLabels_.begin(), 
						    axisLabels_.end(), 
						    range ); 
  
  int num = 0; 
  float decayalpha = 1.0; 
  for ( std::vector<float>::iterator i = r; 
	r != axisLabels_.begin() and num < 4; r--) 
    {
      
      float x1 = round( *r /  gridSpacing_) * gridSpacing_;  
      Voltage v1(x1); 

      if (num > 1) {
	decayalpha = x1/range *2;
      }

      glHScaleString_.drawWorldText(x1, viewY1_ + (viewY2_ - viewY1_)*0.05, 
				    v1.str(0), 8, decayalpha); 
      
      num += 1; 
    }
  

}

void ClusterRenderer::renderVGrid()
{

  glColor4f(1.0, 1.0, 1.0, 0.2); 

  int N = int(round((viewY2_ - 0.0)/gridSpacing_)); 

  // scale 1
  if (N < 15) {
    glColor4f(1.0, 1.0, 1.0, 0.2); 
  } else {
    glColor4f(1.0, 1.0, 1.0, 0.2 * 15.0/N); 
  }

  if (N < 30) {

    for (int i = 1; (i*gridSpacing_) < viewY2_; i++) {
	
	float y = float(i) * gridSpacing_; 
	
	
	glBegin(GL_LINES); 
	glVertex2f(viewX1_, y); 
	glVertex2f(viewX2_, y); 
	glEnd(); 
	
    }
    
    
  } 
  
  // scale 2
  glColor4f(0.8, 0.8, 1.0, 0.7); 

  glBegin(GL_LINES); 
  for (int i = 0; (i*gridSpacing_*10.0) < viewY2_; i++)
    {
      float y = float(i) * gridSpacing_*10.0; 
      glVertex2f(viewX1_, y); 
      glVertex2f(viewX2_, y); 
    }
  glEnd(); 
  

  float range = viewY2_ - viewY1_; // which is basically zero

  // always render the three most recent ones; 
  std::vector<float>::iterator r = std::lower_bound(axisLabels_.begin(), 
						    axisLabels_.end(), 
						    range); 
  
  int num = 0; 
  float decayalpha = 1.0; 
  for ( std::vector<float>::iterator i = r; 
	r != axisLabels_.begin() and num < 4; r--) 
    {
      
      float y1 = round( *r /  gridSpacing_) * gridSpacing_;  
      Voltage v1(y1); 

      if (num > 1) {
	decayalpha = y1/range *2;
      }

      glVScaleString_.drawWorldText(viewX1_,  y1, 
				    v1.str(0), 8, decayalpha); 
      
      num += 1; 
    }
  

}

void ClusterRenderer::setRangeBoxVisible(bool vis)
{
  rangeBoxVisible_ = vis; 

}

void ClusterRenderer::drawRangeBox()
{
  // draw the box that shows the max and min range and our
  // current position
  
  glColor4f(0.0, 0.0, 0.0, 1.0); 

  float frac = 0.25; 
  float xwidth = viewX2_ - viewX1_; 
  float ywidth = viewY2_ - viewY1_; 


  glBegin(GL_LINE_LOOP); 
  glVertex2f(viewX2_ -  xwidth*frac, viewY2_); 
  glVertex2f(viewX2_, viewY2_); 
  glVertex2f(viewX2_, viewY2_ - ywidth*frac); 
  glVertex2f(viewX2_ - xwidth*frac, viewY2_ - ywidth*frac); 
  glEnd(); 
  
  // draw the border
  glColor4f(1.0, 1.0, 1.0, 1.0); 
  glBegin(GL_LINE_LOOP); 
  glVertex2f(viewX2_ -  xwidth*frac, viewY2_); 
  glVertex2f(viewX2_, viewY2_); 
  glVertex2f(viewX2_, viewY2_ - ywidth*frac); 
  glVertex2f(viewX2_ - xwidth*frac, viewY2_ - ywidth*frac); 
  glEnd(); 

  // now, draw the active region
  glColor4f(1.0, 0.0, 0.0, 1.0); 
  glBegin(GL_LINE_LOOP); 
  // lower-left coordinate
  glVertex2f(viewX2_ - xwidth*frac, viewY2_ - ywidth*frac); 

  glVertex2f(viewX2_ - xwidth*frac, 
	     viewY2_ - ywidth*frac + viewY2_/rangeY_*frac*ywidth); 

  glVertex2f(viewX2_ - xwidth*frac + viewX2_/rangeX_*xwidth*frac, 
 	     viewY2_ - ywidth*frac + viewY2_/rangeY_*frac*ywidth); 

  glVertex2f(viewX2_ - xwidth*frac + viewX2_/rangeX_*frac*xwidth, 
	     viewY2_ - ywidth*frac ); 

  glEnd(); 

  
  
}

void ClusterRenderer::fadeInText()
{
  // fade-in sets up a glibmm timer that slowly increases text opacity to 1, 
  // and disables any running timers for this text

  textFadeConn_ = Glib::signal_timeout().connect(sigc::mem_fun(*this, 
							     &ClusterRenderer::fadeInTextHandler), 
					       100,  Glib::PRIORITY_DEFAULT);
  

}


void ClusterRenderer::fadeOutText()
{
  // fade-in sets up a glibmm timer that slowly decreases text opacity to 1
  // and disables any running timers
  std::cout << "ClusterRenderer::fade out text" << std::endl; 


}

bool ClusterRenderer::fadeInTextHandler()
{
  std::cout << "ClusterRenderer::fade in text handler" << std::endl; 

  if (textAlpha_ >= 1.0) {
    textAlpha_ = 1.0; 
    return false;
  } else {
    textAlpha_ += 0.1;
    return true; 
  }

}
