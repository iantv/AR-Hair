attribute vec4 vertex;
attribute vec2 v_uvs;
varying vec2 f_uvs;

void main() {
  gl_Position = vertex;
  f_uvs = v_uvs;
<<<<<<< HEAD:Resources/shaders/background.vert
}
=======
}
>>>>>>> 7373082b9e877efce60dbc024d317fb57aef53dc:Resource/shaders/background.vert
