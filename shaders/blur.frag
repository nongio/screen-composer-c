precision mediump float;
uniform sampler2D tex;
uniform float alpha;
uniform vec3 texsize;
varying vec2 v_texcoord;

//https://www.shadertoy.com/view/XdfGDH
//
float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}


void main() {
	vec3 c = texture2D(tex, v_texcoord.xy).rgb;

    const int mSize = 19;
    const int kSize = (mSize-1)/2;
    float kernel[mSize];
    vec3 final_colour = vec3(0.0);

    //create the 1-D kernel
    float sigma = 7.0;
    float Z = 0.0;
    for (int j = 0; j <= kSize; ++j)
    {
        kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
    }

    //get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j)
    {
        Z += kernel[j];
    }

    //read out the texels
    for (int i=-kSize; i <= kSize; ++i)
    {
        for (int j=-kSize; j <= kSize; ++j)
        {
            vec2 shift = vec2(float(i)/texsize.x, float(j)/texsize.y);
            vec3 cc= texture2D(tex, v_texcoord.xy + shift).rgb;
            final_colour += kernel[kSize+j]*kernel[kSize+i]*cc;

        }
    }


    gl_FragColor = vec4(final_colour/(Z*Z), 1.0);
}
