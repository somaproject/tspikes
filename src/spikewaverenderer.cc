#include "spikewaverenderer.h"
#include "shaders.h"
#include "glconfig.h"
#include "voltage.h"
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <sys/time.h>
#include <time.h>
#include <boost/format.hpp>


SpikeWaveRenderer::SpikeWaveRenderer(GLChan_t chan) : 
  chan_(chan), 
  viewChanged_(false),
  decayVal_(0.02), 
  spikeWaveListFull_(false), 
  spikeWaveListTgtLen_(25), 
  glString_("Mono", false, LEFT),
  ampMin_(-1e-3),
  ampMax_(1e-3), 
  viewX1_(0), 
  viewX2_(SPIKEWAVE_LEN), 
  trigger_(50e-6), 
  isLive_(true)
{

}

SpikeWaveRenderer::~SpikeWaveRenderer()
{

}

void SpikeWaveRenderer::setupOpenGL()
{
    //glEnable(GL_NORMALIZE);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	


  glEnable(GL_LINE_SMOOTH); 
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  render(); 

}

void SpikeWaveRenderer::newSpikeWave(const GLSpikeWave_t & sw)
{

  if (spikeWaveListFull_)
    {
      swl_.push_back(sw); 
      swl_.pop_front(); 
    } 
  else 
    {
      swl_.push_back(sw); 
      if (swl_.size() >= spikeWaveListTgtLen_)
	{
	  spikeWaveListFull_ = true; 

	}
      
    }
  
}

void SpikeWaveRenderer::setListLen(int len)
{
  
  spikeWaveListTgtLen_ = len; 
  spikeWaveListFull_ = false; 
  swl_.clear(); 
  
}

void SpikeWaveRenderer::setViewPort(int viewportWinX, int viewportWinY)
{
  viewportWinX_ = viewportWinX; 
  viewportWinY_ = viewportWinY; 
}

void SpikeWaveRenderer::updateViewingWindow()

{
  
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 


  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 

  glViewport(0, 0, viewportWinX_, viewportWinY_); 

  viewChanged_ = false; 

}

bool SpikeWaveRenderer::renderSpikeWave(const GLSpikeWave_t & sw, 
				    float alpha, 
				    bool plotPoints = false)
{
  // assume we are in a primary GL loop
  // first we render the line  
  switch (chan_) 
    {
    case CHANX:
      glColor4f(1.0, 0.0, 0.0, alpha); 
      break;

    case CHANY:
      glColor4f(0.0, 1.0, 0.0, alpha); 
      break;

    case CHANA:
      glColor4f(0.0, 0.0, 1.0, alpha); 
      break;

    case CHANB:
      glColor4f(1.0, 1.0, 0.0, alpha); 
      break;

    default:
      break;
    }
  

  if (plotPoints)
    {
      glLineWidth(3.0); 
    } 
  else
    {
      glLineWidth(1.0); 
    }
  
  glBegin(GL_LINE_STRIP); 
    for (unsigned int i = 0; i < sw.wave.size(); i++)
    {
      glVertex2f(i, sw.wave[i]); 
    }
  glEnd(); 

  // now we optionally render the points
  if (plotPoints)
    {
      glColor4f(1.0, 1.0, 1.0, alpha); 
      glPointSize(3.0); 
      glBegin(GL_POINTS); 
	for (unsigned int i = 0; i < sw.wave.size(); i++)
	  {
	    glVertex2f(i, sw.wave[i]); 
	  }
      glEnd(); 
    }
  return true; 

}

void SpikeWaveRenderer::render()
{


  updateViewingWindow(); 

  if (isLive_) { 
    glClearColor(0.0, 0.0, 0.0, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
    renderTrigger(); 
    renderGrid(); 
    renderRange(); 
    // real work 
    SpikeWaveList_t::iterator csIter; 
    // always show the last N spikes, and only the last N. 
    float alpha = 0.5; 
    
    for (csIter = swl_.begin(); 
	 csIter != swl_.end(); 
	 csIter ++ ) 
    {
      alpha -= decayVal_; 
      if (alpha < 0.0)
	{
	  alpha = 0.0; 
	}
      renderSpikeWave(*csIter, alpha, false); 
    }
    
    if (! swl_.empty()) 
    {
      renderSpikeWave(swl_.back(), 1.0, true); 
    }
    
    int y = viewportWinY_; 
    glColor4f(1.0, 1.0, 1.0, 1.0); 
    glString_.drawWinText(4, y-15, 
			  boost::str(boost::format("Gain : %1%") % channelState_.gain), 10); 
    if (channelState_.hpf) {
      glString_.drawWinText(4, y-25, "HW HPF: On", 10); 
    } else {
      glString_.drawWinText(4, y-25, "HW HPF: Off", 10); 
    }
    glString_.drawWinText(4, y-35, "Filter : 6 kHz", 10); 
    
    glString_.drawWinText(4, y-45, 
			  boost::str(boost::format("Thold: %s") % Voltage(channelState_.threshold).str()), 10); 
			  
    
  } else { 
    // we are in a "pasued state" 
    // don't show new data

    renderPaused();

  }
    
}

void SpikeWaveRenderer::setTime(uint64_t ts)
{
  currentTime_ = ts; 
}

void SpikeWaveRenderer::renderGrid()
{
  // draw the various vertical markings, at 100 uV
  const float GRIDINC = 100e-6; 

  glLineWidth(3.0); 
   
  glColor4f(1.0, 1.0, 1.0, 1.0); 
  // plot zero line
  glBegin(GL_LINES); 
  glVertex2f(viewX1_, 0.0); 
  glVertex2f(viewX2_, 0.0); 
  glEnd(); 

  glColor4f(1.0, 1.0, 1.0, 0.5); 
  glLineWidth(1.0); 
 
  float ydelta = viewY2_ - viewY1_; 
  int lnumpos = int(viewY2_/GRIDINC); 

  for (int i = 0; i < lnumpos; i++)
    {
      glBegin(GL_LINES); 
      glVertex2f(viewX1_, GRIDINC*(i+1)); 
      glVertex2f(viewX2_, GRIDINC*(i+1)); 
      glEnd(); 
    }
  
  float test = viewY1_ / GRIDINC; 
  lnumpos = int(abs(test)); 

  
  for (int i = 0; i < lnumpos; i++)
    {
      glBegin(GL_LINES); 
      glVertex2f(viewX1_, -GRIDINC*(i+1)); 
      glVertex2f(viewX2_, -GRIDINC*(i+1)); 
      glEnd(); 
    }
  
}


void SpikeWaveRenderer::setPaused(bool state)
{
  

}

void SpikeWaveRenderer::renderPaused()
{

  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  
  int MAXX = viewportWinX_; 
  int MAXY = viewportWinY_; 
  glOrtho(0, MAXX,
	  0, MAXY, -3, 3); 
  
  timeval t; 
  gettimeofday(&t, NULL); 
  float intens = float(t.tv_sec) * 1000000  + float(t.tv_usec); 
  float alpha = sin(intens/1e6 / 5); 

  
  glColor4f(0.7, 0.0, 0.0, 0.3); 
  
  glBegin(GL_POLYGON);
  glVertex2i(0, MAXY/2 + 20); 
  glVertex2i(MAXX, MAXY/2 + 20); 
  glVertex2i(MAXX, MAXY/2 - 20);
  glVertex2i(0, MAXY/2 -20); 
  glEnd(); 

  glColor4f(1.0, 1.0, 1.0, 1.0); 
  glString_.drawWinText(0, MAXY/2, "PAUSED", 30); 


}

void SpikeWaveRenderer::renderRange()
{
 
  // Render the dynamic range Scale -- this is way too complex. 
  
  glDisable(GL_LINE_SMOOTH); 
  
  glPushMatrix(); 
  
  float HEIGHT = 0.3; 
  float Yscale = (ampMax_ - ampMin_) / (viewY2_ - viewY1_) / HEIGHT;  

  float WIDTH = 0.5/10.0; 
  glTranslatef((viewX2_-viewX1_)*(1-WIDTH), viewY2_ - HEIGHT*(viewY2_-viewY1_)/2, 0.0); 
  glScalef(WIDTH, 1.0/Yscale, 1.0); 

  // the full range 
  glLineWidth(1.0); 
  glColor4f(1.0, 1.0, 1.0, 1.0); 
  glBegin(GL_LINE_LOOP);
  glVertex2f(viewX1_, ampMin_); 
  glVertex2f(viewX1_, ampMax_); 
  glVertex2f(viewX2_, ampMax_); 
  glVertex2f(viewX2_, ampMin_); 
  glEnd(); 

  glLineWidth(1.0); 
  glColor4f(1.0, 0.0, 0.0, 1.0); 
  glBegin(GL_LINE_LOOP);
  glVertex2f(viewX1_, viewY1_); 
  glVertex2f(viewX1_, viewY2_); 
  glVertex2f(viewX2_, viewY2_); 
  glVertex2f(viewX2_, viewY1_); 
  glEnd(); 


  glPopMatrix(); 
  glEnable(GL_LINE_SMOOTH); 
  
}

void SpikeWaveRenderer::setLive(bool l)
{
  isLive_ = l; 
  viewChanged_ = true; 
}

void SpikeWaveRenderer::setAmplitudeRange(float min, float max)
{
  ampMin_ = min; 
  ampMax_ = max; 
 
  sanityCheckViewParameters(); 

}

void SpikeWaveRenderer::setAmplitudeView(float min, float max)
{
  // we'd like our scale to look at ~ volts. 
  viewY1_ = min; 
  viewY2_ = max; 

 
  sanityCheckViewParameters(); 


}

void SpikeWaveRenderer::sanityCheckViewParameters()
{

  // first, make sure our view is a subset of our range
  if (ampMin_ > viewY1_) {
    viewY1_ = ampMin_; 
  }

  if (ampMax_ < viewY2_) {
    viewY2_ = ampMax_; 
  }
  
  // if we have a non-zero amplitude and we've somehow gotten
  // ourselves looking at a zero range, reset to sensible defaults

  if (ampMax_ > 0 and viewY1_ == viewY2_) {
    viewY1_ = ampMin_; 
    viewY2_ = ampMax_; 
  }
  
}

void SpikeWaveRenderer::getAmplitudeView(float * min, float * max)
{
  *min = viewY1_; 
  *max = viewY2_; 

}
void SpikeWaveRenderer::renderTrigger()
{
  glLineWidth(3.0);
  float alpha = 0.5; 
  switch (chan_) 
    {
    case CHANX:
      glColor4f(1.0, 0.0, 0.0, alpha); 
      break;

    case CHANY:
      glColor4f(0.0, 1.0, 0.0, alpha); 
      break;

    case CHANA:
      glColor4f(0.0, 0.0, 1.0, alpha); 
      break;

    case CHANB:
      glColor4f(1.0, 1.0, 0.0, alpha); 
      break;

    default:
      break;
    }

  glEnable(GL_LINE_STIPPLE); 
  glLineStipple(1, 0xFF); 
  
  glBegin(GL_LINE_LOOP);
  glVertex2f(viewX1_, trigger_); 
  glVertex2f(viewX2_, trigger_); 
  glEnd(); 
  glDisable(GL_LINE_STIPPLE); 

}

void SpikeWaveRenderer::setTriggerThreshold(float thold)
{
  trigger_ = thold; 
  
}

void SpikeWaveRenderer::updateState(TSpikeChannelState ts)
{
  channelState_ = ts; 
  trigger_ = Voltage(channelState_.threshold).to_volts(); 
}
