#include "Assets/shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace Assets
{
    Shader::Shader(std::string vertexPath, std::string fragmentPath)
    {
        // Create shader
        mID = glCreateProgram();
        attachShader(GL_VERTEX_SHADER, vertexPath, "VERTEX");
        attachShader(GL_FRAGMENT_SHADER, fragmentPath, "FRAGMENT");
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");
    }

    Shader::Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
    {
        // Create shader
        mID = glCreateProgram();
        attachShader(GL_VERTEX_SHADER, vertexPath, "VERTEX");
        attachShader(GL_GEOMETRY_SHADER, geometryPath, "GEOMETRY");
        attachShader(GL_FRAGMENT_SHADER, fragmentPath, "FRAGMENT");
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");
    }

    Shader::Shader(std::string vertexPath, std::string tcsPath, std::string tesPath, std::string geomPath, std::string fragmentPath)
    {
        // Create shader
        mID = glCreateProgram();
        attachShader(GL_VERTEX_SHADER, vertexPath, "VERTEX");
        attachShader(GL_TESS_CONTROL_SHADER, tcsPath, "TCS");
        attachShader(GL_TESS_EVALUATION_SHADER, tesPath, "TES");
        attachShader(GL_GEOMETRY_SHADER, geomPath, "GEOMETRY");
        attachShader(GL_FRAGMENT_SHADER, fragmentPath, "FRAGMENT");
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");
    }

    Shader::~Shader()
    {
    }

    void Shader::attachShader(GLuint shaderType, std::string shaderPath, std::string shaderTypeStr)
    {
        // Retrieve the source code from shaderPath
        std::string shaderCode;
        std::ifstream shaderFile;

        // Ensure ifstream objects can throw exceptions:
        shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            // Open file
            shaderFile.open(shaderPath);
            std::stringstream shaderStream;
            // Read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // Close file handlers
            shaderFile.close();
            // Convert stream into string
            shaderCode = shaderStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char *shaderCodeCStr = shaderCode.c_str();
        
        // Compile the shaders
        unsigned int shaderID;
        shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, &shaderCodeCStr, NULL);
        glCompileShader(shaderID);
        checkCompileErrors(shaderID, shaderTypeStr);

        // Attach
        glAttachShader(mID, shaderID);
    }
    
    // Utility function for checking shader compilation/linking errors.
    void Shader::checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void Shader::use() const
    {
        glUseProgram(mID);
    }

    // Utility uniform functions
    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
    }
    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
    }
    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
    }
    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
    }
    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
}