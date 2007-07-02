#include "glstring.h"
#include <iostream>

GLString::GLString(std::string text, int size) :
  text_(text), 
  size_(size)
{
  
  setupTexture(); 
  generateTexture(); 


}

GLString::~GLString()
{


}

void GLString::setupTexture()
{
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &textureName_); 
  glBindTexture(GL_TEXTURE_2D, textureName_); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  
  glDisable(GL_TEXTURE_2D); 

}

void GLString::generateTexture()
{
  int TEXN = 128; 
  Cairo::RefPtr<Cairo::ImageSurface> surface =
    Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, TEXN, TEXN);
  
  Cairo::RefPtr<Cairo::Context> pContext = 
    Cairo::Context::create(surface); 



  pContext->set_source_rgba(1.0, 1.0, 1.0, 1.0);  
  pContext->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL,
 			     Cairo::FONT_WEIGHT_NORMAL); 
  pContext->set_font_size(size_); 

  Cairo::TextExtents te; 
  pContext->get_text_extents(text_, te); 
  pContext->move_to(0, te.height);

  pContext->show_text(text_); 
  
  glEnable(GL_TEXTURE_2D);
  
  glBindTexture(GL_TEXTURE_2D, textureName_); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXN, TEXN, 0, 
	       GL_BGRA, GL_UNSIGNED_BYTE, surface->get_data()); 
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glDisable(GL_TEXTURE_2D); 

}

void GLString::render()
{
  glEnable(GL_TEXTURE_2D);
  
  glBindTexture(GL_TEXTURE_2D, textureName_); 

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  GLint x[4]; 
  glGetIntegerv(GL_VIEWPORT, x); 
  
  glPushMatrix(); 
  glLoadIdentity(); 
  glOrtho(float(x[0]), float(x[2]), float(x[1]), float(x[3]), -3.0, 3.0); 
  
  glBegin(GL_QUADS);


  glTexCoord2f(0.0, 0.0); 
  glVertex2f(0.0, 128.0);
  
  glTexCoord2f(1.0, 0.0); 
  glVertex2f(128.0, 128.0);
  
  glTexCoord2f(1.0, 1.0); 
  glVertex2f(128.0, 00);
  
  glTexCoord2f(0.0, 1.0);  
  glVertex2f(0.0, 00);
  
  glEnd();
  glDisable(GL_TEXTURE_2D); 
  
  glPopMatrix();
  
}
