#if (logo_type = 0)
#declare box_width = 3.0;
#declare box_height = 3.0;
#else
#declare box_width = 5.5;
#declare box_height = 1.0;
#end

#declare letters =
text
{
#if (logo_type = 0)
  ttf "Dragonwick.ttf" "RTB"
#else
  ttf "Dragonwick.ttf" "RealTimeBattle"
#end
  0.1 , 0
  rotate y*180
  translate <box_width - 0.3,-0.3,0.1>
  scale <1,2,1>
  bounded_by { box { <-box_width, -box_height, 0.0>, <box_width, box_height, 0.2> } }
}

#declare rtblogo =
union
{
  box
  {
    <-box_width, -box_height, -0.2>, <box_width, box_height, 0.0>
    texture { 
      pigment { colour rgb <0.234, 0.6, 0.9> }
      finish
      {
        ambient 0.6
        reflection <0.02, 0.02, 0.02>
        diffuse 0.5
        roughness 0.8
      }
      normal
      {
        bumps
        scale < 1,1,1 >
      }
    }
  }
  object
  {
    letters
    texture { 
      pigment { colour rgb <0.8, 0.6, 0.3> }
      finish
      {
        ambient 0.6
        specular 0.3
        roughness 0.1
      }
      normal
      {
        bumps
        scale <0.5,0.5,0.025>
      }
    }
  }
}