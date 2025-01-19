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
out vec3 col;
uniform ivec3 u;

void main() {
  float i1,j1,t1,r1,a1,s1;
  vec2 a2, b2,c2,d2;
  vec3 a3, b3,c3,d3,f3,g3;
    
    j1   = mod(i1 = u.z/18e5,2);
    c3  = vec3(2*gl_FragCoord.xy-u.xy, 4*u.y);

    a3  = vec3(0, -1, 9),
    d3 = vec3(0, 1+j1/5, -2);
    c3.zy+=c3.yz*vec2(1,-1)*.1;

    if (0<i1-2)
        a3  = vec3(3, 2, -3),
        d3 = vec3(0, .2, -2)*(1+j1*j1),
        c3.xy+=c3.yx*vec2(1,-1)*.3;

    if (0<i1-4)
        a3  = vec3(2, 1, 9),
        d3 = vec3(2, 1+j1/5, -6);

    if (0<i1-6)
        a3  = -vec3(0, 1, -5),
        d3 = vec3(0, 1, -5)*(1+j1*j1);

      c3  = c3/length(c3);
      a3  = a3/length(a3);

      r1 = -d3.y/c3.y;

      b3  = d3+c3*r1;

      t1 = dot(b3, a3);      
      b2 = vec2(-t1-sqrt(a1=t1*t1-dot(b3, b3)+1), a1);

      t1 = dot(d3, c3);      
      a2 = vec2(-t1-sqrt(a1=t1*t1-dot(d3, d3)+1), a1);

      d3 += a2.x*c3;

      c2 += .3;
      d2 = vec2(d3.y,a1 = length(b3))*6;


      for (i1 = 0; ++i1< 8;d2 += d2)
        f3 += c2.x*sin(a3/2+d2.x)+c2.x,
        g3 += c2.y*sin(a3/2+d2.y),
        c2 /= 2;      

      s1 = tanh(55*a2.y);

      col = vec3(5, 2, 1)/3e3/(1-dot(a3, c3));

      if (0<a2.x)
        col = mix(col,
            max(0,dot(a3, d3))*f3,
            s1);

      col = sqrt(tanh(
        mix(col,
            g3*smoothstep(.2, 0, b2.y*max(0, b2.x)),
            smoothstep(.4, .2, abs(2-a1))*(1-s1*step(a2.x,r1))*3*g3.y
        )*j1*(2-j1)
      ));

    
  if (0>u.z)
  for(i1=0;++i1<4;)
  for(j1=0;++j1<5;) {    
    t1 = (ivec3(gl_FragCoord).x+ivec3(gl_FragCoord).y*512+u.z+1)/2/48e3+350;
    s1 = mod(r1 = min(t1*j1/32+i1/3,9),1);
    r1 = 4*sin(exp2(mod(r1-s1,3)/6+8)*t1*j1*i1);
    for (a1=0;++a1<99;r1 += sin(t1*a1)/a1);        
    col = (ivec3(a3 += sin(sin(t1/j1/47)*r1)*exp2(20-13*s1-1/s1))>>ivec3(gl_FragCoord).x%2*8)%256/255.;
  }

}
