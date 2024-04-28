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

#version 430

// ----------------------------------------------------------------------------
// Required prelude

// The result of the shader
out vec3 col;
uniform ivec4 u;

void main() {
  float i1,j1,t1,r1,v1,a1,s1,m1;
  vec2 a2, b2,c2,d2;
  vec3 a3, b3,c3,d3,e3,f3,g3;
  ivec3 coord = ivec3(gl_FragCoord);
  
    i1 = u.z/36e5;
    j1   = mod(i1,2);
    c3  = vec3(2*coord.xy-u.xy, 4*u.y);

    a3  = vec3(0, -1, 33),
    d3 = vec3(0, 1+j1/7, -2),
    c3.zy+=c3.yz*vec2(1,-1)*(-.05+.08*j1);
    
    if (0<i1-2)
        a3  = vec3(3, 2, -3),
        d3 = vec3(0, .2, -2)*(1+j1*j1),
        c3.xy+=c3.yx*vec2(.3,-.3);

    if (0<i1-4)
        a3  = vec3(0, 1, 3),
        d3 = vec3(2, .5+j1/2, -6);
        
    if (0<i1-6)
        a3  = vec3(-3, 2, 6),
        d3 = vec3(0, 0, -6)*(1+j1*j1);

      c3  = normalize(c3);
      a3  = normalize(a3);

      r1 = -d3.y/c3.y;

      b3  = d3+c3*r1;

      t1 = dot(b3, a3);a1=t1*t1-dot(b3, b3)+1;
      b2 = vec2(-t1-sqrt(a1), a1);

      t1 = dot(d3, c3);a1=t1*t1-dot(d3, d3)+1;
      a2 = vec2(-t1-sqrt(a1), a1);

      d3 = d3+a2.x*c3;

      t1 = -d3.y/a3.y;
      a1 = length(b3.xz);
      s1 = length((d3+t1*a3).xz);

      c2 += .25;
      d2 = vec2(d3.y*5,a1*15)+1;

      for (i1 = 1; i1 < 8; i1++) {
        f3 += c2.x*(sin(d2.x+vec3(0, 1, 2)/4)+1);
        g3 += c2.y*(sin(d2.y+vec3(2, 1, 0)/8));
        c2 *= .5;
        c2.y *= smoothstep(1., .6, (1+c3.y)*sqrt(abs(dot(normalize(b3), c3)))*tanh(r1*4E1));
        d2 = 1.99*d2+1;
      }
      g3 = abs(g3);

      m1 = 1-smoothstep(.4, .2, abs(s1-2))*(.5+.4*sin(33*s1)*cos(5*s1))*step(0,t1);
      s1 = tanh(55*a2.y);

      col = vec3(5, 2, 1)/pow(300.2-300*dot(a3, c3), 2);
      e3  = vec3(5, 2, 1)/pow(300.2-300*dot(a3, reflect(c3, d3)), 2);

      if (0<a2.x) col = mix(col, sqrt(e3)*pow(1+dot(c3, d3),8)+max(dot(a3, d3), 0)*s1*f3*m1, s1);

      col = sqrt(tanh(
        mix(col,
            g3*smoothstep(2E-1, 0., b2.y*step(0, b2.x)),
            smoothstep(.4, .2, abs(a1-2))*(a2.x < r1?1-s1:1)*3*g3.y
        )*j1*(2-j1)
      ));
  

  
  for(i1=1;i1<4;i1++)
  for(j1=1;j1<5;j1++) {
    t1 = (coord.x+coord.y*512+u.a)/4/48E3;
    r1 = t1*j1/32+i1/3;
    v1 = mod(r1,1);
    s1 = t1+(coord.x&2);
    m1 = 4*sin(exp2(mod(r1-v1,3)/6+8.5)*t1*j1*i1);
    for (a1=3;a1<50;s1 += a1 *= 1.02)
        m1 += sin(s1*a1)/a1;
    a3 += sin(sin(r1<9?t1/j1/47:0)*m1)*exp2(21-v1*13-1/v1-i1/3-j1/3);

  }
  if (0<u.a)
    col = (ivec3(a3)>>coord.x%2*8)%256/255.;
}
