#ifndef SHADERPROGS_H
#define SHADERPROGS_H

#include <string>

static const char* glstring_vertex = "void main()"
"{"
"gl_TexCoord[0] = gl_TextureMatrix[0]*  gl_MultiTexCoord0;"
"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"}"; 

static const std::string glstring_fragment = "uniform float alpha;"
"uniform sampler2DRect tex;"
"void main()"
"{"
"vec4 c = texture2DRect(tex, gl_TexCoord[0].st);"
" c.r = 1.0; "
"c.g = 1.0; "
" c.b = 1.0; "
"c.a *= alpha; "
"gl_FragColor =c ;"
"}"; 

#endif 
