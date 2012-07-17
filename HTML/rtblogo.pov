#version 3.1;

#include "colors.inc"
#include "skies.inc"

#declare logo_type = 2;  // 0 - small RTB-logo 40x40
                        // 1 - small RealTimeBattle-logo 145x110
                        // 2 - large RealTimeBattle-logo 545x410

#include "rtblogo.inc"

camera
{
#if (logo_type = 0)
  location <0, 0, 5>     // used for small logo
#end
#if (logo_type = 1)
  location <0, 0, 10>     // used for small logo
#end
#if (logo_type = 2)
  location <0, -3, 10>  // used for large logo
#end
  look_at <0, 0.0, 0.0>
  angle 60
}

//sky_sphere { S_Cloud2 }

#if (logo_type = 2)
background { colour rgb <0.980392156863, 0.941176470588, 0.901960784314>  }
#else
background { colour rgb <0.933333333333, 0.866666666667, 0.6> }
#end

light_source
{
  <5, 10, 6>
  colour 1.6
  area_light
  <0, 6, -10>*0.3333
  <-13.6, 5, 3>*0.25
  3, 3
}

object 
{ 
  rtblogo 
#if (logo_type = 2)
#else
  scale <1.0,1.2,1.0>    // for small logos only
#end
}