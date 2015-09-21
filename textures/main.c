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

GLuint cont_tex;

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
  glClear (GL_COLOR_BUFFER_BIT);

  glUseProgram (shader_prog[0]);
  glBindTexture (GL_TEXTURE_2D, cont_tex);
  /* draw rectangle using shader 0 */
  glBindVertexArray (rectangle_vao);

  if (((int) glfwGetTime () % 5) == 0) {
    /* every five seconds, simulate a light */
    light = 1.1f;
  }
  glUniform1f (glGetUniformLocation (shader_prog[0], "light"), light);

  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

  for (i = 0; i < 2; i++) {
    if (vertex_shader[i] && frag_shader[i]) {
      shader_prog[i] = shader_program_create (vertex_shader[i], frag_shader[i]);
      if (!shader_prog[i])
        goto end;
    }
  }


  rectangle_init ();
  load_image_to_texture ("container.jpg", &cont_tex);

  while (!glfwWindowShouldClose (win)) {
    glfwPollEvents ();

    render (shader_prog);

    glfwSwapBuffers (win);
  }

  ret = 0;

  rectangle_deinit ();

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
