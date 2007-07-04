#ifndef GLSTRING_H
#define GLSTRING_H

#include <string>
#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <string>


typedef GLuint textureID_t; 
struct textprop_t
{
  std::string text; 
  int size; 
}; 
struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

struct textPropCompare
{
  bool operator()(const textprop_t x, const textprop_t  y) const
  {
  return x.text < y.text && x.size < y.size; 
  }
}; 

struct cacheItem_t
{
  textprop_t textprop; 
  textureID_t textureID;
  int textureSize; 
  int extentsX, extentsY; 
}; 

typedef std::list<cacheItem_t> cacheList_t; 
typedef std::map<textprop_t, cacheList_t::iterator, textPropCompare> cacheQueryMap_t; 

class GLString
{
 public: 
  GLString(); 
  ~GLString(); 
  void drawWinText(int x, int y, std::string text, int size); 
  void drawWorldText(float x, float y, std::string text, int size); 
  
 private:
  
  cacheItem_t generateTexture(textprop_t); 
  void setupTexture(); 
  GLuint textureName_; 
  void renderPixLoc(int x, int y, cacheItem_t); 
  void renderWorldLoc(float x, float y, cacheItem_t tp);

  cacheQueryMap_t cacheQueryMap_; 
  cacheList_t cacheList_; 
  cacheItem_t cacheQuery(textprop_t); 
  void cacheDelLRU(void); 
  cacheItem_t cacheAppend(textprop_t); 

}; 

#endif // GLSTRING_H 
