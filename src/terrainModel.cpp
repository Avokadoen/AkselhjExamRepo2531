#include "terrainModel.hpp"

#include <iostream>
#include <fstream>




Terrain::Terrain(float maxHeight, float blockScale) {
	this->maxHeight = maxHeight;
	this->blockScale = blockScale;
	yOffset = 0.0f;
	yScale = 1.0f;
}

bool Terrain::generateHeightValues() {

	int index = 0;
	if (imageData) {

		// While imagedata is pointing on data
		// iterate through image
		for (int y = 0; y < imageHeight; y++) {
			for (int x = 0; x < imageWidth; x++) {
				heightValues.push_back(GetHeightValue(imageData + (x + (imageWidth * y)) * nrComponents));
			}
		}
		return true;
	}
	else {
		return false;
	}
}

float Terrain::GetHeightValue(const unsigned char* data)
{
	
	switch (nrComponents)
	{
	case 1:
	{
		return (unsigned char)(data[0]) / (float)0xff;
	}
	break;
	case 2:
	{
		return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
	}
	break;
	case 3:
	{
		return (unsigned int)(data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffff;
	}
	break;
	case 4:
	{
		return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
	}
	break;
	default:
	{
		assert(false);  // Height field with non standard pixel sizes
	}
	break;
	}

	return 0.0f;
	

}


bool Terrain::loadHeightMapData(const char *path)
{

	unsigned int textureID;
	glGenTextures(1, &textureID);

	imageData = stbi_load(path, &imageWidth, &imageHeight, &nrComponents, 0);


	if(!imageData)
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(imageData);
		return false;
	}
	if (!generateHeightValues()) {
		std::cout << "failed to generate height map" << std::endl;
		return false;
	}
	//imageData = data;
	return true;
}

void Terrain::freeImageData() {
	stbi_image_free(imageData);
}

void Terrain::createTerrainMesh(float meshMaxHeight, float meshMinHeight, glm::vec3 primaryColor) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {

			glm::vec3 data(0.0f);
			data.x = x * blockScale;
			data.y = maxHeight * heightValues[(imageWidth*y)+x];
			data.z = y * blockScale;

			if (heightValues[(imageWidth*y) + x] > 0.5 || heightValues[(imageWidth*y) + x] < 0.05) std::cout << heightValues[(imageWidth*y) + x] << " ";

			Vertex vboData;
			vboData.Position	= data;
			vboData.Normal		= glm::vec3(0.0f);
			vboData.TexCoords	= glm::vec2(0.0f);

			vertices.push_back(vboData);
		}
	}
	indices = generateIndices();
	std::vector<glm::vec3> normals = generateNormals(vertices, indices);

	for (int i = 0; i < vertices.size(); i += 3) {
		vertices[indices[i]].Normal = normals[i];
		vertices[indices[i + 1]].Normal = normals[i];
		vertices[indices[i + 2]].Normal = normals[i];
	}

	Mesh mesh(vertices, indices);
	meshes.push_back(mesh);
}

std::vector<unsigned int> Terrain::generateIndices() {
	

	// 2 triangles for every quad of the terrain mesh
	const unsigned int numTriangles = (imageWidth - 1) * (imageHeight - 1) * 2;

	// 3 indices for each triangle in the terrain mesh
	std::vector<unsigned int> indices;
	//indices.resize(numTriangles * 3);

	unsigned int index = 0; // Index in the index buffer
	for (unsigned int y = 0; y < (imageHeight - 1); y++)
	{
		for (unsigned int x = 0; x < (imageWidth - 1); x++)
		{
			int vertexIndex = (y * imageWidth) + x;
			// Top triangle (T0)
			indices.push_back(vertexIndex);                           // V0
			indices.push_back(vertexIndex + imageWidth);		// V3
			indices.push_back(vertexIndex + 1);                      // V1
																	// Bottom triangle (T1)
			indices.push_back(vertexIndex + 1);                           // V0
			indices.push_back(vertexIndex + imageWidth + 1);            // V2
			indices.push_back(vertexIndex + imageWidth);        // V3
		}
	}
	return indices;
}

std::vector<glm::vec3>	Terrain::generateNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
	std::vector<glm::vec3> normals;
	normals.resize(vertices.size());
	for (unsigned int i = 0; i < vertices.size(); i += 3)
	{
		glm::vec3 v0 = vertices[indices[i + 0]].Position;
		glm::vec3 v1 = vertices[indices[i + 1]].Position;
		glm::vec3 v2 = vertices[indices[i + 2]].Position;

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		normals[indices[i + 0]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}

	for(auto&& normal : normals){
		normal = glm::normalize(normal);
	}

	return normals;
}

void Terrain::bindMaterialsToShader(Shader shader) {
	shader.setFloat("maxHeight", maxHeight);
	shader.setFloat("yScale", yScale);
	shader.setFloat("yOffset", yOffset);

	shader.setVec3("snow.ambient", snow.ambient);
	shader.setVec3("snow.diffuse", snow.diffuse);
	shader.setVec3("snow.specular", snow.specular);
	shader.setFloat("snow.shininess", snow.shininess);
	shader.setFloat("snowBottom", SnowBottom);
	shader.setFloat("snowTop", SnowTop);

	shader.setVec3("stone.ambient", stone.ambient);
	shader.setVec3("stone.diffuse", stone.diffuse);
	shader.setVec3("stone.specular", stone.specular);
	shader.setFloat("stone.shininess", stone.shininess);
	shader.setFloat("stoneBottom", StoneBottom);
	shader.setFloat("stoneTop", StoneTop);

	shader.setVec3("grass.ambient", grass.ambient);
	shader.setVec3("grass.diffuse", grass.diffuse);
	shader.setVec3("grass.specular", grass.specular);
	shader.setFloat("grass.shininess", grass.shininess);
	shader.setFloat("grassBottom", GrassBottom);
	shader.setFloat("grassTop", GrassTop);

	shader.setVec3("mud.ambient", mud.ambient);
	shader.setVec3("mud.diffuse", mud.diffuse);
	shader.setVec3("mud.specular", mud.specular);
	shader.setFloat("mud.shininess", mud.shininess);
	shader.setFloat("mudBottom", MudBottom);
	shader.setFloat("mudTop", MudTop);

	shader.setVec3("water.ambient", water.ambient);
	shader.setVec3("water.diffuse", water.diffuse);
	shader.setVec3("water.specular", water.specular);
	shader.setFloat("water.shininess", water.shininess);
	shader.setFloat("waterTop", WaterTop);

}

void Terrain::scale(glm::vec3 scale) {
	Model::scale(scale);

	// save scale for color placement in shader
	yScale = scale.y;
}

void Terrain::scale(float scale) {
	Model::scale(scale);

	// save scale for color placement in shader
	yScale = scale;
}

void Terrain::translate(glm::vec3 offset) {
	Model::translate(offset);

	yOffset = offset.y;
}