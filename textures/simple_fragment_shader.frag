#version 330 core

/* import color defined in vertex shader by declaring it as input */
in vec4 vert_color;

in vec2 tex_coordo;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform float light;
uniform bool tex_coord_swap_y[2];

/* The fragment shader shall set a color */
out vec4 color;

void main ()
{
  vec4 color1;
  vec4 color2;

  if (tex_coord_swap_y[0])
    color1 = texture (tex1, vec2 (tex_coordo.x, 1.0 - tex_coordo.y));
  else
    color1 = texture (tex1, tex_coordo);

  if (tex_coord_swap_y[1])
    color2 = texture (tex2, vec2 (tex_coordo.x, 1.0 - tex_coordo.y));
  else
    color2 = texture (tex2, tex_coordo);

  /* bend the two and multiply by the light */
  color = mix (color1, color2, 0.2f) * vec4 (light, light, light, 0.3f);
}
