#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <SOIL/SOIL.h>

/* 0: vertex buffer object
 * 1: element buffer object
 */
GLuint rectangle_bo[2];
GLuint rectangle_vao;

/* 0: vertex buffer object
 * 1: element buffer object
 */
GLuint cube_bo[2];
GLuint cube_vao;

GLuint cont_tex;
GLuint face_tex;

GLfloat my_model_matrix[4][4];

GLfloat my_view_matrix[4][4] = {
  { 1, 0, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, -3.0f, 1 },
};

GLfloat my_projection_matrix[4][4];

static void
build_model_matrix_rot_x (GLfloat mat[4][4], GLfloat degrees, GLfloat scale[3],
    GLfloat translation[3])
{
  GLfloat rad;

  rad = (degrees * M_PI) / 180.0;

  mat[0][0] = scale[0];
  mat[0][1] = 0;
  mat[0][2] = 0;
  mat[0][3] = 0;

  mat[1][0] = 0;
  mat[1][1] = scale[1] * cos (rad);
  mat[1][2] = scale[2] * sin (rad);
  mat[1][3] = 0;

  mat[2][0] = 0;
  mat[2][1] = -(scale[1] * sin (rad));
  mat[2][2] = scale[2] * cos (rad);
  mat[2][3] = 0;

  mat[3][0] = translation[0];
  mat[3][1] = translation[1];
  mat[3][2] = translation[2];
  mat[3][3] = 1;
}

static void
build_model_matrix_rot (GLfloat mat[4][4], GLfloat degrees, GLfloat rot_axis[3])
{
  GLfloat rad;
  GLfloat a;

  rad = (degrees * M_PI) / 180.0;

  a = (1 - cos (rad));

  mat[0][0] = cos (rad) + rot_axis[0] * rot_axis[0] * a;
  mat[0][1] = rot_axis[0] * rot_axis[1] * a + rot_axis[2] * sin (rad);
  mat[0][2] = rot_axis[0] * rot_axis[2] * a - rot_axis[1] * sin (rad);
  mat[0][3] = 0;

  mat[1][0] = rot_axis[0] * rot_axis[1] * a - rot_axis[2] * sin (rad);
  mat[1][1] = cos (rad) + rot_axis[1] * rot_axis[1] * a;
  mat[1][2] = rot_axis[1] * rot_axis[2] * a + rot_axis[0] * sin (rad);
  mat[1][3] = 0;

  mat[2][0] = rot_axis[0] * rot_axis[2] * a + rot_axis[1] * sin (rad);
  mat[2][1] = rot_axis[1] * rot_axis[2] * a - rot_axis[0] * sin (rad);
  mat[2][2] = cos (rad) + rot_axis[2] * rot_axis[2] * a;
  mat[2][3] = 0;

  mat[3][0] = 0;
  mat[3][1] = 0;
  mat[3][2] = 0;
  mat[3][3] = 1;
}

static void
build_projection_matrix (GLfloat mat[4][4], GLfloat fov, GLfloat fov_ratio,
    GLfloat near, GLfloat far)
{
  GLfloat fov_rad = (fov * M_PI) / 180.0;

  mat[0][0] = cos (0.5 * fov_rad) / sin (0.5 * fov_rad);
  mat[0][1] = 0;
  mat[0][2] = 0;
  mat[0][3] = 0;

  mat[1][0] = 0;
  mat[1][1] = mat[0][0] * fov_ratio;
  mat[1][2] = 0;
  mat[1][3] = 0;

  mat[2][0] = 0;
  mat[2][1] = 0;
  mat[2][2] = -(far + near) / (far - near);
  mat[2][3] = -1;

  mat[3][0] = 0;
  mat[3][1] = 0;
  mat[3][2] = -(2 * far * near) / (far - near);
  mat[3][3] = 0;
}

static void
matrix_4x4_display (GLfloat mat[4][4])
{
  int i;

  printf ("+-                               -+\n");

  for (i = 0; i < 4; i++)
    printf ("| %+.4f %+.4f %+.4f %+.4f |\n", mat[0][i], mat[1][i], mat[2][i],
        mat[3][i]);

  printf ("+-                               -+\n");
}

static void
rectangle_init (void)
{
  GLfloat vertices[] = {
    /* positions */      /* colors */       /* tex coord */
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,    /* bottom left */
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,    /* top left */
     0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,    /* top right */
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,    /* bottom right */
  };

  GLuint indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  glGenBuffers (2, rectangle_bo);
  glGenVertexArrays (1, &rectangle_vao);

  glBindVertexArray (rectangle_vao);

  /* copy our vertices and our indices to buffer object */
  glBindBuffer (GL_ARRAY_BUFFER, rectangle_bo[0]);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, rectangle_bo[1]);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
      GL_STATIC_DRAW);

  /* set the layout of the vertex data in memory */
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat),
      (GLvoid *) (3 * sizeof (GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat),
      (GLvoid *) (6 * sizeof (GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray (0);
}

static void
rectangle_deinit (void)
{
  glDeleteVertexArrays (1, &rectangle_vao);
  glDeleteBuffers (2, rectangle_bo);
}

static void
cube_init (void)
{
  GLfloat vertices[] = {
    /* positions */      /* tex coord */

    /* front face */
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,     /* bottom left */
    -0.5f,  0.5f, 0.5f,  0.0f, 1.0f,     /* top left */
     0.5f,  0.5f, 0.5f,  1.0f, 1.0f,     /* top right */
     0.5f, -0.5f, 0.5f,  1.0f, 0.0f,     /* bottom right */

    /* right face */
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     /* bottom front */
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f,     /* top front */
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,     /* bottom back */
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     /* top back */

    /* back face */
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    /* bottom left */
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    /* bottom right */
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    /* top right */
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    /* top left */

    /* left face */
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    /* bottom front */
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    /* top front */
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    /* bottom back */
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    /* top back */

    /* top face */
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,    /* front right */
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,    /* front left */
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    /* back right */
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    /* back left */

    /* bottom face */
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    /* front left */
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    /* front right */
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,    /* back left */
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,    /* back right */
  };

  GLuint indices[] = {
    /* front face */
    0, 1, 2,
    0, 2, 3,

    /* right face */
    4, 5, 6,
    5, 6, 7,

    /* back face */
    8, 9, 10,
    8, 10, 11,

    /* left face */
    12, 13, 14,
    13, 14, 15,

    /* top face */
    16, 17, 18,
    17, 18, 19,

    /* bottom face */
    20, 21, 22,
    21, 22, 23,
  };

  glGenBuffers (2, cube_bo);
  glGenVertexArrays (1, &cube_vao);

  glBindVertexArray (cube_vao);

  /* copy our vertices and our indices to buffer object */
  glBindBuffer (GL_ARRAY_BUFFER, cube_bo[0]);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, cube_bo[1]);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
      GL_STATIC_DRAW);

  /* set the layout of the vertex data in memory */
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), NULL);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray (0);
}

static void
cube_deinit (void)
{
  glDeleteVertexArrays (1, &cube_vao);
  glDeleteBuffers (2, cube_bo);
}

static int
load_shader_from_file (const char * path, GLchar ** shader)
{
  FILE *f;
  struct stat st;
  GLchar *buf;
  size_t rbytes;

  if (stat (path, &st) < 0)
    return -1;

  f = fopen (path, "r");
  if (f == NULL)
    return -1;

  buf = malloc (st.st_size + 1);
  if (buf == NULL) {
    fprintf (stderr, "failed to allocated shader buffer");
    return -1;
  }

  rbytes = fread (buf, 1, st.st_size, f);
  if (rbytes < (size_t) st.st_size) {
    fprintf (stderr, "short read: %zu/%zd", rbytes, st.st_size);
    free (buf);
    return -1;
  }

  buf[st.st_size] = '\0';

  *shader = buf;
  return 0;
}

static GLuint
shader_init (const char * path, GLenum shader_type)
{
  GLchar *shader = NULL;
  GLint success;
  GLuint shader_id = 0;

  printf ("creating shader of type %d from file %s\n", shader_type, path);

  if (load_shader_from_file (path, &shader) < 0) {
    fprintf (stderr, "failed to load shader %s\n", path);
    goto beach;
  }

  shader_id = glCreateShader (shader_type);
  if (!shader_id) {
    fprintf (stderr, "failed to create shader\n");
    goto beach;
  }

  glShaderSource (shader_id, 1, (const GLchar **) &shader, NULL);
  glCompileShader (shader_id);
  glGetShaderiv (shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog (shader_id, 512, NULL, info_log);
    fprintf (stderr, "failed to compile shader: %s\n", info_log);
    goto beach;
  }

  free (shader);

  return shader_id;

beach:
  if (shader)
    free (shader);

  if (shader_id)
    glDeleteShader (shader_id);

  return 0;
}

static GLuint
shader_program_create (GLuint vertex_shader_id, GLuint fragment_shader_id)
{
  GLint success;
  GLuint prog_id;

  prog_id = glCreateProgram ();
  if (!prog_id) {
    fprintf (stderr, "failed to create shader program\n");
    return 0;
  }

  glAttachShader (prog_id, vertex_shader_id);
  glAttachShader (prog_id, fragment_shader_id);
  glLinkProgram (prog_id);

  glGetProgramiv (prog_id, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog (prog_id, 512, NULL, info_log);
    fprintf (stderr, "failed to link program: %s", info_log);
    glDeleteProgram (prog_id);
    return 0;
  }

  return prog_id;
}

static int
load_image_to_texture (const char * path, GLuint * texture)
{
  int width;
  int height;
  unsigned char *img;

  img = SOIL_load_image (path, &width, &height, 0, SOIL_LOAD_RGB);
  if (img == NULL)
    return -1;

  glGenTextures (1, texture);
  glBindTexture (GL_TEXTURE_2D, *texture);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
      GL_UNSIGNED_BYTE, img);
  glGenerateMipmap (GL_TEXTURE_2D);

  SOIL_free_image_data (img);
  glBindTexture (GL_TEXTURE_2D, 0);

  return 0;
}

static void
render (GLuint shader_prog[2])
{
  GLfloat light = 1.0f;

  /* clear screen */
  glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram (shader_prog[0]);

  /* bind container texture to the first one and link it with the good
   * uniform variable */
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, cont_tex);
  glUniform1i (glGetUniformLocation (shader_prog[0], "tex1"), 0);

  /* bind face texture to the second one and link with the good uniform var */
  glActiveTexture (GL_TEXTURE1);
  glBindTexture (GL_TEXTURE_2D, face_tex);
  glUniform1i (glGetUniformLocation (shader_prog[0], "tex2"), 1);

  /* draw rectangle using shader 0 */
  glBindVertexArray (rectangle_vao);

  glUniformMatrix4fv (glGetUniformLocation (shader_prog[0], "model"), 1,
        GL_FALSE, *my_model_matrix);
  glUniformMatrix4fv (glGetUniformLocation (shader_prog[0], "view"), 1,
        GL_FALSE, *my_view_matrix);
  glUniformMatrix4fv (glGetUniformLocation (shader_prog[0], "projection"), 1,
        GL_FALSE, *my_projection_matrix);

  if (((int) glfwGetTime () % 5) == 0) {
    /* every five seconds, simulate a light */
    light = 1.1f;
  }
  glUniform1f (glGetUniformLocation (shader_prog[0], "light"), light);

  /* texture 2 needs to be swapped in y */
  glUniform1ui (glGetUniformLocation (shader_prog[0], "tex_coord_swap_y[1]"), 1);

  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glUseProgram (shader_prog[1]);
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, cont_tex);
  glUniform1i (glGetUniformLocation (shader_prog[1], "tex1"), 0);

  glActiveTexture (GL_TEXTURE1);
  glBindTexture (GL_TEXTURE_2D, face_tex);
  glUniform1i (glGetUniformLocation (shader_prog[1], "tex2"), 1);

  glUniform1f (glGetUniformLocation (shader_prog[1], "light"), 1.0f);

  {
    GLfloat model[4][4];
    GLfloat axis[3] = { 1.0f, -0.5f, 0.0f };

    build_model_matrix_rot (model, glfwGetTime () * 20.0, axis);
    glUniformMatrix4fv (glGetUniformLocation (shader_prog[1], "model"), 1,
          GL_FALSE, *model);
  }

  glUniformMatrix4fv (glGetUniformLocation (shader_prog[1], "view"), 1,
        GL_FALSE, *my_view_matrix);
  glUniformMatrix4fv (glGetUniformLocation (shader_prog[1], "projection"), 1,
        GL_FALSE, *my_projection_matrix);

  /* texture 2 needs to be swapped in y */
  glUniform1ui (glGetUniformLocation (shader_prog[1], "tex_coord_swap_y[1]"), 1);

  glBindVertexArray (cube_vao);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements (GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray (0);
}

static void
win_key_callback (GLFWwindow * win, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose (win, GL_TRUE);
}

int
main (int argc, char *argv[])
{
  int ret = 1;
  GLFWwindow *win;
  GLuint vertex_shader[2] = { 0, };
  GLuint frag_shader[2] = {0, };
  GLuint shader_prog[2] = { 0, };
  int i;

  glfwInit ();
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);

  /* create window */
  win = glfwCreateWindow (800, 600, "OpenGL test", NULL, NULL);
  if (win == NULL) {
    fprintf (stderr, "failed to create window\n");
    glfwTerminate ();
    return 1;
  }

  glfwMakeContextCurrent (win);
  glfwSetKeyCallback (win, win_key_callback);

  glewExperimental = GL_TRUE;
  if (glewInit () != GLEW_OK) {
    fprintf (stderr, "failed to init glew\n");
    return 1;
  }

  /* set the rendering window position and dimension */
  glViewport (0, 0, 800, 600);

  vertex_shader[0] = shader_init ("simple_vertex_shader.vert", GL_VERTEX_SHADER);
  if (!vertex_shader[0])
    goto end;

  frag_shader[0] = shader_init ("simple_fragment_shader.frag",
      GL_FRAGMENT_SHADER);
  if (!frag_shader[0])
    goto end;

  vertex_shader[1] = shader_init ("cube.vert", GL_VERTEX_SHADER);
  if (!vertex_shader[1])
    goto end;

  frag_shader[1] = shader_init ("simple_fragment_shader.frag",
      GL_FRAGMENT_SHADER);
  if (!frag_shader[1])
    goto end;

  for (i = 0; i < 2; i++) {
    if (vertex_shader[i] && frag_shader[i]) {
      shader_prog[i] = shader_program_create (vertex_shader[i], frag_shader[i]);
      if (!shader_prog[i])
        goto end;
    }
  }


  rectangle_init ();
  cube_init ();
  load_image_to_texture ("container.jpg", &cont_tex);
  load_image_to_texture ("awesomeface.png", &face_tex);

  /* to make the plane laying on the floor, we rotate around the x-axis a little
   * bit */
  {
    GLfloat scaling[3] = { 1.0f, 1.0f, 1.0f };

    /* make the 'floor' at our foot */
    GLfloat translation[3] = { 0.0f, -1.0f, 0.0f };

    build_model_matrix_rot_x (my_model_matrix, -55.0f, scaling, translation);
  }

  build_projection_matrix (my_projection_matrix, 45.0f, 800 / 600, 0.1f,
      100.0f);

  printf ("model matrix is:\n");
  matrix_4x4_display (my_model_matrix);

  printf ("view matrix is\n");
  matrix_4x4_display (my_view_matrix);

  printf ("projection matrix is\n");
  matrix_4x4_display (my_projection_matrix);

  glEnable (GL_DEPTH_TEST);

  while (!glfwWindowShouldClose (win)) {
    glfwPollEvents ();

    render (shader_prog);

    glfwSwapBuffers (win);
  }

  ret = 0;

  rectangle_deinit ();
  cube_deinit ();

end:

  for (i = 0; i < 2; i++) {
    if (vertex_shader[i])
      glDeleteShader (vertex_shader[i]);

    if (frag_shader[i])
      glDeleteShader (frag_shader[i]);

    if (shader_prog[i])
      glDeleteProgram (shader_prog[i]);
  }


  glfwTerminate ();

  return ret;
}

