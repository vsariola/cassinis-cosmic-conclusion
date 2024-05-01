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
uniform ivec3 u;

void main() {
  float i1,j1,t1,r1,a1,s1,m1;
  vec3 a3,c3,d3;

  ivec3 coord = ivec3(gl_FragCoord);

  i1  = u.z/36e5;
  j1  = mod(i1,2);
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

  // d3 = ro
  // c3 = rd
  c3  = normalize(c3);
  // a3 = sd
  a3  = normalize(a3);

  // i1 = prod
  i1  = dot(d3, c3);
  // j1 = pdet 
  j1  = -dot(d3, d3)+i1*i1+1;
  // i1 = pi
  i1  = -i1-sqrt(j1);

  // t1 = pt
  t1  = max(tanh(1E3*j1/i1), 0);

  // j1 = col
  j1  = smoothstep(0., 1E-2, dot(d3+c3*i1, a3))*t1;

  // r1 = ri
  r1 = -d3.y/c3.y;
  // t1 = rt
  a1  = (r1 < i1) ? 1:1.-t1;
  // a3 = rp
  a3 = d3+c3*r1;

  // i1 = rdi
  i1 = abs(abs(length(a3)-2)-.2)-.18;
  // t1 = raa
  t1 = abs(dot(normalize(a3), c3))*length(fwidth(a3)); 
  // j1 = col
  col += mix(j1, smoothstep(t1, -t1, i1), smoothstep(t1, -t1, i1-.05)*a1);
  

  for(i1=1;i1<4;i1++)
  for(j1=1;j1<5;j1++) {
    t1 = (coord.x+coord.y*512+u.z+1)/4/48E3+175;
    r1 = min(t1*j1/32+i1/3,9);
    s1 = mod(r1,1);    
    m1 = 4*sin(exp2(mod(r1-s1,3)/6+8)*t1*j1*i1);
    r1 = t1+(coord.x&2);
    for (a1=3;a1<50;r1 += a1 *= 1.02)
        m1 += sin(r1*a1)/a1;
    a3 += sin(sin(t1/j1/47)*m1)*exp2(21-13*s1-1/s1-i1/3-j1/3);
  }
  if (0>u.z)
    col = (ivec3(a3)>>coord.x%2*8)%256/255.;
}
