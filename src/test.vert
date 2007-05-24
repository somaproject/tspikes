uniform int axes; 

varying vec4 passcolor; 

void main()
{
  // transform the vertex
  vec4 point; 
  if (axes == 0) {
     point = gl_Vertex; 
  } else if (axes == 1) {
     point = gl_Vertex.xzyw; 

  } else if (axes == 2) {
     point = gl_Vertex.xwzy; 

  } else if (axes == 3) {
      point = gl_Vertex.yzxw; 

  } else if (axes == 4) {
     point = gl_Vertex.ywxz; 

  } else if (axes == 5) {
     point = gl_Vertex.zwxy; 
  }

  point.w = 1.0; 
  // eliminate w point
  gl_Position = gl_ModelViewProjectionMatrix * point; 

 passcolor = gl_Color; 

}
