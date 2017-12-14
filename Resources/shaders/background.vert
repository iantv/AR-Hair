attribute vec4 vertex;
attribute vec2 v_uvs;
varying vec2 f_uvs;

void main() {
  gl_Position = vertex;
  f_uvs = v_uvs;
}