#ifndef GLSTRING_H
#define GLSTRING_H

#include <string>
#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

class GLString
{
 public: 
  GLString(std::string text, int size); 
  ~GLString(); 
  void setText(std::string text); 
  void render(); 

 private:
  std::string text_; 
  int size_; 
  GLfloat x, y; 
  void generateTexture(); 
  void setupTexture(); 
  GLuint textureName_; 
  
  

}; 

#endif // GLSTRING_H 
