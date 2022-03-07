precision highp float;
uniform sampler2D tex;
uniform sampler2D bgtex;
uniform float alpha;
uniform vec3 texsize;
uniform vec3 texpos;
uniform vec3 bgsize;

varying vec2 v_texcoord;

void main() {

    vec2 bgpos = vec2(
            texpos.x / bgsize.x / 2.0 + texsize.x / 2.0 / bgsize.x * v_texcoord.x,
            1.0 - (texpos.y / bgsize.y / 2.0 + texsize.y / 2.0 / bgsize.y * v_texcoord.y));
    vec4 c = texture2D(tex, v_texcoord);
    vec4 bg = texture2D(bgtex, vec2(bgpos.x, 1.0 - bgpos.y));
    vec4 glass = vec4(bg.r * c.r, bg.g * c.g, bg.b * c.b, c.a);

    gl_FragColor = mix(c, glass, 0.4);
}


