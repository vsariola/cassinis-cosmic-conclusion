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

#version 330

// ----------------------------------------------------------------------------
// Required prelude

// The result of the shader
out vec3 col;
uniform ivec3 u;

void main() {
  float i1,t1,r1,a1,s1,m1,k1,j1 = mod(i1 = u.z/18e5,2);
  vec3 f3,g3,c3=vec3(gl_FragCoord.xy-u.xy/2,2*u.y),a3=vec3(0,-1,9),d3=vec3(0,1+j1/5,-2);

    c3.zy+=c3.yz*vec2(1,-1)/9;

    if (i1>2)
        a3 = vec3(3, 2, -3),
        d3 = vec3(0, .2, -2)*(1+j1),
        c3.xy+=c3.yx*vec2(1,-1)/3;

    if (i1>4)
        a3  = vec3(2, 1, 9),
        d3 = vec3(2, 1+j1/5, -6);

    if (i1>6)
        a3  = vec3(-j1,0,6),
        d3 = vec3(0,1,-6);

      c3 /= sqrt(dot(c3,c3));
      a3 /= sqrt(dot(a3,a3));

      r1 = -d3.y/c3.y;

      col  = d3+c3*r1;

      t1 = -dot(col, a3);
      t1 -= sqrt(a1=t1*t1-dot(col, col)+1);

      m1 = t1*a1;

      t1 = -dot(d3, c3);
      t1 -= sqrt(a1=t1*t1-dot(d3, d3)+1);

      s1 = tanh(55*a1);
      a1 = sqrt(dot(col,col));

      d3 += t1*c3;

      k1=.3;
      for (i1 = 0;9>++i1;k1 /= 2)
        f3 += k1*sin(a3/4+2/k1*d3.y)+k1,
        g3 += k1*sin(a3/4+4/k1*a1);

      col = vec3(5, 2, 1)/(3333-3332*dot(a3, c3));

      if (t1>0)
        col = mix(col,
            max(0,dot(a3, d3))*f3,
            s1);

      col = sqrt(tanh(
        mix(col,
            g3/cosh(9*max(0, m1)),
            g3/cosh(9*(2-a1))*(1-s1*step(t1,r1))*3
        )*j1*(2-j1)
      ));


  if (-u.z>0)
  for(i1=0;4>++i1;)
  for(j1=0;5>++j1;)
    t1 = (ivec3(gl_FragCoord).x+ivec3(gl_FragCoord).y*512+u.z+1)/2/48e3+350,
    s1 = mod(r1 = min(t1*j1/32+i1/3,9),1),
    col = (ivec3(a3 += sin(
        sin(t1/j1/48)*(
            sin(t1*j1*i1*exp2(mod(r1-s1,3)/6+8))*4+sin(t1*3)+sin(t1*11)
        )
    )*exp2(20-13*s1-1/s1))>>ivec3(gl_FragCoord).x%2*8)%256/255.;

}
