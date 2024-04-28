// Generated with Shader Minifier 1.3.6 (https://github.com/laurentlb/Shader_Minifier/)
#ifndef SHADER_INL_
# define SHADER_INL_
# define VAR_col "y"
# define VAR_u "v"

const char *shader_frag =
 "#version 430\n"
 "out vec3 y;"
 "uniform ivec4 v;"
 "vec2 t(vec3 y,vec3 v)"
 "{"
   "float s=dot(y,v),a=s*s-dot(y,y)+1;"
   "return vec2(-s-sqrt(a),a);"
 "}"
 "void main()"
 "{"
   "float s,a,i,d,f,x,z,l;"
   "vec2 m,r,n,e;"
   "vec3 p,w,c,g,u,C,F;"
   "ivec3 E=ivec3(gl_FragCoord);"
   "s=v.z/3.6e6;"
   "a=mod(s,2);"
   "c=vec3(2*E.xy-v.xy,4*v.y);"
   "p=vec3(0,-1,33),g=vec3(0,1+a/7,-2),c.zy+=c.yz*vec2(1,-1)*(-.05+.08*a);"
   "if(0<s-2)"
     "p=vec3(3,2,-3),g=vec3(0,.2,-2)*(1+a*a),c.xy+=c.yx*vec2(.3,-.3);"
   "if(0<s-4)"
     "p=vec3(0,1,3),g=vec3(2,.5+a/2,-6);"
   "if(0<s-6)"
     "p=vec3(-3,2,6),g=vec3(0,0,-6)*(1+a*a);"
   "c=normalize(c);"
   "p=normalize(p);"
   "d=-g.y/c.y;"
   "w=g+c*d;"
   "m=t(g,c);"
   "r=t(w,p);"
   "g+=m.x*c;"
   "i=-g.y/p.y;"
   "x=length(w.xz);"
   "z=length((g+i*p).xz);"
   "n+=.25;"
   "e=vec2(g.y*5,x*15)+1;"
   "y=vec3(5,2,1)/1e5/pow(1-.999*dot(p,c),2);"
   "u=vec3(5,2,1)/1e5/pow(1-.999*dot(p,reflect(c,g)),2);"
   "for(s=1;s<8;s++)"
     "C+=n.x*(sin(e.x+vec3(0,1,2)/4)+1),F+=n.y*sin(e.y+vec3(2,1,0)/8),n*=.5,n.y*=smoothstep(1.,.6,(1+c.y)*sqrt(abs(dot(normalize(w),c)))*tanh(d*40.)),e=1.99*e+1;"
   "F=abs(F);"
   "l=1-smoothstep(.4,.2,abs(z-2))*(.5+.4*sin(33*z)*cos(5*z));"
   "z=tanh(55*m.y);"
   "if(0<m.x)"
     "{"
       "y*=1-z;"
       "y+=sqrt(u)*pow(1+dot(c,g),8.)+max(dot(p,g),0)*z*C;"
       "if(0<i)"
         "y*=l;"
     "}"
   "y=sqrt(tanh(mix(y,F*smoothstep(.2,0.,r.y*step(0,r.x)),smoothstep(.4,.2,abs(x-2))*(m.x<d?"
     "1-z:"
     "1)*3*F.y)*a*(2-a)));"
   "for(s=1;s<4;s++)"
     "for(a=1;a<5;a++)"
       "{"
         "i=(E.x+E.y*512+v.w)/4/4.8e4;"
         "d=i*a/32+s/3;"
         "f=mod(d,1);"
         "z=i+(E.x&2)*i/50;"
         "l=4*sin(exp2(mod(d-f,3)/6+8.5)*i*a*s);"
         "for(x=3;x<50;z+=x*=1.02)"
           "l+=sin(z*x)/x;"
         "p+=sin(sin(d<9?"
           "i/a/47:"
           "0)*l)*exp2(21-f*13-1/f-s/3-a/3);"
       "}"
   "if(0<v.w)"
     "y=(ivec3(p)>>E.x%2*8)%256/255.;"
 "}";

#endif // SHADER_INL_
