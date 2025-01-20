// Generated with Shader Minifier 1.3.6 (https://github.com/laurentlb/Shader_Minifier/)
#ifndef SHADER_INL_
# define SHADER_INL_
# define VAR_col "i"
# define VAR_u "v"

const char *shader_frag =
 "#version 330\n"
 "out vec3 i;"
 "uniform ivec3 v;"
 "void main()"
 "{"
   "float s,d,m,y,g,c,f,l;"
   "vec3 z,e,t,x,C;"
   "d=mod(s=v.z/18e5,2);"
   "e=vec3(2*gl_FragCoord.xy-v.xy,4*v.y);"
   "z=vec3(0,-1,9),t=vec3(0,1+d/5,-2);"
   "e.yz+=e.zy*vec2(-1,1)*.1;"
   "if(0<s-2)"
     "z=vec3(3,2,-3),t=vec3(0,.2,-2)*(1+d),e.yx+=e.xy*vec2(-1,1)*.3;"
   "if(0<s-4)"
     "z=vec3(2,1,9),t=vec3(2,1+d/5,-6);"
   "if(0<s-6)"
     "z=-vec3(0,1,-5),t=vec3(0,1,-5)*(1+d);"
   "e/=length(e);"
   "z/=length(z);"
   "y=-t.y/e.y;"
   "i=t+e*y;"
   "m=-dot(i,z);"
   "m-=sqrt(g=m*m-dot(i,i)+1);"
   "f=m*g;"
   "m=-dot(t,e);"
   "m-=sqrt(g=m*m-dot(t,t)+1);"
   "c=tanh(55*g);"
   "g=length(i);"
   "t+=m*e;"
   "l=.3;"
   "for(s=0;++s<9;l/=2)"
     "x+=l*sin(z/4+2/l*t.y)+l,C+=l*sin(z/4+2/l*g);"
   "i=vec3(5,2,1)/3e3/(1-dot(z,e));"
   "if(0<m)"
     "i=mix(i,max(0,dot(z,t))*x,c);"
   "i=sqrt(tanh(mix(i,C/cosh(9*max(0,f)),C/cosh(9*(2-g))*(1-c*step(m,y))*3)*d*(2-d)));"
   "if(0>v.z)"
     "for(s=0;++s<4;)"
       "for(d=0;++d<5;)"
         "m=(ivec3(gl_FragCoord).x+ivec3(gl_FragCoord).y*512+v.z+1)/2/48e3+350,c=mod(y=min(m*d/32+s/3,9),1),i=(ivec3(z+=sin(sin(m/d/48)*(sin(m*d*s*exp2(mod(y-c,3)/6+8))*4+sin(m*3)+sin(m*11)))*exp2(20-13*c-1/c))>>ivec3(gl_FragCoord).x%2*8)%256/255.;"
 "}";

#endif // SHADER_INL_
