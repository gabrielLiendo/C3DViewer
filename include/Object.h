#pragma once
class Object
{
public:
	std::string name = "";

	// Model Tranformation Components
	glm::vec3 center, normalize, scale, translation, oldAngles, angles;

	// Bounding box
	OBB boundingBox;

	Object() = default;

	Object(std::vector<Mesh> meshes, glm::vec3 pickingColor, std::string path)
	{
		this->path = path;
		this->meshes = meshes;
		this->pickingColor = pickingColor;

		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, scaleFactor;

		min_x = max_x = meshes[0].vertices[0].position.x;
		min_y = max_y = meshes[0].vertices[0].position.y;
		min_z = max_z = meshes[0].vertices[0].position.z;

		// Find min and max values for bounding box
		int n = (int)meshes.size();
		for(int  i = 0; i < n; i++)
		{
			int m = (int)meshes[i].vertices.size();
			for (int j = 0; j < m; j++)
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
		initMatrices();

		// Create bounding box
		boundingBox = OBB(vmin, vmax, { 1.0, 1.0, 1.0 });
	}

	Object(std::vector<Mesh> meshes, std::string path, std::string name, glm::vec3 normalize, glm::vec3 scale, glm::vec3 translation, glm::mat4 rotationMat, glm::vec3 angles,
		bool showWireframe, bool showVertices, bool showNormals, int pointSize,
		glm::vec3 pickingColor, glm::vec3 wireframeColor, glm::vec3 verticesColor, glm::vec3 normalsColor, glm::vec3 boxColor, glm::vec3 vmin, glm::vec3 vmax)
	{
		this->meshes = meshes;
		this->path = path;
		this->name = name;
		this->normalize = normalize;
		this->scale = scale;
		this->translation = translation;
		this->rotationMat = rotationMat;
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

		initMatrices();
	}

	~Object()
	{
		meshes.clear();
	}

	// Initialize matrices
	void initMatrices()
	{
		normalizeMat = glm::scale(glm::mat4(1.0f), normalize);
		centerMat = glm::translate(glm::mat4(1.0f), center);
		scaleMat = glm::scale(glm::mat4(1.0f), scale);
		translationMat = glm::translate(glm::mat4(1.0f), translation);
		rotationMat =  glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void clampAngle(float &angle)
	{
		if(angle > 360.0f)
			angle -= 360.0f;
		else if(angle < -360.0f)
			angle += 360.0f;
	}

	void addXRot(float xoffset)
	{
		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians((float)xoffset), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMat = rotX * rotationMat;

		oldAngles.x = angles.x;
		angles.x += (float)xoffset;

		clampAngle(angles.x);
	}

	void addYRot(float yoffset)
	{
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians((float)yoffset), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMat = rotY * rotationMat;

		oldAngles.y = angles.y;
		angles.y += (float)yoffset;

		clampAngle(angles.y);
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

	void drawFlatPicking(Shader shader)
	{	
		int n = (int)meshes.size();
		for (int i = 0; i < n; i++)
		{
			shader.setVec3f("color", pickingColor);
			meshes[i].draw();
		}
	}

	void draw(LightingModel model, Shader shader)
	{
		glm::vec3 color;
		int n = (int)meshes.size();

		if (showWireframe)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0, -1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (int i = 0; i < n; i++)
			{
				shader.setVec3f("gMaterial.diffuseColor", wireframeColor);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}

		for (int i = 0; i < n; i++)
		{
			if (*meshes[i].getShow())
			{
				shader.setVec3f("gMaterial.diffuseColor", *meshes[i].mtl->getDiffuse());
				shader.setVec3f("gMaterial.ambientColor", *meshes[i].mtl->getAmbient());

				if (model == PHONG_L)
				{
					shader.setVec3f("gMaterial.diffuseColor", *meshes[i].mtl->getDiffuse());
					shader.setVec3f("gMaterial.specularColor", *meshes[i].mtl->getSpecular());
					shader.setFloat("gMaterial.shininess", *meshes[i].mtl->getShininess());
				}
			
				meshes[i].draw(shader);
			}
		}
	}
	
	void drawVertices()
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize((float)pointSize);

		int n = (int)meshes.size();
		for (int i = 0; i < n; i++)
			meshes[i].drawVertex();

		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

	void getInfo(std::ofstream &outfile)
	{
		outfile << "o " << name << "\n"
				<< "p " << path << "\n"
				<< "n " << normalize.x << " " << normalize.y << " " << normalize.z << "\n"
				<< "s " << scale.x << " " << scale.y << " " << scale.z << "\n"
				<< "t " << translation.x << " " << translation.y << " " << translation.z << "\n"
				<< "r " << rotationMat[0][0] << " " << rotationMat[0][1] << " " << rotationMat[0][2] << " " 
						<< rotationMat[1][0] << " " << rotationMat[1][1] << " " << rotationMat[1][2] << " "
						<< rotationMat[2][0] << " " << rotationMat[2][1] << " " << rotationMat[2][2] << " " 
						<< rotationMat[3][0] << " " << rotationMat[3][1] << " " << rotationMat[3][2] << "\n"
				<< "a " << angles.x << " " << angles.y << " " << angles.z << "\n"
				<< "rs " << showWireframe << " " << showVertices << " " << showNormals << "\n"
				<< "pt " << pointSize << "\n"
				<< "pc " << pickingColor.x << " " << pickingColor.y << " " << pickingColor.z << "\n"
				<< "wc " << wireframeColor.x << " " << wireframeColor.y << " " << wireframeColor.z << "\n"
				<< "vc " << verticesColor.x << " " << verticesColor.y << " " << verticesColor.z << "\n"
				<< "nc " << normalsColor.x << " " << normalsColor.y << " " << normalsColor.z << "\n";

		boundingBox.getInfo(outfile);

		int n = (int)meshes.size();
		for (int  i = 0; i < n; i++)
			meshes[i].getInfo(outfile);
	}

	std::vector<Mesh>* getMeshes()
	{
		return &meshes;
	}

	void setPlanarXYTextCoords()
	{	
		glm::vec3 vmin = boundingBox.vmin, vmax = boundingBox.vmax;
		float kx = vmax.x - vmin.x;
		float ky = vmax.y - vmin.y;

		int n = meshes.size();
		for(int i=0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for(int j = 0; j < m; j++)
			{	
				glm::vec3 pos = meshes[i].vertices[j].position;
				meshes[i].vertices[j].textCoord.x = (pos.x - vmin.x) / kx;
				meshes[i].vertices[j].textCoord.y = (pos.y - vmin.y) / ky;
 			}
			meshes[i].resetData();
		}
	}

	void setPlanarYZTextCoords()
	{	
		glm::vec3 vmin = boundingBox.vmin, vmax = boundingBox.vmax;
		float ky = vmax.y - vmin.y;
		float kz = vmax.z - vmin.z;

		int n = meshes.size();
		for(int i=0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for(int j = 0; j < m; j++)
			{	
				glm::vec3 pos = meshes[i].vertices[j].position;
				meshes[i].vertices[j].textCoord.x = (pos.y - vmin.y) / ky;
				meshes[i].vertices[j].textCoord.y = (pos.z - vmin.z) / kz;
 			}
			meshes[i].resetData();
		}
	}

	void setPlanarXZTextCoords()
	{	
		glm::vec3 vmin = boundingBox.vmin, vmax = boundingBox.vmax;
		float kx = vmax.x - vmin.x;
		float kz = vmax.z - vmin.z;

		int n = meshes.size();
		for(int i=0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for(int j = 0; j < m; j++)
			{	
				glm::vec3 pos = meshes[i].vertices[j].position;
				meshes[i].vertices[j].textCoord.x = (pos.x - vmin.x) / kx;
				meshes[i].vertices[j].textCoord.y = (pos.z - vmin.z) / kz;
 			}
			meshes[i].resetData();
		}
	}

	void setSphericalTextCoords()
	{	
		int n = meshes.size();
		for(int i=0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for(int j = 0; j < m; j++)
			{	
				glm::vec3 normal = glm::normalize(meshes[i].vertices[j].normal);
				meshes[i].vertices[j].textCoord.x = glm::clamp(atan2(normal.x, normal.z)/(2*PI) + 0.5, 0.0, 1.0);
				meshes[i].vertices[j].textCoord.y = glm::clamp(asin(normal.y)/PI + 0.5, 0.0, 1.0);
 			}
			meshes[i].resetData();
		}
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