#pragma once
class BoundingBox
{
public:
    BoundingBox() = default;

    BoundingBox(glm::vec3 vmin, glm::vec3 vmax, glm::vec3 size)
    {
        this->vmin = vmin;
        this->vmax = vmax;
        this->size = size;

        center = glm::vec3((vmin.x + vmax.x) / 2, (vmin.y + vmax.y) / 2, (vmin.z + vmax.z) / 2);

        initMesh();
    }

    void draw()
    {
        bind();
        glDrawArrays(GL_LINE_LOOP, 0, sizeof(vertices)/36);
    }

    glm::mat4 getModelTransformation()
    {
        return
            glm::translate(glm::mat4(1.0f), center) *
            glm::scale(glm::mat4(1.0f), size);
    }

private:
    // Cube 1x1x1, centered on origin
    float vertices[324] =
    {
        // back
        0.5,0.5,-0.5, 0.0f,  0.0f, -1.0f, 1,0,0,
        0.5,-0.5,-0.5, 0.0f,  0.0f, -1.0f,1,0,0,
       -0.5,0.5,-0.5,  0.0f,  0.0f, -1.0f,1,0,0,
       -0.5,0.5,-0.5,0.0f,  0.0f, -1.0f,1,0,0,
        0.5,-0.5,-0.5, 0.0f,  0.0f, -1.0f,1,0,0,
       -0.5,-0.5,-0.5, 0.0f,  0.0f, -1.0f,1,0,0,

       // front 
      -0.5,0.5,0.5,  0.0f,  0.0f,  1.0f,  1,0,0,
      -0.5,-0.5,0.5,  0.0f,  0.0f,  1.0f, 1,0,0,
       0.5,-0.5,0.5,  0.0f,  0.0f,  1.0f, 1,0,0,
      -0.5,0.5,0.5,  0.0f,  0.0f,  1.0f,  1,0,0,
       0.5,-0.5,0.5, 0.0f,  0.0f,  1.0f,  1,0,0,
       0.5,0.5,0.5,  0.0f,  0.0f,  1.0f,  1,0,0,

       // left
       -0.5,0.5,-0.5, -1.0f,  0.0f,  0.0f,1,0,0,
       -0.5,-0.5,-0.5, -1.0f,  0.0f,  0.0f,1,0,0,
       -0.5,-0.5,0.5, -1.0f,  0.0f,  0.0f,1,0,0,
       -0.5,0.5,-0.5, -1.0f,  0.0f,  0.0f,1,0,0,
       -0.5,-0.5,0.5, -1.0f,  0.0f,  0.0f,1,0,0,
       -0.5,0.5,0.5, -1.0f,  0.0f,  0.0f,1,0,0,

       // right
        0.5,0.5,0.5, 1.0f,  0.0f,  0.0f, 1,0,0,
        0.5,-0.5,0.5, 1.0f,  0.0f,  0.0f,1,0,0,
        0.5,-0.5,-0.5, 1.0f,  0.0f,  0.0f,1,0,0,
        0.5,0.5,0.5, 1.0f,  0.0f,  0.0f,1,0,0,
        0.5,-0.5,-0.5, 1.0f,  0.0f,  0.0f,1,0,0,
        0.5,0.5,-0.5, 1.0f,  0.0f,  0.0f,1,0,0,

        // bottom
       -0.5,-0.5,0.5, 0.0f, -1.0f,  0.0f,1,0,0,
       -0.5,-0.5,-0.5, 0.0f, -1.0f,  0.0f,1,0,0,
        0.5,-0.5,-0.5,0.0f, -1.0f,  0.0f,1,0,0,
       -0.5,-0.5,0.5, 0.0f, -1.0f,  0.0f,1,0,0,
        0.5,-0.5,-0.5, 0.0f, -1.0f,  0.0f,1,0,0,
        0.5,-0.5,0.5, 0.0f, -1.0f,  0.0f,1,0,0,

        // top
        -0.5,0.5,-0.5, 0.0f,  1.0f,  0.0f, 1,0,0,
        -0.5,0.5,0.5, 0.0f,  1.0f,  0.0f, 1,0,0,
         0.5,0.5,0.5, 0.0f,  1.0f,  0.0f, 1,0,0,
        -0.5,0.5,-0.5,  0.0f,  1.0f,  0.0f, 1,0,0,
         0.5,0.5,0.5, 0.0f,  1.0f,  0.0f,1,0,0,
         0.5,0.5,-0.5, 0.0f,  1.0f,  0.0f,1,0,0,
    };

    // Model Tranformation
    glm::vec3 center, size;
    glm::vec3 color;

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
        glBufferData(GL_ARRAY_BUFFER, 324*sizeof(float), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void bind()
    {
        glBindVertexArray(VAO);
    }
};