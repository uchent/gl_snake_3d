#include "../GL/glut.h"

#ifndef SHADER_H
#define SHADER_H

GLuint createShader(const char *filename, const char *type);
GLuint createProgram(GLuint vert, GLuint frag);

#include <stdio.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"

#include "shader.h"

GLuint createShader(const char *filename, const char *type)
{
	FILE *fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  //same as rewind(fp);

	char *source = (char *)malloc(sizeof(char) * (fsize + 1));
	fread(source, fsize, 1, fp);
	fclose(fp);

	source[fsize] = 0;

	GLuint shader;
	if (0 == strcmp(type, "vertex"))
	{
		/* GLuint glCreateShader( GLenum shaderType ); */
		shader = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (0 == strcmp(type, "fragment"))
	{
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else
	{
		puts("Error: Unknown type of shader.");
		free(source);
		return -1;
	}

	/*
	void glShaderSource(GLuint shader,
	GLsizei count,
	const GLchar **string,
	const GLint *length);
	*/
	glShaderSource(shader, 1, (const GLchar **)&source, 0);

	glCompileShader(shader);
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char *infoLog = (char *)malloc(sizeof(char) * (maxLength));
		glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

		glDeleteShader(shader);

		puts(infoLog);
		free(infoLog);

		return -1;
	}

	free(source);
	return shader;
}

GLuint createProgram(GLuint vert, GLuint frag)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char *infoLog = (char *)malloc(sizeof(char) * (maxLength));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		glDeleteProgram(program);
		glDeleteShader(vert);
		glDeleteShader(frag);

		puts(infoLog);
		free(infoLog);

		return -1;
	}

	glDetachShader(program, vert);
	glDetachShader(program, frag);

	return program;
}

#endif