uniform float alpha;

uniform sampler2DRect tex; 

void main()
{
	
vec4 c = texture2DRect(tex, gl_TexCoord[0].st);
c.r = 1.0; 
c.g = 1.0; 
c.b = 1.0; 
c.a *= alpha; 
gl_FragColor =c ; 

}
