#ifndef SHADER_H
#define SHADER_H
class Shader
{
public:
    unsigned int ID;
    Shader() = default;

    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = NULL)
    {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode, fragmentCode, geometryCode;
        std::ifstream vShaderFile, fShaderFile, gShaderFile;

        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::badbit);
        if(geometryPath)
            gShaderFile.exceptions(std::ifstream::badbit);

        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            if (geometryPath)
            {
                std::stringstream gShaderStream;
                gShaderFile.open(geometryPath);
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        // 2. Compile shaders
        GLuint vertex, fragment, geometry;
        GLint success;
        GLchar infoLog[512];

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // Print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // Print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        if (geometryPath)
        {
            const GLchar* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            // Print compile errors if any
            glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(geometry, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }

        // Shader Program
        this->ID = glCreateProgram();
        glAttachShader(this->ID, vertex);
        if (geometryPath)
            glAttachShader(this->ID, geometry);
        glAttachShader(this->ID, fragment);
        glLinkProgram(this->ID);

        // Print linking errors if any
        glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath)
            glDeleteShader(geometry);

    }
    // Uses the current shader
    void use() 
    { 
        glUseProgram(this->ID); 
    }

    void setVec3f(const std::string &name, glm::vec3 value)
    {
        glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
    }

    void setMat4f(const std::string &name, glm::mat4 value)
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setBool(const std::string &name, bool value)
    {
        glUniform1i(getUniformLocation(name), (int)value);
    }

    void setInt(const std::string &name, int value)
    {
        glUniform1i(getUniformLocation(name), value);
    }
    void setFloat(const std::string &name, float value)
    {
        glUniform1f(getUniformLocation(name), value);
    }

private:
    int getUniformLocation(const std::string &name)
    {
        return glGetUniformLocation(ID, name.c_str());
    }
};

#endif


