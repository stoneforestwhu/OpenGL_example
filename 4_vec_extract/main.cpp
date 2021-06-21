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
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  program = glCreateProgram();
  GLuint cs_0 = glCreateShader(GL_COMPUTE_SHADER);
  glAttachShader(program, cs_0);
  glDeleteShader(cs_0);
  std::shared_ptr<char> cs_string = readShaderFile("shader/cs.txt");
  char *cs_string_ptr = cs_string.get();
  glShaderSource(cs_0, 1, &(cs_string_ptr), NULL);
  glCompileShader(cs_0);
  GLint compile_status = 0;
  glGetShaderiv(cs_0, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != 1) {
    char shader_log[1000];
    int shader_log_len = 0;
    glGetShaderInfoLog(cs_0, 1000, &shader_log_len, shader_log);
    std::cout << shader_log << std::endl;
  }

  glLinkProgram(program);
  GLint link_status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != 1) {
    char prog_info_log[1000];
    GLsizei prog_info_len = 0;
    glGetProgramInfoLog(program, 1000, &prog_info_len, prog_info_log);
    std::cout << prog_info_log << std::endl;
  }

  GLuint buffer_0[2];
  glGenBuffers(2, buffer_0);

  //  TestInput:g_input[4];
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_0[0]);
  // void *buf0_ptr = readBufferFile("data/bufD_0.bin", 64);
  int buf0_ptr[4] = {1, 2, 3, 4};
  glBufferData(GL_SHADER_STORAGE_BUFFER, 16, buf0_ptr, GL_DYNAMIC_COPY);

  //  TestOutput:g_output[4]
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer_0[1]);
  // void *buf1_ptr = readBufferFile("data/bufD_1.bin", 64).;
  glBufferData(GL_SHADER_STORAGE_BUFFER, 16, NULL, GL_DYNAMIC_COPY);

  glUseProgram(program);

  glDispatchCompute(1, 1, 1);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_0[1]);
  shared_ptr<int> res_data(new int[16]);
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint) * 4, res_data.get());
  writeIntDataToFile(res_data.get(), 16);
	exit(0);
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
  glutInitWindowSize(400, 300);
  glutCreateWindow("TesselateTri"); // get an openGL context

  GLenum err = glewInit(); // init glew
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(-2);
  }

  compute();

  glutMainLoop();

  return 0;
}