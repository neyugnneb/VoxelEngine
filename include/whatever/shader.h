#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // Debug: Print current working directory and file paths
        std::cout << "Trying to load vertex shader: " << vertexPath << std::endl;
        std::cout << "Trying to load fragment shader: " << fragmentPath << std::endl;
        
        // Debug: Check current working directory
        std::cout << "Current working directory check..." << std::endl;
        system("cd");
        
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
        // Test if files exist before setting exception handling
        std::ifstream test1(vertexPath);
        if (!test1.good()) {
            std::cout << "ERROR: Cannot access vertex shader file: " << vertexPath << std::endl;
        } else {
            std::cout << "Vertex shader file found successfully" << std::endl;
        }
        test1.close();
        
        std::ifstream test2(fragmentPath);
        if (!test2.good()) {
            std::cout << "ERROR: Cannot access fragment shader file: " << fragmentPath << std::endl;
        } else {
            std::cout << "Fragment shader file found successfully" << std::endl;
        }
        test2.close();
        
        try 
        {
            // open files
            std::cout << "Opening vertex shader file..." << std::endl;
            vShaderFile.open(vertexPath);
            if (!vShaderFile.is_open()) {
                std::cout << "ERROR: Failed to open vertex shader file" << std::endl;
                return;
            }
            
            std::cout << "Opening fragment shader file..." << std::endl;
            fShaderFile.open(fragmentPath);
            if (!fShaderFile.is_open()) {
                std::cout << "ERROR: Failed to open fragment shader file" << std::endl;
                return;
            }
            
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            std::cout << "Reading vertex shader content..." << std::endl;
            vShaderStream << vShaderFile.rdbuf();
            std::cout << "Reading fragment shader content..." << std::endl;
            fShaderStream << fShaderFile.rdbuf();
            
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            
            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            std::cout << "Shaders loaded successfully!" << std::endl;
            std::cout << "Vertex shader length: " << vertexCode.length() << " characters" << std::endl;
            std::cout << "Fragment shader length: " << fragmentCode.length() << " characters" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return; // Exit constructor early on file error
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        std::cout << "Shader program created successfully with ID: " << ID << std::endl;
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif