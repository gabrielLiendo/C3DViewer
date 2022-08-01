#pragma once
class Model
{
public:
    int size;

    // data =  x,y,z,nx,ny,nz,x,y,z,nx,ny,nz, .... 
    Model(GLfloat* data, int size)
    {
        this->size = size;
        // First, set the container's VAO (and VBO)
        glGenVertexArrays(1, &containerVAO);
        glBindVertexArray(containerVAO);

        glGenBuffers(1, &VBO);
        // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    void draw()
    {
        bind();
        glDrawArrays(GL_TRIANGLES, 0, size / 36);
    }

private:
    GLuint VBO, containerVAO;

    void bind()
    {
        glBindVertexArray(containerVAO);
    }

};