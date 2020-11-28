#include "shader.h"
#include <fstream>
#include <iostream>

Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename) :
	shaderHandle(createShader(vertexShaderFilename, fragmentShaderFilename)) {
    bind();
}

Shader::~Shader() {
    glDeleteProgram(shaderHandle);
}

void Shader::bind() {
    glUseProgram(shaderHandle);
}

void Shader::unbind() {
    glUseProgram(0);
}

GLuint Shader::getHandle() const {
	return shaderHandle;
}

GLuint Shader::compile(std::string shaderSource, GLenum type) {
	auto id = glCreateShader(type);
	const char* src = shaderSource.c_str();
	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if(result != GL_TRUE) {
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Shader compilation error: " << message << std::endl;
		delete[] message;
		id = 0;
	}

	return id;
}

std::string Shader::parse(const char* filename) {
	auto* file = fopen(filename, "rb");
	if (!file)
		return "";

	std::string contents;
	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);
	contents.resize(filesize);

	fread(&contents[0], 1, filesize, file);
	fclose(file);

	return contents;
}

GLuint Shader::createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
    auto vertexShaderSource = parse(vertexShaderFilename);
    auto fragmentShaderSource = parse(fragmentShaderFilename);

    auto program = glCreateProgram();
    auto vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    auto fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    #ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
    #endif

    return program;
}
