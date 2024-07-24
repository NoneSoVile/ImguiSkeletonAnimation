#ifndef SHADER_H
#define SHADER_H
#include "GLErrorDef.h"
#include <stdio.h>
#include <string>
using std::string;
#define BASE_TEX_ID   1
class Shader
{
public:
	GLuint program;
	Shader():program(0){};
	GLuint LoadShader(GLenum shader_type, const char* source);
	GLuint CreateProgram(const char* vertex_source, const char* fragment_source, const char* compute_source);
	bool setShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* goemetryPath = NULL);

	bool setComputeShader(const GLchar* shaderPath);
	bool isValid() { return program != 0; }
	// Uses the current shader
	Shader& Use(int tex_loc = BASE_TEX_ID)
	{
		glUseProgram(this->program);
		if (tex_loc > 0) {
			glUniform1i(glGetUniformLocation(this->program, "ourTexture"), tex_loc);
			glActiveTexture(GL_TEXTURE0 + tex_loc);
		}

		return *this;
	}

	void UseAndBindTexture(string texName, int texID, int tex_loc = BASE_TEX_ID)
	{
		glUseProgram(this->program);
		glUniform1i(glGetUniformLocation(this->program, texName.c_str()), tex_loc);
		glActiveTexture(GL_TEXTURE0 + tex_loc);
		glBindTexture(GL_TEXTURE_2D, texID);
	}
	void enable()
	{
		glUseProgram(this->program);
	}
	void disable(){
		glUseProgram(0);
	}

	Shader& setUniformMatrix4fv(const char* name, float* m, int32_t count, bool transpose)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0) {
			glUniformMatrix4fv(loc, count, transpose, m);
		}
		return *this;
	}

	Shader& setUniformMatrix3fv(const char* name, float* m, int32_t count, bool transpose)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0) {
			glUniformMatrix3fv(loc, count, transpose, m);
		}
		return *this;
	}

	GLint getUniformLocation(const char* uniform, bool isOptional)
	{
		GLint result = glGetUniformLocation(this->program, uniform);

		if (result == -1)
		{
			printf("could not find uniform \"%s\" in program %d", uniform, program);
		}

		return result;
	}

	Shader& setUniform1f(GLint index, float value)
	{
		if (index >= 0)
		{
			glUniform1f(index, value);
		}
		return *this;
	}

	Shader& setUniform2f(GLint index, float value1, float value2)
	{
		if (index >= 0)
		{
			glUniform2f(index, value1, value2);
		}
		return *this;
	}

	Shader& setUniform1i(const char *name, int32_t value)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform1i(loc, value);
		}
		return *this;
	}

	Shader& setUniform2i(const char* name, int32_t value1, int32_t value2)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform2i(loc, value1, value2);
		}
		return *this;
	}

	Shader& setUniform3i(const char* name, int32_t value1, int32_t value2, int32_t value3)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform3i(loc, value1, value2, value3);
		}
		return *this;
	}

	Shader& setUniform4i(const char* name, int32_t value1, int32_t value2, int32_t value3, int32_t value4)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform4i(loc, value1, value2, value3, value4);
		}
		return *this;
	}

	Shader& setUniform1ui(const char* name, int32_t value)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform1ui(loc, value);
		}
		return *this;
	}

	Shader& setUniform2ui(const char* name, int32_t value1, int32_t value2)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform2ui(loc, value1, value2);
		}
		return *this;
	}

	Shader& setUniform1f(const char *name, float value)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform1f(loc, value);
		}
		return *this;
	}

	Shader& setUniform2f(const char* name, float value1, float value2)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform2f(loc, value1, value2);
		}
		return *this;
	}

	Shader& setUniform3f(const char* name, float value1, float value2, float value3)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform3f(loc, value1, value2, value3);
		}
		return *this;
	}

	Shader& setUniform4f(const char* name, float value1, float value2, float value3, float value4)
	{
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0)
		{
			glUniform4f(loc, value1, value2, value3, value4);
		}
		return *this;
	}

	Shader& setUniform2fv(const char* name, const float* value, int32_t count) {
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0) {
			glUniform2fv(loc, count, value);
		}
		return *this;
	}

	Shader& setUniform3fv(const char* name, const float* value, int32_t count) {
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0) {
			glUniform3fv(loc, count, value);
		}
		return *this;
	}

	Shader& setUniform4fv(const char* name, const float* value, int32_t count) {
		GLint loc = getUniformLocation(name, false);
		if (loc >= 0) {
			glUniform4fv(loc, count, value);
		}
		return *this;
	}

	GLint getAttribLocation(const char* attribname)
	{
		return glGetAttribLocation(this->program, attribname);
	}

	void enableVertexAttribArray(GLuint index)
	{
		glEnableVertexAttribArray(index);
	}

	void disableVertexAttribArray(GLuint index)
	{
		glDisableVertexAttribArray(index);
	}


	void vertexAttribDivisor(GLuint index, GLuint divisor)
	{
		glVertexAttribDivisor(index, divisor);
	}

	void attributePointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
	{
		glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
	}

};

#endif