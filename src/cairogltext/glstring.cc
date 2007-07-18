#include "glstring.h"
#include <iostream>
#include <assert.h>

GLString::GLString(std::string family, 
		   bool isBold, 
		   StringHPos hpos):
  family_(family), 
  isBold_(isBold), 
  hpos_(hpos)
{
  
}

GLString::~GLString()
{


}

void GLString::drawWinText(int x, int y, std::string text, int size)
{
  textprop_t tp; 
  tp.text = text; 
  tp.size = size; 

  cacheItem_t ci = cacheQuery(tp); 
  renderPixLoc(x, y, ci); 
  
}
void GLString::drawWorldText(float x, float y, std::string text, int size)
{
  textprop_t tp; 
  tp.text = text; 
  tp.size = size; 

  cacheItem_t ci = cacheQuery(tp); 
  renderWorldLoc(x, y, ci); 
  
}

void GLString::setupTexture()
{

}

cacheItem_t GLString::generateTexture(textprop_t tp)
{

  textureID_t textureName; 
  // generate the GL texture
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &textureName); 
  glBindTexture(GL_TEXTURE_2D, textureName); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  
  glDisable(GL_TEXTURE_2D); 


  
  // Create a tiny surface to begin with simply so we can get the extents
  int texn = 32; 
  Cairo::RefPtr<Cairo::ImageSurface> surface =
    Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, texn, texn);
  
  Cairo::RefPtr<Cairo::Context> pContext = 
    Cairo::Context::create(surface); 

  pContext->set_source_rgba(1.0, 1.0, 1.0, 1.0);  

  if (isBold_) {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_BOLD); 
  } else {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL); 
  }    
  
  pContext->set_font_size(tp.size); 

  Cairo::TextExtents te; 
  pContext->get_text_extents(tp.text, te); 
  pContext->move_to(0, te.height);

  // compute the new texn;
  texn = 32; 
  while (texn < te.height or texn < te.width) 
    {
      texn = texn * 2;  // powers of two 
    }
  
  // then we do it all over again

  surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, texn, texn); 
  pContext = Cairo::Context::create(surface); 

  pContext->set_source_rgba(1.0, 1.0, 1.0, 1.0);  
  
  if (isBold_) {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_BOLD); 
  } else {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL); 
  }    
  

  pContext->set_font_size(tp.size); 

  pContext->move_to(0, te.height);
  
  pContext->show_text(tp.text); 
  
  glEnable(GL_TEXTURE_2D);
  
  glBindTexture(GL_TEXTURE_2D, textureName); 

  
  // test process; 
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texn, texn, 0, 
	       GL_BGRA, GL_UNSIGNED_BYTE, surface->get_data()); 

  glDisable(GL_TEXTURE_2D); 

  cacheItem_t ret; 
  ret.textprop = tp; 
  ret.textureID = textureName; 
  ret.textureSize = texn;
  ret.extentsX = te.width; 
  ret.extentsY = te.height; 
  return ret; 

}

void GLString::renderWorldLoc(float x, float y, cacheItem_t tp)
{
  glEnable(GL_TEXTURE_2D);
  
  glBindTexture(GL_TEXTURE_2D, tp.textureID); 

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  
  GLdouble model[16] ;
  GLdouble proj[16] ;
  GLint vp[4]; 
  glGetDoublev(GL_MODELVIEW_MATRIX, model) ;
  glGetDoublev(GL_PROJECTION_MATRIX, proj) ;
  glGetIntegerv(GL_VIEWPORT, vp); 
  
  GLdouble objX = x, objY = y, objZ = 0 ;
  GLdouble winX, winY, winZ ;

  gluProject(objX, objY, objZ,
 	     model, proj, vp,
 	     &winX, &winY, &winZ ) ;
  
  
  glPushMatrix(); 
  glLoadIdentity(); 
  // convert to pixel coords 

  glOrtho(float(vp[0]), float(vp[2]), float(vp[1]), float(vp[3]), -3.0, 3.0); 

  glTranslatef(float(winX), 
	       float(winY) - float(tp.textureSize) + float(tp.extentsY)/2.0, 0.); 
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0); 
  glVertex2f(0.0, float(tp.textureSize));
  
  glTexCoord2f(1.0, 0.0); 
  glVertex2f(float(tp.textureSize), float(tp.textureSize));
  
  glTexCoord2f(1.0, 1.0); 
  glVertex2f(float(tp.textureSize), 0.0);
  
  glTexCoord2f(0.0, 1.0);  
  glVertex2f(0.0, 0.0);
  
  glEnd();
  glDisable(GL_TEXTURE_2D); 
  
  glPopMatrix();
  
}

void GLString::renderPixLoc(int x, int y, cacheItem_t tp)
{
  // helps on getting exact rasterization
  //  http://msdn2.microsoft.com/en-us/library/ms537007.aspx

  GLint vp[4]; 
  glGetIntegerv(GL_VIEWPORT, vp); 

  glMatrixMode(GL_PROJECTION); 
  glPushMatrix(); 
  glLoadIdentity(); 

  glEnable(GL_TEXTURE_2D);
  
  glBindTexture(GL_TEXTURE_2D, tp.textureID); 

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // convert to pixel coords
  
  gluOrtho2D(vp[0], vp[2], vp[1], vp[3]); 
  
  glTranslatef(float(x), 
	       float(y) - float(tp.textureSize) + float(tp.extentsY)/2.0, 0.); 
  
  glMatrixMode (GL_MODELVIEW); 
  glPushMatrix(); 
  glLoadIdentity(); 
  glTranslatef(0.375, 0.375, 0.0); 
  //glColor4f(1.0, 0.0, 0.0, 1.0); 
  glBegin(GL_QUADS);
  
  
  glTexCoord2f(0.0, 0.0); 
  glVertex2i(0, tp.textureSize);
 
  glTexCoord2f(1.0, 0.0); 
  glVertex2i(tp.textureSize, tp.textureSize);
  
  glTexCoord2f(1.0, 1.0); 
  glVertex2i(tp.textureSize, 0);
  
  glTexCoord2f(0.0, 1.0);  
  glVertex2i(0, 0);
  
  glEnd();
  glPopMatrix(); 
 
  
  glDisable(GL_TEXTURE_2D); 
  
  
  glMatrixMode(GL_PROJECTION); 
  glPopMatrix();
  
}

cacheItem_t GLString::cacheQuery(textprop_t tp)
{
  // perform lookup in the map
  cacheQueryMap_t::iterator pos; 
  pos = cacheQueryMap_.find(tp); 
  if (pos != cacheQueryMap_.end() ) {
    // It was a hit, return the resulting cache item
    return *pos->second; 

  }  else {
    //  it's a miss
    if (cacheList_.size() > 100) {
      cacheDelLRU(); 
    }
    // add this to the cache
    return  cacheAppend(tp); 
  }

}

void GLString::cacheDelLRU()
{
  // delete the oldest
  cacheItem_t tpdel = cacheList_.back(); 
  

  cacheList_t::iterator tppdel = cacheQueryMap_[tpdel.textprop]; 
  GLuint tdel[1]; 
  tdel[0]  = tppdel->textureID; 
  glDeleteTextures( tppdel->textureSize, tdel); 
  cacheQueryMap_.erase(tppdel->textprop); 
  cacheList_.erase(tppdel); 
  
}

cacheItem_t GLString::cacheAppend(textprop_t tp)
{
  
  cacheItem_t tid = generateTexture(tp); 
  cacheList_.push_front(tid); 
  cacheList_t::iterator i = cacheList_.begin(); 
  
  // add to the relevant caches
  cacheQueryMap_[tid.textprop] = i; 
  
  return *i; 
  
}
