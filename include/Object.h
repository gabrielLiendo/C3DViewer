#pragma once
class Object
{
public:
	std::string name;

	// Model Tranformation Components
	glm::vec3 center, normalize, scale, translation, oldAngles, angles;

	// Bounding box
	OBB boundingBox;

	Object() = default;

	Object(std::vector<Mesh> meshes, glm::vec3 pickingColor, std::string path)
	{
		name = "";
		this->path = path;
		this->meshes = meshes;
		this->pickingColor = pickingColor;

		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, scaleFactor;

		min_x = max_x = meshes[0].vertices[0].position.x;
		min_y = max_y = meshes[0].vertices[0].position.y;
		min_z = max_z = meshes[0].vertices[0].position.z;

		size_t n = meshes.size();
		for (size_t  i = 0; i < n; i++)
		{
			size_t m = meshes[i].vertices.size();
			for (size_t  j = 0; j < m; j++)
			{
				Vertex vertex = meshes[i].vertices[j];
				if (vertex.position.x < min_x) min_x = vertex.position.x;
				if (vertex.position.x > max_x) max_x = vertex.position.x;
				if (vertex.position.y < min_y) min_y = vertex.position.y;
				if (vertex.position.y > max_y) max_y = vertex.position.y;
				if (vertex.position.z < min_z) min_z = vertex.position.z;
				if (vertex.position.z > max_z) max_z = vertex.position.z;
			}
		}

		// Normalize mesh into unitary parallelepiped
		glm::vec3 vmax = glm::vec3(max_x, max_y, max_z);
		glm::vec3 vmin = glm::vec3(min_x, min_y, min_z);

		scaleFactor = std::max(std::max(max_x - min_x, max_y - min_y), max_z - min_z);
		normalize = glm::vec3(1 / scaleFactor, 1 / scaleFactor, 1 / scaleFactor);
		center = -(vmax + vmin) * 0.5f;

		// Initialize model matrix components
		translation = glm::vec3(0.0, 0.0, 0.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		angles = glm::vec3(0.0f, 0.0f, 0.0f);
		oldAngles = angles;

		// Initialize matrices
		normalizeMat = glm::scale(glm::mat4(1.0f), normalize);
		centerMat = glm::translate(glm::mat4(1.0f), center);
		scaleMat = glm::scale(glm::mat4(1.0f), scale);
		translationMat = glm::translate(glm::mat4(1.0f), translation);
		
		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		rotationMat = rotX * rotY * rotZ;

		// Create bounding box
		boundingBox = OBB(vmin, vmax, { 1.0, 1.0, 1.0 });
	}

	Object(std::vector<Mesh> meshes, std::string path, std::string name, glm::vec3 normalize, glm::vec3 scale, glm::vec3 translation, glm::vec3 angles,
		bool showWireframe, bool showVertices, bool showNormals, int pointSize,
		glm::vec3 pickingColor, glm::vec3 wireframeColor, glm::vec3 verticesColor, glm::vec3 normalsColor, glm::vec3 boxColor, glm::vec3 vmin, glm::vec3 vmax)
	{
		this->meshes = meshes;
		this->path = path;
		this->name = name;
		this->normalize = normalize;
		this->scale = scale;
		this->translation = translation;
		this->angles = angles;
		oldAngles = angles;

		this->showWireframe = showWireframe;
		this->showVertices = showVertices;
		this->showNormals = showNormals;

		this->pickingColor = pickingColor;
		this->pointSize = pointSize;
		this->wireframeColor = wireframeColor;
		this->verticesColor = verticesColor;
		this->normalsColor = normalsColor;

		this->boundingBox = OBB(vmin, vmax, boxColor);

		// Initialize matrices
		normalizeMat = glm::scale(glm::mat4(1.0f), normalize);
		centerMat = glm::translate(glm::mat4(1.0f), center);
		scaleMat = glm::scale(glm::mat4(1.0f), scale);
		translationMat = glm::translate(glm::mat4(1.0f), translation);

		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		rotationMat = rotX * rotY * rotZ;
	}

	void addXRot(double xoffset)
	{
		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians((float)xoffset), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMat = rotX * rotationMat;

		oldAngles.x = angles.x;
		angles.x += xoffset;
	}

	void addYRot(double yoffset)
	{
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians((float)yoffset), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMat = rotY * rotationMat;

		oldAngles.y = angles.y;
		angles.y += yoffset;
	}

	void setScaleMat(){ scaleMat = glm::scale(glm::mat4(1.0f), scale); }
	
	void setRotationsMats()
	{	
		glm::vec3 offsetAngles = angles - oldAngles;

		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(offsetAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(offsetAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(offsetAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		oldAngles = angles;

		rotationMat = rotX * rotY * rotZ * rotationMat;
	}

	void setTranslationMat(){ translationMat = glm::translate(glm::mat4(1.0f), translation); }

	glm::mat4 getBoxModel()
	{
		return boundingBox.getModelTransformation();
	}

	glm::vec3* getBoxColor()
	{
		return &boundingBox.color;
	}

	glm::vec3* getNormalsColor()
	{
		return &normalsColor;
	}

	float getScaleNormal()
	{
		return boundingBox.scaleNormal(getModelTransformation());
	}

	int* getPointSize()
	{
		return &pointSize;
	}

	bool* getWireframeBool()
	{
		return &showWireframe;
	}

	bool* getShowVertices()
	{
		return &showVertices;
	}

	bool* getShowNormals()
	{
		return &showNormals;
	}

	glm::vec3* getWireframeColor()
	{
		return &wireframeColor;
	}

	glm::vec3* getVerticesColor()
	{
		return &verticesColor;
	}

	glm::mat4 getModelTransformation()
	{
		return translationMat * rotationMat * scaleMat * normalizeMat * centerMat;
	}

	void drawFlatPicking(int colorLoc)
	{	
		int n = (int)meshes.size();
		for (int i = 0; i < n; i++)
		{
			glUniform3f(colorLoc, pickingColor.x, pickingColor.y, pickingColor.z);
			meshes[i].draw();
		}
	}

	void draw(bool lighting, int diffuseLoc, int ambientLoc, int specularLoc)
	{
		glm::vec3 color;
		size_t n = meshes.size();

		if (showWireframe)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0, -1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (size_t  i = 0; i < n; i++)
			{
				glUniform3f(diffuseLoc, wireframeColor.x, wireframeColor.y, wireframeColor.z);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}

		for (size_t  i = 0; i < n; i++)
		{
			if (*meshes[i].getShow())
			{
				color = *meshes[i].mtl->getDiffuse();
				glUniform3f(diffuseLoc, color.x, color.y, color.z);

				if (lighting)
				{
					color = *meshes[i].mtl->getAmbient();
					glUniform3f(ambientLoc, color.x, color.y, color.z);

					color = *meshes[i].mtl->getSpecular();
					glUniform3f(specularLoc, color.x, color.y, color.z);
				}
				
				meshes[i].draw();
			}
		}
	}
	
	void drawVertices()
	{
		glEnable(GL_PROGRAM_POINT_SIZE);

		glPointSize((float)pointSize);
		for (size_t  i = 0; i < meshes.size(); i++)
			meshes[i].drawVertex();

		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

	void getInfo(std::ofstream &outfile)
	{
		outfile << "o " << name << "\n";
		outfile << "p " << path << "\n";
		outfile << "n " << normalize.x << " " << normalize.y << " " << normalize.z << "\n";
		outfile << "s " << scale.x << " " << scale.y << " " << scale.z << "\n";
		outfile << "t " << translation.x << " " << translation.y << " " << translation.z << "\n";
		outfile << "a " << angles.x << " " << angles.y << " " << angles.z << "\n";
		outfile << "rs " << " " << showWireframe << " " << showVertices << " " << showNormals << "\n";
		outfile << "pt " << pointSize << "\n";
		outfile << "pc " << pickingColor.x << " " << pickingColor.y << " " << pickingColor.z << "\n";
		outfile << "wc " << wireframeColor.x << " " << wireframeColor.y << " " << wireframeColor.z << "\n";
		outfile << "vc " << verticesColor.x << " " << verticesColor.y << " " << verticesColor.z << "\n";
		outfile << "nc " << normalsColor.x << " " << normalsColor.y << " " << normalsColor.z << "\n";
		boundingBox.getInfo(outfile);

		for (size_t  i = 0; i < meshes.size(); i++)
			meshes[i].getInfo(outfile);
	}

	std::vector<Mesh>* getMeshes()
	{
		return &meshes;
	}

private:
	// Object basic components
	std::string path;
	std::vector<Mesh> meshes;

	// Model matrices
	glm::mat4 centerMat, normalizeMat, translationMat, scaleMat, rotationMat;

	// Rendering State
	bool showWireframe = false;
	bool showVertices = false; int pointSize = 3;
	bool showNormals = false;

	// Colors
	glm::vec3 pickingColor;
	glm::vec3 wireframeColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 verticesColor = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 normalsColor = glm::vec3(0.0f, 1.0f, 0.0f);
};