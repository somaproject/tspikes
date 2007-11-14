
varying vec4 passcolor;

void main()
{		 
  vec4 newcolor = passcolor;  

  newcolor.w = 1.0; 
  gl_FragColor = newcolor;
}	

