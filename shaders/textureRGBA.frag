precision mediump float;
uniform sampler2D tex;
uniform float alpha;
varying vec2 v_texcoord;

void main() {
    vec4 tex_c = texture2D(tex, v_texcoord);
    gl_FragColor = vec4(tex_c.rgba);
}
