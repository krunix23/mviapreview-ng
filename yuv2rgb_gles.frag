precision highp float; 
precision highp sampler2D; 
precision highp int; 
uniform sampler2D s_baseMap;
uniform float texture_width;
uniform float texel_width;
varying vec2 v_texCoord;

// OpenGLES
// BLX source (YUV)
void main()
{
  float red, green, blue;
  vec4 luma_chroma;
  float luma, chroma_u,  chroma_v;
  float pixelx, pixely;
  float xcoord, ycoord;
  vec3 yuv;
  float res;

  pixelx = v_texCoord.s; 
  pixely = v_texCoord.t; 

  xcoord = floor(pixelx * texture_width); 

  luma_chroma = texture2D(s_baseMap, vec2(pixelx, pixely));

  luma = (luma_chroma.g - 0.0625) * 1.1643; 

  if( 0.0 == mod(xcoord, 2.0) ) // even
  {
    chroma_u = luma_chroma.r; 
    chroma_v = texture2D(s_baseMap, 
    vec2(pixelx - texel_width, pixely)).r; 
  }
  else // odd
  {
    chroma_v = luma_chroma.r; 
    chroma_u = texture2D(s_baseMap, 
    vec2(pixelx + texel_width, pixely)).r; 
  }
  chroma_u = chroma_u - 0.5; 
  chroma_v = chroma_v - 0.5; 

  red = luma + 1.5958 * chroma_v; 
  green = luma - 0.39173 * chroma_u - 0.81290 * chroma_v; 
  blue = luma + 2.017 * chroma_u; 

  // set the color based on the texture color
  gl_FragColor = vec4(red, green, blue, 0.0);
}
