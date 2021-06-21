#include <GL/glew.h>
#include <GL/glut.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <io.h>

using std::ifstream;
using std::ios;
using std::ostringstream;
using std::string;
using std::vector;
using std::shared_ptr;

GLuint program;
GLuint vao;

bool exist_opengl32() { return _access("opengl32.dll", 0) != -1; }

typedef struct __trace_img_header // 8 Byte align
{
  unsigned int magicNum;
  GLenum format;
  GLenum type;
  GLenum target;
  uint64_t width;
  uint64_t height;
  uint64_t depth;
  uint64_t imageSize;
} __trace_img_header;

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

std::shared_ptr<char> readBufferFile(std::string bufFileName, size_t bufSize) {
  std::shared_ptr<char> bufPtr(new char[bufSize + 1]);
  std::fstream bufFile(bufFileName, ios::in | ios::binary);
  if (!bufFile) {
    std::cout << "can't find " << bufFileName << " \n";
    assert(0);
  }
  bufFile.read(bufPtr.get(), bufSize);
  return bufPtr;
}

std::shared_ptr<char> readTexFile(std::string texFileName, size_t texSize) {
  __trace_img_header a_header;
  unsigned int header_size = sizeof(a_header);
  std::shared_ptr<char> bufPtr(new char[texSize + 1]);
  std::fstream bufFile(texFileName, ios::in | ios::binary);
  if (!bufFile) {
    std::cout << "can't find " << texFileName << " \n";
    assert(0);
  }
  bufFile.seekg(header_size, ios::beg); //  set offset
  bufFile.read(bufPtr.get(), texSize);
  return bufPtr;
}

template<typename T>
void printData(T *data, unsigned int row, unsigned int coloum) {
  for (unsigned int i = 0; i < row; i++) {
    for (unsigned int j = 0; j < coloum; j++) {
      std::cout << data[i * coloum + j] << " ";
    }
    std::cout << "\n";
  }
}

void writeFpDataToFile(float *data, unsigned int row, unsigned int coloum) {
  static int counter = 0;
  char *log_subfix;
  char result_file_name[512];
  if (exist_opengl32())
    log_subfix = "wx";
  else
    log_subfix = "nv";
  sprintf_s(result_file_name, "cmp/fp_result_%s_%d.log", log_subfix, counter);
  std::fstream outFile(result_file_name, ios::out);
  for (unsigned int i = 0; i < row; i++) {
    for (unsigned int j = 0; j < coloum; j++) {
      float aVal = data[i * coloum + j];
      char aStr[512];
      sprintf_s(aStr, "%f", aVal);
      outFile << aStr << " ";
    }
    outFile << "\n";
  }
  outFile << "==========================\n";
  outFile.close();
  counter++;
}

void writeIntDataToFile(int *data, unsigned int tex_size) {
  static int counter = 0;
  char *log_subfix;
  char result_file_name[512];
  unsigned int intNum = tex_size/4;
  unsigned int coloum = 4;
  unsigned int row = intNum / coloum;
  unsigned int extra_size = tex_size % coloum;
  if (exist_opengl32())
    log_subfix = "wx";
  else
    log_subfix = "nv";
  sprintf_s(result_file_name, "cmp/int_result_%s_%d.log", log_subfix, counter);
  std::fstream outFile(result_file_name, ios::out);
  for (unsigned int i = 0; i < row; i++) {
    for (unsigned int j = 0; j < coloum; j++) {
      int aVal = data[i * coloum + j];
      char aStr[512];
      sprintf_s(aStr, "0x%x", aVal);
      outFile << aStr << " ";
    }
    outFile << "\n";
  }
  for (unsigned int k = 0; k < extra_size; ++k) {
    int aVal = data[row * coloum + k];
    char aStr[512];
    sprintf_s(aStr, "0x%x", aVal);
    outFile << aStr << " ";
  }
  outFile << "\n";
  outFile << "==========================\n";
  outFile.close();
  counter++;
}

void compute(){
	GLuint vs_0=glCreateShader(GL_VERTEX_SHADER);
  std::shared_ptr<char> vs_0_src_0_shared_ptr = readShaderFile("shader/vs_0_src_0.txt");
  char* vs_0_src_0 = vs_0_src_0_shared_ptr.get();
  char** vs_0_srcPtr_0 = &vs_0_src_0;
  glShaderSource(vs_0, 1, vs_0_srcPtr_0, NULL);
  glCompileShader(vs_0);
  GLint  compile_status_1;
  glGetShaderiv(vs_0, GL_COMPILE_STATUS, &compile_status_1);
  GLuint ps_0=glCreateShader(GL_FRAGMENT_SHADER);

  std::shared_ptr<char> ps_0_src_0_shared_ptr = readShaderFile("shader/ps_0_src_0.txt");
  char* ps_0_src_0 = ps_0_src_0_shared_ptr.get();
  char** ps_0_srcPtr_0 = &ps_0_src_0;
  glShaderSource(ps_0, 1, ps_0_srcPtr_0, NULL);
  glCompileShader(ps_0);
  GLint  compile_status_2;
  glGetShaderiv(ps_0, GL_COMPILE_STATUS, &compile_status_2);
  if (compile_status_2 != GL_TRUE) {
    char shader_log[1024];
    glGetShaderInfoLog(ps_0, 1024, NULL, shader_log);
    std::cout << shader_log << std::endl;
    assert(0);
  }
  program = glCreateProgram();
  glAttachShader(program, vs_0);
  glAttachShader(program, ps_0);
  glBindAttribLocation(program, 0, "aPos"); // trace add
  glLinkProgram(program);
  GLint  prog_status_0;
  glGetProgramiv(program, GL_LINK_STATUS, &prog_status_0); // 1
  glDeleteShader(vs_0);
  glDeleteShader(ps_0);

  GLuint buffer_0;
  glGenBuffers(1, &buffer_0);

  //  TestInput:g_input[4];
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_0);
  // void *buf0_ptr = readBufferFile("data/bufD_0.bin", 64);
  float buf0_ptr[4096];   //  1024 vec4
  for (unsigned int i = 0; i < 4096; ++i)
    buf0_ptr[i] = 5.0f;

  glBufferData(GL_SHADER_STORAGE_BUFFER, 16384, buf0_ptr, GL_DYNAMIC_COPY);

  GLuint vao_0[1];
  glGenVertexArrays(1, vao_0);

  GLuint buf_0[1];
  glGenBuffers(1, buf_0);
  glBindVertexArray(vao_0[0]);
  glBindBuffer(GL_ARRAY_BUFFER, buf_0[0]);

  float vertices[] = {
      -1.0f, -1.0f, 0.0f, // left
      1.0f,  -1.0f, 0.0f, // right
      0.0f,  1.0f,  0.0f  // top
  }; 
  glBufferData(GL_ARRAY_BUFFER, 36, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0x0, 3, GL_FLOAT, GL_FALSE, 12, 0);
  glEnableVertexAttribArray(0x0);
  glBindBuffer(GL_ARRAY_BUFFER, 0x0);
  glBindVertexArray(0x0);
  unsigned int int_val_0 = 0x3e4ccccd;
  float fp_val_0;
  memcpy(&fp_val_0, &int_val_0, 4);
  unsigned int int_val_1 = 0x3e99999a;
  float fp_val_1;
  memcpy(&fp_val_1, &int_val_1, 4);
  unsigned int int_val_2 = 0x3e99999a;
  float fp_val_2;
  memcpy(&fp_val_2, &int_val_2, 4);
  unsigned int int_val_3 = 0x3f800000;
  float fp_val_3;
  memcpy(&fp_val_3, &int_val_3, 4);
  glClearColor(fp_val_0, fp_val_1, fp_val_2, fp_val_3);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(program);
  glBindVertexArray(vao_0[0]);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_0);
  shared_ptr<float> data_ptr(new float[4096]);
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 16384, data_ptr.get());
  writeFpDataToFile(data_ptr.get(), 1024, 4);
  //exit(0);
}

void render() {
  static const GLfloat green[] = {0.2f, 0.3f, 0.3f, 1.0f};
  glClearBufferfv(GL_COLOR, 0, green);

  glUseProgram(program);
  glDrawArrays(GL_TRIANGLES, 0, 3);

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
  glutInitWindowSize(400, 300);
  glutCreateWindow("TesselateTri"); // get an openGL context

  GLenum err = glewInit(); // init glew
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(-2);
  }

  compute();
  glutDisplayFunc(render);
  glutMainLoop();

  return 0;
}