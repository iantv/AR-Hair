attribute vec4 vertex;
attribute vec2 v_uvs;
varying vec2 f_uvs;
uniform mat4 mvp;

void main() {
  gl_Position = mvp*vertex;
  f_uvs = v_uvs;
}
