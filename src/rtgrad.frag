uniform float decayRate; 
uniform float activePos; 

varying vec4 passworld;

void main()
{		
  vec4 outcolor = vec4{1.0, 1.0, 1.0, 0.0}; 	

  if (passworld.x < activePos)
  {	
     outcolor.w = 1.0 - (activePos - passworld.x)*decayRate; 
  }

  gl_FragColor = outcolor; 
}	

