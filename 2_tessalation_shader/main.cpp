#include<GL/glew.h>
#include<GL/glut.h>
#include<GL/GL.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<istream>
#include<string>
#include<vector>
#include <memory>
#include <io.h>

using std::string;
using std::ifstream;
using std::ostringstream;
using std::ios;
using std::vector;
using std::shared_ptr;

GLuint program;
GLuint vao;

std::shared_ptr<char> readShaderFile(std::string strSource) {
  std::ifstream myfile(strSource);
  if (myfile.fail()) {
    std::cout << "Can not open it " << std::endl;
    // throw new std::runtime_error("IO stream corrupted");
  }
  std::string shaderStr((std::istreambuf_iterator<char>(myfile)),
                        std::istreambuf_iterator<char>());
  myfile.close();
  size_t len = shaderStr.length();
  std::shared_ptr<char> shaderPtr(new char[len + 1]);
  strcpy_s(shaderPtr.get(), len + 1, shaderStr.c_str());
  std::cout << shaderStr << std::endl;
  return shaderPtr;
} 


void startup() {
  program = glCreateProgram();
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  
  shared_ptr<char> vs_string = readShaderFile("shader/vs.txt");
  char* vs_string_ptr = vs_string.get();
  glShaderSource(vs, 1, &vs_string_ptr, NULL);
  glCompileShader(vs);

  // Tesselation Control Shader
  GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
  shared_ptr<char> tcs_string = readShaderFile("shader/tcs.txt");
  char *tcs_string_ptr = tcs_string.get();
  glShaderSource(tcs, 1, &tcs_string_ptr, NULL);
  glCompileShader(tcs);

  // Tesselation Evaluation Shader
  GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
  shared_ptr<char> tes_string = readShaderFile("shader/tes.txt");
  char *tes_string_ptr = tes_string.get();
  glShaderSource(tes, 1, &tes_string_ptr, NULL);
  glCompileShader(tes);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  shared_ptr<char> fs_string = readShaderFile("shader/fs.txt");
  char *fs_string_ptr = fs_string.get();
  glShaderSource(fs, 1, &fs_string_ptr, NULL);
  glCompileShader(fs);

  glAttachShader(program, vs);
  glAttachShader(program, tcs);
  glAttachShader(program, tes);
  glAttachShader(program, fs);

  glLinkProgram(program);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void render() {
  static const GLfloat green[] = {0.0f, 0.25f, 0.0f, 1.0f};
  glClearBufferfv(GL_COLOR, 0, green);

  glUseProgram(program);
  glDrawArrays(GL_PATCHES, 0, 3);

  glutSwapBuffers();
}

void shutdown() {
  glDeleteVertexArrays(1, &vao);
  glDeleteProgram(program);
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  GLfloat aspect = GLfloat(width) / height;

  // glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'q':
  case 'Q':
  case 033 /* Escape key */:
    exit(EXIT_SUCCESS);
    break;

  case 'm': // 切换填充模式或镂空模式
  {
    static GLenum mode = GL_FILL;
    mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    break;
  }
  }

  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA /*| GLUT_STENCIL | GLUT_DEPTH*/);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(512, 512);
  glutCreateWindow("TesselateTri"); // get an openGL context

  GLenum err = glewInit(); // init glew
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(-2);
  }

  startup();

  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  return 0;
}