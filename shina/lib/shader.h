#pragma once
#ifndef SHINA_LIB_SHADER_H
#define SHINA_LIB_SHADER_H

#include "main.h"
#include "exception.h"
#include <sstream>



class smShader {
private:
	smShader();
	unsigned int shaderID;
	void checkCompileErrors(GLuint normalShader, std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(normalShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(normalShader, 1024, NULL, infoLog);
				std::cout << "Shader compile error of : " << type << "\n" << infoLog << std::endl;
			}
		}
		else {
			glGetProgramiv(normalShader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(normalShader, 1024, NULL, infoLog);
				std::cout << "Shader link error:" << "\n" << infoLog << std::endl;
			}
		}
	}

public:
	smShader(string vetexShaderFile, string fragmentShaderFile)
	{
		std::string vetexFileContent;
		std::string fragmentFileContent;
		std::ifstream vetexStream(vetexShaderFile,std::ios::in);
		std::ifstream fragmentStream(fragmentShaderFile,std::ios::in);
		try {
			std::stringstream vetexBuffer;
			std::stringstream fragmentBuffer;
			vetexBuffer << vetexStream.rdbuf();
			fragmentBuffer << fragmentStream.rdbuf();
			vetexStream.close();
			fragmentStream.close();
			vetexFileContent = vetexBuffer.str();
			fragmentFileContent = fragmentBuffer.str();
			const char* vetexCstr = vetexFileContent.c_str();
			const char* fragmentCstr = fragmentFileContent.c_str();
			unsigned int vetexID, fragmentID;
			//vetex shader
			std::cout << vetexCstr << std::endl;
			vetexID = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vetexID, 1, &(vetexCstr), NULL);
			glCompileShader(vetexID);
			checkCompileErrors(vetexID, "VERTEX");
			//fragment shader
			fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentID, 1, &(fragmentCstr), NULL);
			glCompileShader(fragmentID);
			checkCompileErrors(fragmentID, "FRAGMENT");
			//link
			shaderID = glCreateProgram();
			glAttachShader(shaderID, vetexID);
			glAttachShader(shaderID, fragmentID);
			glLinkProgram(shaderID);
			checkCompileErrors(shaderID, "PROGRAM");
			glDeleteShader(vetexID);
			glDeleteShader(fragmentID);

		}
		catch (std::ifstream::failure &e)
		{
			throw e;
		}
	}
	void use() {
		glUseProgram(shaderID);
	}
	void setBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
	}
	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
	}
	void setVec2(const std::string &name, const glm::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const {
		glUniform2f(glGetUniformLocation(shaderID, name.c_str()), x, y);
	}
	void setVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(shaderID, name.c_str()), x, y, z);
	}
	void setVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(shaderID, name.c_str()), x, y, z, w);
	}
	void setMat2(const std::string &name, const glm::mat2 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const std::string &name, const glm::mat3 &mat) const {
		glUniformMatrix3fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const std::string &name, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}


};

#endif