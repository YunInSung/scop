#ifndef __PARSE_H__
#define __PARSE_H__

#include "common.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "texture.h"
#include <sstream>
#include <map>

struct Normal {
	float nx, ny, nz;
};
struct Vertex {
	float x, y, z;
};
struct TexCoord {
	float u, v;
};
struct Face {
	int vertexIndex;
	int normalIndex;
	int textureVertexIndex;
};
struct FaceVertices
{
	Vertex vertex;
	Normal normal;
	TexCoord texCoord;
};
struct Material {
    std::string name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess; 

    std::string ambientTexture;
    std::string diffuseTexture;
    std::string specularTexture;
    std::string shininessTexture;
};

enum TriangleCheck {
	LOG,
	TRIANGULATION,
	RETRY
};

CLASS_PTR(Parse)
class Parse {
	private:
		bool LoadFile(const std::string& filepath);
		bool mtlLoad(const std::string& mtlPath);
		void makeNormals();
		void makeTexCoord();
		bool triangulation(int faceIndex, int nonSetIndex, int criteriaIndex);
		void textureClipping(std::vector<int> index);
		TriangleCheck triangleCheck(int faceIndex, int&nonSetIndex);
		bool newTriangel(std::vector<TexCoord>& newTexCoords, std::vector<int>& idx, int exceptIdx, int faceIndex);
		void setNonSetIdx(const std::vector<Face>& face, const std::vector<TexCoord>& newTexCoords, const TexCoord& nonSetTexCoord, int faceIndex, int exceptIdx);
		bool inCriteriaIndexList(int idx);
		
		std::vector<TexCoord> retUVCoord(int idx1, int idx2);

		Parse() {}
		std::vector<Vertex> m_vertices;
		std::vector<Normal> m_vertexNormals;
		std::vector<std::vector<TexCoord>> m_textureVertices;
		std::vector<std::vector<Face>> m_face;
		std::vector<std::vector<FaceVertices>> m_faceVertices;
		std::vector<Material> m_materials;
		std::vector<int> m_criteriaIndexList;
	
	public:
		static ParseUPtr Load(const std::string& filepath);

		std::vector<Vertex> getVertices() const { return m_vertices; }
		std::vector<Normal> getVertexNormals() const { return m_vertexNormals; }
		std::vector<std::vector<TexCoord>> getTextureVertices() const { return m_textureVertices; }
		std::vector<std::vector<Face>> getFace() const { return m_face; }
		std::vector<Material> getMaterials() const { return m_materials; }
		std::unique_ptr<float[]> getVBO() const;
		std::unique_ptr<uint32_t[]> getIndices() const;
		~Parse();
};

#endif