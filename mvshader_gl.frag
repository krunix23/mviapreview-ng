uniform sampler2D s_baseMap;
uniform highp float texture_width;
uniform highp float texel_width;
varying vec2 v_texCoord;
uniform int isYUV;

// OpenGL
void main()
{
  float red, green, blue;
  vec4 luma_chroma;
  float luma, chroma_u,  chroma_v;
  float pixelx, pixely;
  float xcoord;

  pixelx = v_texCoord.s;
  pixely = v_texCoord.t;

  xcoord = floor (pixelx * texture_width);

  luma_chroma = texture2D(s_baseMap, vec2(pixelx, pixely));

  if( isYUV != 0 )
  {
    luma = (luma_chroma.r - 0.0625) * 1.1643;

    if (0.0 == mod(xcoord , 2.0)) // even
    {
      chroma_u = luma_chroma.a;
      chroma_v = texture2D(s_baseMap, 
      vec2(pixelx + texel_width, pixely)).a;
    }
    else // odd
    {
      chroma_v = luma_chroma.a;
      chroma_u = texture2D(s_baseMap, 
      vec2(pixelx - texel_width, pixely)).a;     
    }
    chroma_u = chroma_u - 0.5;
    chroma_v = chroma_v - 0.5;

    red = luma + 1.5958 * chroma_v;
    green = luma - 0.39173 * chroma_u - 0.81290 * chroma_v;
    blue = luma + 2.017 * chroma_u;
  }
  else
  {
    red = luma_chroma.b;
    green = luma_chroma.g;
    blue = luma_chroma.r;
  }

  gl_FragColor = vec4(red, green, blue, 1.0);
}
