varying highp vec2 f_uvs;
uniform sampler2D texture;

void main() {
  gl_FragColor = texture2D(texture, f_uvs);
}
