#include "clusterrenderer.h"
#include "shaders.h"

ClusterRenderer::ClusterRenderer(GLSPVectpList_t * pspvl, CViewMode cvm)
  : pspvl_(pspvl), 
    decayRate_(0.01), 
    decayMode_(LOG), 
    viewChanged_(false), 
    viewMode_( cvm), 
    viewX1_(0), viewX2_(10), viewY1_(0), viewY2_(10), 
    pCurSPVect_(pspvl->begin()), 
    isSetup_(false),
    gridSpacing_(50e-6)

{

  // configure view pointers
  viewStartIter_ = pspvl_->begin(); 
  viewEndIter_ = pspvl_->end(); 

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

  GLuint vshdr = loadGPUShader("test.vert", GL_VERTEX_SHADER); 
  GLuint fshdr = loadGPUShader("test.frag", GL_FRAGMENT_SHADER); 
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
      renderGrid();   
      renderSpikeVector(*pCurSPVect_, true); 
      
    }
  
  glColor4f(1.0, 1.0, 1.0, 0.4); 

  // render text for axes
  glString_.drawWinText(160, 11, "X", 20); 
  glString_.drawWinText(4, 150, "Y", 20); 
  
  GLenum g = glGetError(); 
  while (g != GL_NO_ERROR) {
    std::cerr << "There was a GL error " << g << " in " << 
      viewMode_ << std::endl; 
    g = glGetError(); 
  }
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
      pCurSPVect_ = --pspvl_->end(); 
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
  // render horizontal and vertical grid
  
  glColor4f(1.0, 1.0, 1.0, 0.2); 

  glBegin(GL_LINES); 

  for (int i = 0; (i*gridSpacing_) < viewX2_; i++)
    {
      float x = float(i) * gridSpacing_; 
      glVertex2f(x, viewY1_); 
      glVertex2f(x, viewY2_); 
    }
  glEnd(); 

  glBegin(GL_LINES); 

  for (int i = 0; (i*gridSpacing_) < viewY2_; i++)
    {
      float y = float(i) * gridSpacing_; 
      glVertex2f(viewX1_, y); 
      glVertex2f(viewX2_, y); 
    }
  glEnd(); 

}

void ClusterRenderer::setGrid(float g)
{
  gridSpacing_ = g; 

}
