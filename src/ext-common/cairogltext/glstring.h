#ifndef GLSTRING_H
#define GLSTRING_H

#include <string>
#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <string>
#include "shaderutil/shaders.h"


typedef GLuint textureID_t; 
struct textprop_t
{
  std::string text; 
  int size; 
}; 

struct textPropCompare
{
  bool operator()(const textprop_t x, const textprop_t  y) const
  {
    return (x.text < y.text) or x.size < y.size; 
  }
}; 

struct cacheItem_t
{
  textprop_t textprop; 
  textureID_t textureID;
  int texWidth, texHeight; 
  int extentsX, extentsY; 
}; 

typedef std::list<cacheItem_t> cacheList_t; 
typedef std::map<textprop_t, cacheList_t::iterator, textPropCompare> cacheQueryMap_t; 

enum StringHPos { LEFT, CENTER, RIGHT}; 
enum StringVPos { BASELINE, MEDIAN}; 

		 
class GLString
{
 public: 
  GLString(std::string family, bool isBold, 
	   StringHPos hpos, StringVPos vpos = BASELINE); 
  ~GLString(); 
  void drawWinText(int x, int y, std::string text, int size, float alpha =1.0); 
  void drawWorldText(float x, float y, std::string text, int size, float alpha=1.0); 
  
 private:
  std::string family_; 
  bool isBold_;
  StringHPos hpos_; 
  StringVPos vpos_; 

  GLuint gpuProg_; 
  bool gpuProgCompiled_; 

  void checkGPUProgCompiled(); 

  cacheItem_t generateTexture(textprop_t); 
  void setupTexture(); 
  GLuint textureName_; 
  void renderPixLoc(int x, int y, cacheItem_t, float alpha); 
  void renderWorldLoc(float x, float y, cacheItem_t tp, float alpha);
  
  cacheQueryMap_t cacheQueryMap_; 
  cacheList_t cacheList_; 
  cacheItem_t cacheQuery(textprop_t); 
  void cacheDelLRU(void); 
  cacheItem_t cacheAppend(textprop_t); 
  
  void unpremultiplyAlpha(unsigned char * pbuf, 
			  int width, int height, int stride); 
 
}; 

#endif // GLSTRING_H 
