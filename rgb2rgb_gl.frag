uniform sampler2D s_baseMap;
uniform highp float texture_width;
uniform highp float texel_width;
varying vec2 v_texCoord;

// OpenGL
// RGB source (e.g. BlueFOX) on X86
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
