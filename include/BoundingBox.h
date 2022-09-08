#pragma once
class BoundingBox
{
public:
    BoundingBox() = default;

    BoundingBox(glm::vec3 vmin, glm::vec3 vmax, glm::vec3 size, std::shared_ptr<glm::vec3>& normalize,
             std::shared_ptr<glm::vec3> &translation, std::shared_ptr<glm::vec3> &scale, std::shared_ptr<glm::vec3>& angles)
    {
        this->vmin = vmin;
        this->vmax = vmax;
        this->size = size;
        this->normalize = normalize;
        this->translation = translation;
        this->scale = scale;
        this->angles = angles;

        center = glm::vec3((vmin.x + vmax.x)/ 2, (vmin.y + vmax.y)/ 2, (vmin.z + vmax.z) * 2);
        initMesh();
    }

    void draw()
    {
        bind();
        glDrawArrays(GL_LINE_STRIP, 0, 20);
    }

    glm::vec3* getBoxColor()
    {
        return &color;
    }

    glm::mat4 getModelTransformation()
    {
        return
            glm::translate(glm::mat4(1.0f), *translation) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angles->x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angles->y), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angles->z), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), *scale)*

            glm::scale(glm::mat4(1.0f), *normalize)*

            glm::translate(glm::mat4(1.0f), center)*
            glm::scale(glm::mat4(1.0f), size);
    }

    float scaleNormal(glm::mat4 model)
    {
        glm::vec4 v1 = model * glm::vec4(vmin, 1.0);
        glm::vec4 v2 = model * glm::vec4(vmax, 1.0);

        float diagonal = sqrt(glm::length(v2 - v1));
        return diagonal * 0.05;
    }

private:
    // Cube 1x1x1, centered on origin
    float vertices[120] =
    {
        // back
       -0.5, 0.5,-0.5,  0.0f,  0.0f, -1.0f,
        0.5, 0.5,-0.5,  0.0f,  0.0f, -1.0f, 
        0.5,-0.5,-0.5,  0.0f,  0.0f, -1.0f,
       -0.5,-0.5,-0.5,  0.0f,  0.0f, -1.0f,

       // left
       -0.5,-0.5,0.5,  -1.0f,  0.0f,  0.0f,
       -0.5,0.5,0.5,   -1.0f,  0.0f,  0.0f,
       -0.5,0.5,-0.5,  -1.0f,  0.0f,  0.0f,
        -0.5,-0.5,-0.5, -1.0f,  0.0f,  0.0f,
     
        // top
       -0.5,0.5,-0.5, 0.0f,  1.0f,  0.0f,
       -0.5,0.5,0.5,  0.0f,  1.0f,  0.0f,
        0.5,0.5,0.5,  0.0f,  1.0f,  0.0f,
        0.5,0.5,-0.5, 0.0f,  1.0f,  0.0f,

        // right
       0.5, 0.5,-0.5,  1.0f,  0.0f,  0.0f,
       0.5,-0.5,-0.5, 1.0f,  0.0f,  0.0f,
       0.5,-0.5,0.5,  1.0f,  0.0f,  0.0f,
       0.5, 0.5, 0.5,   1.0f,  0.0f,  0.0f,


       // front 
        0.5,0.5,0.5,   0.0f,  0.0f,  1.0f,
       -0.5,0.5,0.5,   0.0f,  0.0f,  1.0f,
         -0.5,-0.5,0.5,  0.0f,  0.0f,  1.0f,
        0.5,-0.5,0.5,  0.0f,  0.0f,  1.0f,
    };

    // Model Tranformation
    glm::vec3 size;
    std::shared_ptr <glm::vec3>  normalize, scale, translation, angles;
    glm::vec3 center;
    glm::vec3 color = {1.0, 1.0, 1.0};

    glm::vec3 vmin, vmax;
    float min_x, max_x, min_y, max_y, min_z, max_z;
    unsigned int VAO, VBO;

    void initMesh()
    {
        // Gen and bind container's VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Gen and bind VBO, We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void bind()
    {
        glBindVertexArray(VAO);
    }
};