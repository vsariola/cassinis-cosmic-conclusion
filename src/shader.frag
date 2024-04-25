/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2024 Mårten Rånge

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#version 130

// ----------------------------------------------------------------------------
// Required prelude

// The result of the shader
out vec3 fcol;
uniform ivec4 u;

// ----------------------------------------------------------------------------
// The Shader
//  The entrypoint is: void main()
//  gl_FragCoord is the input fragment position
//  fcol is the output fragment color

mat2 rot(float a) {
  return mat2(cos(a),sin(a),-sin(a),cos(a));
}

void main() {   
  vec2 p = (2*gl_FragCoord.xy-u.xy)/u.y;  
  vec3 col = vec3(0);
    
  int sx = int(gl_FragCoord.x);
  float t = (sx/4+gl_FragCoord.y*128+65536*u.a)/44100;

  for(float i=1.;i<4.;i++)
  for(float j=1.;j<5.;j++) {        
    float r = t*j/32.+i/3.,
          v = mod(r,1.),
          a = 3.,
          s = t*(1+.01*(sx&2)),
          m = s + 4.*sin(exp2(mod(r-v,3.)/6.+8.5)*t*j*i);
        for (;a<50.;s += a *= 1.02)
            m += sin(s*a)/a;
        if(r<9.)                    
           col += sin(sin(t/j/47.)*m)*exp2(-v*12.-1./v+5.-i/3.-j/3.);
                
    }
  fcol = trunc(mod(col*((sx&1)==0?256:1),1.0)*256.)/255.;
   
}
