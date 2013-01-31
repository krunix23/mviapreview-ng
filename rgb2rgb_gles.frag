precision highp float; 
precision highp sampler2D;
uniform sampler2D s_baseMap;
uniform float texture_width;
uniform float texel_width;
varying vec2 v_texCoord;

// OpenGLES
// RGB source (e.g. VirtualDevice)
void main()
{
  float red, green, blue;
  vec4 luma_chroma;

  float pixelx, pixely;

  pixelx = v_texCoord.s;
  pixely = v_texCoord.t;

  luma_chroma = texture2D(s_baseMap, vec2(pixelx, pixely));

  red = luma_chroma.b;
  green = luma_chroma.g;
  blue = luma_chroma.r;
  gl_FragColor = vec4(red, green, blue, 1.0);
}
