#pragma once
struct OBB
{
    glm::vec3 vmin, vmax, color;       // Properties
    glm::vec3 center, extents;         // Model Tranformation

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

    unsigned int VAO, VBO;

    OBB() = default;

    OBB(glm::vec3 vmin, glm::vec3 vmax, glm::vec3 color)
    {
        this->vmin = vmin;
        this->vmax = vmax;
        this->color = color;

        center = (vmax + vmin) * 0.5f;
        extents = glm::vec3((vmax.x - vmin.x), (vmax.y - vmin.y), (vmax.z - vmin.z)) * 0.5f;

        initMesh();
    }

    void draw()
    {
        bind();
        glDrawArrays(GL_LINE_STRIP, 0, 20);
    }

    glm::mat4 getModelTransformation()
    {
        return
            glm::translate(glm::mat4(1.0f), center)*
            glm::scale(glm::mat4(1.0f), extents * 2.0f);
    }

    float scaleNormal(glm::mat4 model)
    {
        glm::vec4 v1 = model * glm::vec4(vmin, 1.0);
        glm::vec4 v2 = model * glm::vec4(vmax, 1.0);

        float diagonal = sqrt(glm::length(v2 - v1));
        return diagonal * 0.05;
    }

    void getInfo(std::ofstream& outfile)
    {
        outfile << "bc " << color.x << " " << color.y << " " << color.z << "\n";
        outfile << "b_vmin " << vmin.x << " " << vmin.y << " " << vmin.z << "\n";
        outfile << "b_vmax " << vmax.x << " " << vmax.y << " " << vmax.z << "\n";
    }

private:
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