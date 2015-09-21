#version 330 core

/* import color defined in vertex shader by declaring it as input */
in vec4 vert_color;

in vec2 tex_coordo;

uniform sampler2D tex;
uniform float light;

/* The fragment shader shall set a color */
out vec4 color;

void main ()
{
  color = texture (tex, tex_coordo) * vec4 (light, light, light, 0.3f);
}
