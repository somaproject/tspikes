#include "ratetimelinerenderer.h"
#include "glconfig.h"
#include "shaders.h"

RateTimelineRenderer::RateTimelineRenderer() : 
  BASEDURATION(1000.0), 
  majorTick_(BASEDURATION), 
  minorTick_(BASEDURATION/10.0), 
  viewT1_(0.0), 
  viewT2_(BASEDURATION), 
  viewX1_(0.0), 
  viewX2_(200.0),
  viewportWinX_(100), 
  viewportWinY_(100), 
  isLive_(true), 
  decayRate_(0.01), 
  activePos_(100.0),
  cursorTime_(viewT1_),
  cursorOpacity_(0.0), 
  cursorVisible_(false),
  cursorString_("Mono", false, LEFT)

{

  rates_.reserve(10000); 

}

RateTimelineRenderer::~RateTimelineRenderer()
{
}


void RateTimelineRenderer::setup()
{


  //glEnable(GL_NORMALIZE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
  glEnable(GL_BLEND); 

  glEnable(GL_LINE_SMOOTH); 
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint vshdr = loadGPUShader("rtgrad.vert", GL_VERTEX_SHADER); 
  GLuint fshdr = loadGPUShader("rtgrad.frag", GL_FRAGMENT_SHADER); 
  std::list<GLuint> shaders; 
  shaders.push_back(vshdr); 
  shaders.push_back(fshdr); 
  gpuProgGradient_ = createGPUProgram(shaders); 

}

void RateTimelineRenderer::updateViewingWindow()

{
  glLoadIdentity(); 

  glOrtho(viewT1_, viewT2_, viewX1_, viewX2_, -3, 3); 

  glViewport(0, 0, viewportWinX_, viewportWinY_); 
  
}

bool RateTimelineRenderer::appendRate(RatePoint_t rp)
{
  if (rates_.empty() ){
    viewT2_ = viewT2_ - viewT1_; 
    viewT1_ = rp.time; 
    viewT2_ += viewT1_; 
  }

  rates_.push_back(rp); 

  if (isLive_) {
    activePos_ = rp.time; 
    if (rp.time > viewT2_ ) {
      float width = viewT2_ - viewT1_; 
      viewT2_ = rp.time ; 
      viewT1_ = viewT2_ - width; 
    }
  }
  return true; 

}

void RateTimelineRenderer::drawTicks()
{

  int N = rates_.size(); 


  glLineWidth(1.0); 
  // draw minor ticks
  glColor4f(0.0, 0.0, 1.0, 0.5); 
  
  for (int i = 0; i*minorTick_ < viewT2_ ; i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*minorTick_), viewX1_); 
      glVertex2f(float(i*minorTick_), viewX2_); 

      glEnd(); 
	
    }

  // draw major ticks
  glColor4f(0.7, 0.7, 1.0, 0.9); 

  for (int i = 0; i * majorTick_ <= viewT2_; i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*majorTick_), viewX1_); 
      glVertex2f(float(i*majorTick_), viewX2_); 

      glEnd(); 
	
    }

  // draw actual primary line
}
void RateTimelineRenderer::render()
{


  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  updateViewingWindow(); 

  drawTicks(); 
  
  useGPUProgram(gpuProgGradient_); 
  // now draw filled segment gradient
  GLint vp = glGetUniformLocation(gpuProgGradient_, "decayRate"); 
  glUniform1f(vp, decayRate_); 
  GLint vp2 = glGetUniformLocation(gpuProgGradient_, "activePos"); 
  glUniform1f(vp2, activePos_); 


  // primary polygons (filled)

  for (int i = 1; i < rates_.size() ; i++)
    {
      
       glBegin(GL_POLYGON); 
       timeval_t timel = rates_[i-1].time; 
       timeval_t timeh = rates_[i].time; 
       rateval_t ratel = rates_[i-1].rate; 
       rateval_t rater = rates_[i].rate; 
       glVertex2f(timel, 0.0);
       glVertex2f(timel, ratel); 
       glVertex2f(timeh, rater); 
       glVertex2f(timeh, 0.0); 
       glEnd(); 
     }

  glEnd(); 
   
  useGPUProgram(0); 

  // main line
  glColor4f(1.0, 1.0, 1.0, 1.0); 

  glBegin(GL_LINE_STRIP); 
  for (int i = 0; i < rates_.size() ; i++)
    {
      glVertex2f(rates_[i].time, rates_[i].rate); 
    }

  glEnd(); 

//   std::cout << "rendered between T1 =" << viewT1_ 
// 	    << " and T2 = " << viewT2_ << std::endl; 

  renderCursor(); 
  renderStartCursor(); 
  renderLatestCursor(); 

}

void RateTimelineRenderer::setLive(bool v)
{
  isLive_ = v; 

}

void RateTimelineRenderer::renderCursor()
{
  // The cursor follows the mouse pointer when the mouse pointer
  // is over the drawing area. 

  if (cursorVisible_) {

    // render cursor line

    glLineWidth(4.0); 
    glColor4f(1.0, 1.0, 1.0, 0.5); 
    glBegin(GL_LINES);
    glVertex2f(cursorTime_, viewX1_); 
    glVertex2f(cursorTime_, viewX2_); 
    glEnd(); 

    glColor4f(1.0, 1.0, 1.0, 1.0); 

    cursorString_.drawWorldText(cursorTime_ + 1, viewX1_ + 30, "Test String Off", 10); 

    
  }

} 

void RateTimelineRenderer::renderStartCursor() {

  if ( ! rates_.empty() ){
    glLineWidth(4.0); 
    glColor4f(0.0, 1.0, 0.0, 1.0); 
    glBegin(GL_LINES);
    
    glVertex2f(rates_[0].time+0.1, viewX1_); 
    glVertex2f(rates_[0].time+0.1, viewX2_); 
    glEnd(); 
  }

}

void RateTimelineRenderer::renderLatestCursor() {
  // The Latest Cursor is the cursor at the most-recently-added point

  if ( ! rates_.empty() ){
    glLineWidth(4.0); 
    glColor4f(0.5, 0.5, 1.0, 1.0); 
    glBegin(GL_LINES);
    
    glVertex2f(rates_.back().time, viewX1_); 
    glVertex2f(rates_.back().time, viewX2_); 
    glEnd(); 

    glColor4f(1.0, 1.0, 1.0, 1.0); 

    glPointSize(4.0);
    glBegin(GL_POINTS); 
    glVertex2f(rates_.back().time, rates_.back().rate); 
    glEnd(); 
    
  }

}


void RateTimelineRenderer::setCursorTime(float time)
{
  cursorTime_ = time; 
}

void RateTimelineRenderer::setCursorVisible(bool visible)				    
{
  cursorVisible_ = visible; 

}

viewsignal_t & RateTimelineRenderer::viewSignal()
{
  return viewSignal_; 

}

float RateTimelineRenderer::getViewT1()
{
  return viewT1_; 
}

float RateTimelineRenderer::getViewT2()
{
  return viewT2_; 
}

void RateTimelineRenderer::setViewT(float t1, float t2)
{
  viewT1_ = t1; 
  viewT2_ = t2; 

}

bool RateTimelineRenderer::getLive()
{
  return isLive_; 
}

void RateTimelineRenderer::setActivePos(float x)
{
  activePos_ = x; 

}

bool RateTimelineRenderer::ratesEmpty()
{
  return rates_.empty(); 

}

timeval_t RateTimelineRenderer::getStartTime()
{
  if (!rates_.empty() ) {
    return rates_.front().time; 
  } else {
    return 0.0; 
  }

}


void RateTimelineRenderer::setViewPort(int viewportWinX, int viewportWinY)
{
  viewportWinX_ = viewportWinX; 
  viewportWinY_ = viewportWinY; 


}
