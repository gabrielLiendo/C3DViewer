class Texture
{
public:
    Texture(const std::string& filePath){
        this->filePath = filePath;
        load();
    }

    // Load and generate the texture
    bool load(){
        stbi_set_flip_vertically_on_load(1);
        int width = 0, height = 0, nrChannels = 0;
        unsigned char *imageData = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

        if (!imageData)
        {
            std::cout << "Failed to load texture" << std::endl;
            return false;
        }
        
        glGenTextures(1, &textureMesh);
        glBindTexture(GL_TEXTURE_2D, textureMesh);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(imageData);
    }

    void bind()
    {
        glBindTexture(GL_TEXTURE_2D, textureMesh);
    }

private:
    std::string filePath;
    GLuint textureMesh;
};