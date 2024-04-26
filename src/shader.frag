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

vec2 rayPlanet(vec3 ro, vec3 rd) {  
  float 
    b = dot(ro, rd)
  , h = b*b - dot(ro, ro) + 1E4;
  return vec2(-b-sqrt(h), h);
}

vec3 skyCol(vec3 sunDir, vec3 rd) {
  float gf = 1.0001-dot(sunDir, rd);
  return vec3(5, 2, 1)/5E4/gf/gf;
}

void main() {   
  ivec3 coord = ivec3(gl_FragCoord);  

  for(float i=1;i<4;i++)
  for(float j=1;j<5;j++) {        
    float t = (coord.x+coord.y*512+u.a)/4/48000.,
          r = t*j/32+i/3,
          v = mod(r,1),
          a = 3,
          s = t+(coord.x&2)*t/50,
          m = 4*sin(exp2(mod(r-v,3)/6+8.5)*t*j*i);
        for (;a<50;s += a *= 1.02)
            m += sin(s*a)/a;
       col += sin(sin(r<9?t/j/47:0)*m)*exp2(21-v*13-1/v-i/3-j/3);
                
    }
  col = (ivec3(col)>>coord.x%2*8)%256/255.;  
  if (u.a<0) {
        float 
        tp   = u.z/8e5
      , tm   = mod(tp,2)
      , tn   = tp-tm      
      ;

      vec3 
        sunDir            
      , rd    = vec3(2*coord.xy-u.xy, 4*u.y)
      , ro = tn == 0 ? ((sunDir  = vec3(0, -1, 33)),(rd.zy+=rd.yz*vec2(1,-1)*(-.05+.08*tm)),vec3(0, 90+15*tm, -180)) :
           tn == 2 ? ((sunDir  = vec3(3, 2, -3)),(rd.xy+=rd.yx*vec2(.3,-.3)),vec3(0, 25, -250)*(1+tm*tm)) :
           tn == 4 ? ((sunDir  = vec3(0, 1, 3)),vec3(180, 50+tm*50, -600)) :
                     ((sunDir  = vec3(-3, 2, 6)),vec3(0, 0, -600)*(1+tm*tm));      

      rd  = normalize(rd);
      sunDir = normalize(sunDir);

      float ri = -ro.y/rd.y;
      
      col = skyCol(sunDir, rd);

      vec3 rp  = ro+rd*ri;

      vec2  
        si = rayPlanet(ro, rd)
      , pi = rayPlanet(rp, sunDir)
      ;
  
      vec3 
        p = ro+si.x*rd
      , n = normalize(p)
      , pcol
      , rcol
      ;

      float 
        rri = -p.y/sunDir.y
      , fre = 1+dot(rd, n)
      , rr  = length(rp.xz)
      , rrr = length((p+rri*sunDir).xz)
      , bf  = tanh(5E-3*si.y)
      , rrf  = 1-smoothstep(40., 19., abs(rrr-200))*(.5+.4*sin(.3*rrr)*cos(.05*rrr))*tanh(100*sunDir.y*sunDir.y)
      , rbf = pi.x > 0 ? smoothstep(2E3, 0., pi.y)+3E-2  : 1
      ;
      vec2 
        a2 = vec2(.25)
      , h2 = vec2(p.y*5E-2,rr*.15+1)
      ; 

      for (float i = 1; i < 8; i++) {
        pcol += a2.x*(sin(h2.x+vec3(0, 1, 2)/4)+1);
        rcol += a2.y*(sin(h2.y+vec3(2, 1, 0)/8));
        a2 *= .5;
        a2.y *= smoothstep(1., .6, (1+rd.y)*sqrt(abs(dot(normalize(rp.xz), normalize(rd.xz))))*tanh(ri*4E-3));
        h2 = 1.98*h2+1.34;
      }
      rcol = abs(rcol);

      fre *= fre*fre;

      if (si.x > 0) {
        col *= 1-bf;
        col += sqrt(skyCol(sunDir, reflect(rd, n)))*fre+max(dot(sunDir, n), 0)*bf*(pcol+vec3(.4, .9, 2)*fre*fre);
        if (rri > 0)
          col *= rrf;
      }  
       
      col = sqrt(tanh(
        mix(col,
            rcol*rbf,
            smoothstep(40, 19, abs(rr-200))*(si.x < ri?1-bf:1)*dot(rcol, vec3(1))
        )*tm*(2-tm)
      ));
  }  
}
