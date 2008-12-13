#include "glstring.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include "shaderprogs.h"

GLString::GLString(std::string family, 
		   bool isBold, 
		   StringHPos hpos, 
		   StringVPos vpos):
  family_(family), 
  isBold_(isBold), 
  hpos_(hpos), 
  vpos_(vpos), 
  gpuProgCompiled_(false)
{
  
}

GLString::~GLString()
{


}

void GLString::drawWinText(int x, int y, std::string text, int size, 
			   float alpha)
{
  textprop_t tp; 
  tp.text = text; 
  tp.size = size; 

  cacheItem_t ci = cacheQuery(tp); 
  renderPixLoc(x, y, ci, alpha); 
  
}
void GLString::drawWorldText(float x, float y, std::string text, int size,
			     float alpha)
{
  textprop_t tp; 
  tp.text = text; 
  tp.size = size; 

  cacheItem_t ci = cacheQuery(tp); 
  renderWorldLoc(x, y, ci, alpha); 
  
}

void GLString::setupTexture()
{

}

cacheItem_t GLString::generateTexture(textprop_t tp)
{
  checkGPUProgCompiled(); 

  // Create the string in cairo, render it to an image surface
  // and turn that image surface into a gl texture
  
  // Create a tiny surface to begin with simply so we can get the extents
  int texn = 32; 

  Cairo::FontOptions fo; 
  fo.set_hint_style(Cairo::HINT_STYLE_FULL); 
  Cairo::RefPtr<Cairo::ImageSurface> surface =
    Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, texn, texn);
  
  Cairo::RefPtr<Cairo::Context> pContext = 
    Cairo::Context::create(surface); 

  pContext->set_font_options(fo); 
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

  // compute the new texn;
  int texWidth, texHeight; 
  texWidth = te.width + 4; 
  texHeight = te.height + 4; 

  // then we do it all over again

  surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, texWidth,
					texHeight); 

  pContext = Cairo::Context::create(surface); 
  pContext->set_font_options(fo); 

  pContext->set_source_rgba(1.0, 1.0, 1.0, 1.0);  
  
  if (isBold_) {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_BOLD); 
  } else {
    pContext->select_font_face(family_.c_str(), Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL); 
  }    
  

  pContext->set_font_size(tp.size); 

  pContext->move_to(0.5, te.height + 0.5);
  
  pContext->show_text(tp.text); 

  // now, remove the premultiplied texture
  unsigned char * data = surface->get_data(); 

  // FIXME Texture all white
  int pos  = 0;
  while (pos < texHeight * texWidth * 4) {
    //data[pos] = 255; 
    pos++; 
  }
  
  // opengl bind the texture

  textureID_t textureName; 
  // generate the GL texture
  glEnable(GL_TEXTURE_RECTANGLE_ARB); 
  glGenTextures(1, &textureName); 
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureName); 
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  
  
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureName); 
  
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, texWidth, texHeight, 0, 
	       GL_BGRA, GL_UNSIGNED_BYTE, data); 

  glDisable(GL_TEXTURE_RECTANGLE_ARB); 

  // save results when debugging
//   std::ofstream myFile;
//   myFile.open ("data2.rgba", std::ios::out | std::ios::binary);
//   myFile.write((const char *)surface->get_data(), texWidth * texHeight * 4); 
//   std::cout << "size :" << texWidth << " "  << texHeight << std::endl;

  cacheItem_t ret; 
  ret.textprop = tp; 
  ret.textureID = textureName; 
  ret.texWidth = texWidth;
  ret.texHeight = texHeight; 
  ret.extentsX = te.width; 
  ret.extentsY = te.height; 
  return ret; 

}

void GLString::checkGPUProgCompiled()
{
  // If we've never compiled the shaders before, do so.
  if (!gpuProgCompiled_) {
    
    GLuint vshdr = loadGPUShader(glstring_vertex, GL_VERTEX_SHADER); 
    GLuint fshdr = loadGPUShader(glstring_fragment, GL_FRAGMENT_SHADER); 
    std::list<GLuint> shaders; 
    shaders.push_back(vshdr); 
    shaders.push_back(fshdr); 
    gpuProg_ = createGPUProgram(shaders); 

    GLint texSampler; 
    texSampler = glGetUniformLocation(gpuProg_, "tex"); 
    glUniform1i(texSampler, GL_TEXTURE0); 

    gpuProgCompiled_  = true; 
  }

}
void GLString::renderWorldLoc(float x, float y, cacheItem_t tp, float alpha)
{
  checkGPUProgCompiled(); 
  useGPUProgram(gpuProg_); 

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

  glActiveTexture(GL_TEXTURE0); 
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tp.textureID); 
  
  GLint unialpha = glGetUniformLocation(gpuProg_,"alpha");
  glUniform1f(unialpha, alpha); 

  if (hpos_ == LEFT) {
    // nothing
  } else if (hpos_ == CENTER ) {
    winX -= tp.texWidth/2; 
  }
  glTranslatef(float(winX), 
	       float(winY) - float(tp.texHeight) + float(tp.extentsY)/2.0, 0.); 
  glBegin(GL_QUADS);

  glTexCoord2i(0, 0); 
  glVertex2f(0.0, float(tp.texHeight));
  
  glTexCoord2i(tp.texWidth, 0); 
  glVertex2f(float(tp.texWidth), float(tp.texHeight));
  
  glTexCoord2i(tp.texWidth, tp.texHeight); 
  glVertex2f(float(tp.texWidth), 0.0);
  
  glTexCoord2i(0, tp.texHeight);  
  glVertex2f(0.0, 0.0);
  
  glEnd();
  glDisable(GL_TEXTURE_RECTANGLE_ARB); 
  
  glPopMatrix();

  useGPUProgram(0); 
  
}

void GLString::renderPixLoc(int x, int y, cacheItem_t tp, float alpha)
{
  // helps on getting exact rasterization
  //  http://msdn2.microsoft.com/en-us/library/ms537007.aspx
  checkGPUProgCompiled(); 
  useGPUProgram(gpuProg_); 

  GLint vp[4]; 
  glGetIntegerv(GL_VIEWPORT, vp); 

  glMatrixMode(GL_PROJECTION); 
  glPushMatrix(); 
  glLoadIdentity(); 

  // convert to pixel coords
  
  gluOrtho2D(vp[0], vp[2], vp[1], vp[3]); 
  
  glTranslatef(float(x), 
	       float(y) - float(tp.texHeight) + float(tp.extentsY)/2.0, 0.); 
  
  glMatrixMode (GL_MODELVIEW); 
  glPushMatrix(); 
  glLoadIdentity(); 
  glTranslatef(0.375, 0.375, 0.0); 

  glActiveTexture(GL_TEXTURE0); 
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tp.textureID); 

  
  // set alpha
  GLint unialpha = glGetUniformLocation(gpuProg_,"alpha");
  glUniform1f(unialpha, alpha); 

  glColor4f(1.0, 1.0, 1.0, 1.0); 
  glBegin(GL_QUADS);

  glTexCoord2i(0, 0); 
  glVertex2f(0.0, float(tp.texHeight));
  
  glTexCoord2i(tp.texWidth, 0); 
  glVertex2f(float(tp.texWidth), float(tp.texHeight));
  
  glTexCoord2i(tp.texWidth, tp.texHeight); 
  glVertex2f(float(tp.texWidth), 0.0);
  
  glTexCoord2i(0, tp.texHeight);  
  glVertex2f(0.0, 0.0);
  
  glEnd();
  glPopMatrix(); 
 
  
  glDisable(GL_TEXTURE_RECTANGLE_ARB); 
  
  
  glMatrixMode(GL_PROJECTION); 
  glPopMatrix();
  
  useGPUProgram(0); 
 
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
  glDeleteTextures(1, tdel); 
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


int to_int (float fValue)
{
  fValue = floor (fValue * 256);
  if (fValue > 255)
    return 255;
  else
    return (int) fValue;
}
float d1 (int iValue)
{
return iValue / 255.0f;
}

float d2 (int iValue)
{
return (iValue + 0.5f) / 256.0f;
}

void GLString::unpremultiplyAlpha(unsigned char* pbuf, 
				  int width, int height, int stride)
{
  
  // based on code by macslow, gl-cairo-aatrick

  int iX = 0;
  int iY = 0;
  
  assert (pbuf != NULL);
  /* get rid of premultiplied-alpha for use in OpenGL */
  for (iX = 0; iX < width; iX++)
    {
      for (iY = 0; iY < height; iY++)
	{
	  unsigned char ucRed;
	  unsigned char ucGreen;
	  unsigned char ucBlue;
	  unsigned char ucAlpha;
	
	  ucRed = pbuf[iY * stride + iX * 4 + 2];
	  ucGreen = pbuf[iY * stride + iX * 4 + 1];
	  ucBlue = pbuf[iY * stride + iX * 4 + 0];
	  ucAlpha = pbuf[iY * stride + iX * 4 + 3];
	
	  ucRed = to_int (d2 (ucRed) / d1 (ucAlpha));
	  ucGreen = to_int (d2 (ucGreen) / d1 (ucAlpha));
	  ucBlue = to_int (d2 (ucBlue) / d1 (ucAlpha));

	  pbuf[iY * stride + iX * 4 + 0] = ucBlue;
	  pbuf[iY * stride + iX * 4 + 1] = ucGreen;
	  pbuf[iY * stride + iX * 4 + 2] = ucRed;
	}
    }
}
