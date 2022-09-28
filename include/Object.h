#pragma once
class Object
{
public:
	Object() = default;

	Object(std::vector<Mesh> meshes, std::string path)
	{
		name = "";
		this->path = path;
		this->meshes = meshes;
	
		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, size_x, size_y, size_z, scaleFactor;

		min_x = max_x = meshes[0].vertices[0].position.x;
		min_y = max_y = meshes[0].vertices[0].position.y;
		min_z = max_z = meshes[0].vertices[0].position.z;

		int n = meshes.size();
		for (int i = 0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
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

		// Initialize model matrix components
		translation = glm::vec3(0.0, 0.0, 0.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		angles = glm::vec3(0.0f, 0.0f, 0.0f);

		// Normalize mesh into unitary parallelepiped
		scaleFactor = std::max(std::max(max_x - min_x, max_y - min_y), max_z - min_z);
		normalize = glm::vec3(1/ scaleFactor, 1/ scaleFactor, 1/ scaleFactor);

		// Create bounding box
		boundingBox = OBB(glm::vec3(min_x, min_y, min_z), glm::vec3(max_x, max_y, max_z), { 1.0, 1.0, 1.0 });
	}

	Object(std::vector<Mesh> meshes, std::string path, std::string name, glm::vec3 normalize, glm::vec3 scale, glm::vec3 translation, glm::vec3 angles,
		bool useDepthTest, bool useCullFace, bool useMultisample, bool showWireframe, bool showVertices, bool showNormals,
		int pointSize, glm::vec3 wireframeColor, glm::vec3 verticesColor, glm::vec3 normalsColor, glm::vec3 boxColor, glm::vec3 vmin, glm::vec3 vmax)
	{
		this->meshes = meshes;
		this->path = path;
		this->name = name;
		this->normalize = normalize;
		this->scale = scale;
		this->translation = translation;
		this->angles = angles;

		this->useDepthTest = useDepthTest;
		this->useCullFace = useCullFace;
		this->useMultisample = useMultisample;
		this->showWireframe = showWireframe;
		this->showVertices = showVertices;
		this->showNormals = showNormals;

		this->pointSize = pointSize;
		this->wireframeColor = wireframeColor;
		this->verticesColor = verticesColor;
		this->normalsColor = normalsColor;

		this->boundingBox = OBB(vmin, vmax, boxColor);

	}

	void setName(std::string name) 
	{
		this->name = name;
	}

	void addXRot(float xoffset)
	{
		angles.x += xoffset;
	}

	void addYRot(float yoffset)
	{
		angles.y += yoffset;
	}

	void changeXTranslation(float newXTranslation) {
		float xoffset = newXTranslation - translation.x;
		std::cout << boundingBox.center.x << " " << xoffset << std::endl;
 		translation.x = newXTranslation;
		boundingBox.center.x = newXTranslation;
	}

	std::string getName()
	{
		return name;
	}

	glm::vec3* getScaleFactor()
	{
		return &scale;
	}

	glm::vec3* getTranslationFactor()
	{
		return &translation;
	}

	glm::vec3* getRotationFactor()
	{
		return &angles;
	}

	glm::mat4 getModelTransformation()
	{
		return
			glm::translate(glm::mat4(1.0f), translation) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), scale)* 
			glm::scale(glm::mat4(1.0f), normalize);
	}

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

	bool* getZBufferBool()
	{
		return &useDepthTest;
	}

	bool* getCullFaceBool()
	{
		return &useCullFace;
	}

	bool* getWireframeBool()
	{
		return &showWireframe;
	}

	bool* getVerticesBool()
	{
		return &showVertices;
	}

	bool* getNormalsBool()
	{
		return &showNormals;
	}

	bool* getMultisampleBool()
	{
		return &useMultisample;
	}

	glm::vec3* getWireframeColor()
	{
		return &wireframeColor;
	}

	glm::vec3* getVerticesColor()
	{
		return &verticesColor;
	}

	void draw(int colorLoc)
	{
		glm::vec3 color;

		if (useDepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (useCullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		if (useMultisample)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		for (int i = 0; i < meshes.size(); i++)
		{	
			if (*meshes[i].getShow())
			{
				color = *meshes[i].mtl->getDiffuse();
				glUniform3f(colorLoc, color.x, color.y, color.z);
				meshes[i].draw();
			}

		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		if (showVertices)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glPointSize(pointSize);
			for (int i = 0; i < meshes.size(); i++)
			{
				glUniform3f(colorLoc, verticesColor.x, verticesColor.y, verticesColor.z);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (showWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (int i = 0; i < meshes.size(); i++)
			{
				glUniform3f(colorLoc, wireframeColor.x, wireframeColor.y, wireframeColor.z);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	
	void drawNormals()
	{
		for (int i = 0; i < meshes.size(); i++)
			meshes[i].drawVertex();
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

	bool TestRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance) 
	{
		float tMin = 0.0f;
		float tMax = std::numeric_limits<double>::infinity();

		glm::mat4 ModelMatrix = getModelTransformation();

		glm::vec3 min = glm::vec3(ModelMatrix * glm::vec4(boundingBox.vmin, 1.0));
		glm::vec3 max = glm::vec3(ModelMatrix * glm::vec4(boundingBox.vmax, 1.0));
		glm::vec3 size = glm::vec3((max.x - min.x) / 2, (max.y - min.y) / 2, (max.z - min.z) / 2);
		glm::vec3 center = glm::vec3(ModelMatrix * glm::vec4(boundingBox.center, 1.0f));
		
		glm::vec3 delta = center - ray_origin; //vector pointing from the origin of the ray to the OBB,

		std::cout << size.x << " " << size.y << " " << size.z << std::endl;

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		{
			glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
			float e = glm::dot(xaxis, delta);
			float f = glm::dot(ray_direction, xaxis);

			if (fabs(f) > 0.001f) { // Standard case

				float t1 = (e + size.y) / f; // Intersection with the "left" plane
				float t2 = (e - size.y) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				// We want t1 to represent the nearest intersection, 
				// so if it's not the case, invert t1 and t2
				if (t1 > t2) {
					float w = t1; t1 = t2; t2 = w; // swap t1 and t2
				}

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;
				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin)
					tMin = t1;

				// And here's the trick :
				// If "far" is closer than "near", then there is NO intersection.
				// See the images in the tutorials for the visual explanation.
				if (tMax < tMin)
					return false;

			}
			else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + size.y > 0.0f || -e + size.y < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		// Exactly the same thing than above.
		{
			glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
			float e = glm::dot(yaxis, delta);
			float f = glm::dot(ray_direction, yaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + size.x) / f;
				float t2 = (e - size.x) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + size.x > 0.0f || -e + size.x < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		// Exactly the same thing than above.
		{
			glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
			float e = glm::dot(zaxis, delta);
			float f = glm::dot(ray_direction, zaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + size.y) / f;
				float t2 = (e - size.y) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + size.y > 0.0f || -e + size.y < 0.0f)
					return false;
			}
		}

		intersection_distance = tMin;
		return true;
	}

	void getInfo(std::ofstream &outfile)
	{
		outfile << "o " << name << "\n";
		outfile << "p " << path << "\n";
		outfile << "n " << normalize.x << " " << normalize.y << " " << normalize.z << "\n";
		outfile << "s " << scale.x << " " << scale.y << " " << scale.z << "\n";
		outfile << "t " << translation.x << " " << translation.y << " " << translation.z << "\n";
		outfile << "a " << angles.x << " " << angles.y << " " << angles.z << "\n";
		outfile << "rs " << useDepthTest << " " << useCullFace << " " << useMultisample
				<< " " << showWireframe << " " << showVertices << " " << showNormals << "\n";
		outfile << "pt " << pointSize << "\n";
		outfile << "wc " << wireframeColor.x << " " << wireframeColor.y << " " << wireframeColor.z << "\n";
		outfile << "vc " << verticesColor.x << " " << verticesColor.y << " " << verticesColor.z << "\n";
		outfile << "nc " << normalsColor.x << " " << normalsColor.y << " " << normalsColor.z << "\n";
		boundingBox.getInfo(outfile);

		for (int i = 0; i < meshes.size(); i++)
			meshes[i].getInfo(outfile);
	}

	std::vector<Mesh>* getMeshes()
	{
		return &meshes;
	}

	// Model Tranformation
	glm::vec3 normalize, scale, translation, angles;

	// Bounding box
	OBB boundingBox;
private:
	// Object basic components
	std::string name, path;
	std::vector<Mesh> meshes;

	// Rendering State
	bool useDepthTest = true;
	bool useCullFace = true;
	bool useMultisample = true;
	bool showWireframe = false;
	bool showVertices = false; int pointSize = 3;
	bool showNormals = false;

	// Colors
	glm::vec3 wireframeColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 verticesColor = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 normalsColor = glm::vec3(0.0f, 1.0f, 0.0f);
};

struct ObjectDistance
{
	Object* obj;
	float intersectionDistance;
	int index;

	ObjectDistance() = default;

	ObjectDistance(Object* obj, float intersectionDistance, int index)
	{
		this->obj = obj;
		this->intersectionDistance = intersectionDistance;
		this->index = index;
	}
};

std::vector<Object> objects;
bool draw = false;
Object *selectedObject;