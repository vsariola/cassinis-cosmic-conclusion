// Generated with Shader Minifier 1.3.6 (https://github.com/laurentlb/Shader_Minifier/)
#ifndef SHADER_INL_
# define SHADER_INL_
# define VAR_fcol "i"
# define VAR_u "d"

const char *shader_frag =
 "#version 130\n"
 "out vec3 i;"
 "uniform ivec4 d;"
 "void main()"
 "{"
   "vec3 f=vec3(0);"
   "int m=int(gl_FragCoord.x);"
   "float s=(m/4+gl_FragCoord.y*128+65536*d.w)/44100;"
   "for(float v=1.;v<4.;v++)"
     "for(float e=1.;e<5.;e++)"
       "{"
         "float g=s*e/32.+v/3.,y=mod(g,1.),t=3.,u=s*(1+.01*(m&2)),w=u+4.*sin(exp2(mod(g-y,3.)/6.+8.5)*s*e*v);"
         "for(;t<50.;u+=t*=1.02)"
           "w+=sin(u*t)/t;"
         "if(g<9.)"
           "f+=sin(sin(s/e/47.)*w)*exp2(-y*12.-1./y+5.-v/3.-e/3.);"
       "}"
   "i=trunc(mod(f*((m&1)==0?"
     "256:"
     "1),1.)*256.)/255.;"
 "}";

#endif // SHADER_INL_
