#include "parse.h"

void printFaceVertex(std::vector<FaceVertices> faceVertex, int idx)
{
	std::cout.width(6);
	std::cout << std::right << "vertices";
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].vertex.x;
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].vertex.y;
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].vertex.z << std::endl;
	std::cout.width(6);
	std::cout << std::right << "normals";
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].normal.nx;
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].normal.ny;
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].normal.nz << std::endl;
	std::cout.width(6);
	std::cout << std::right << "Textures";
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].texCoord.u;
	std::cout.width(9);
	std::cout << std::right << faceVertex[idx].texCoord.v << std::endl;
}

void printNowIdx(std::vector<std::vector<FaceVertices>> m_faceVertices, std::vector<std::vector<Face>> m_face, int nowIdx) 
{
	std::cout.width(3);
	std::cout << std::right << "faceIdx: " << nowIdx << std::endl;
	for (int j = 0; j < 3; j++)
	{
		std::vector<FaceVertices>& faceVertex = m_faceVertices[nowIdx];
		std::cout << m_face[nowIdx][j].vertexIndex << std::endl;
		printFaceVertex(faceVertex, j);
	}
	std::cout << std::endl;
}

void printFaceVertices(std::vector<std::vector<FaceVertices>> m_faceVertices, std::vector<std::vector<Face>> m_face) 
{
	for (int i = 0 ; i < m_faceVertices.size() ; i++)
	{
		printNowIdx(m_faceVertices, m_face, i);
	}
}

bool equalCheck(float a, float b)
{
	if (abs(a - b) < 0.0001)
		return true;
	return false;
}

float dotProduct(Vertex v1, Vertex v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float dotProduct(TexCoord v1, TexCoord v2)
{
	return (v1.u * v2.u + v1.v * v2.v);
}

float distance(Vertex v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float distance(TexCoord v)
{
	return sqrt(v.u * v.u + v.v * v.v);
}

bool equalTriangleCheck(Vertex vec1, Vertex vec2, TexCoord vec1_2D, TexCoord vec2_2D)
{
	const float dist1 = distance(vec1);
	const float dist2 = distance(vec2);
	const float dist1_2D = distance(vec1_2D);
	const float dist2_2D = distance(vec2_2D);
	if (equalCheck(dist1, dist1_2D) && equalCheck(dist2, dist2_2D) && \
		equalCheck(dotProduct(vec1, vec2), dotProduct(vec1_2D, vec2_2D)))
		return true;
	return false;
}

std::vector<TexCoord> Parse::retUVCoord(int idx1, int idx2) {
	const Vertex& p1 = m_vertices[idx1];
	const Vertex& p2 = m_vertices[idx2];
	const Vertex vec12 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
	const float dist12 = distance(vec12);
	std::vector<TexCoord> ret;
	for (int i = 0 ; i < m_textureVertices[idx1].size() ; i++) {
		for (int j = 0 ; j < m_textureVertices[idx2].size() ; j++) {
			TexCoord& t1 = m_textureVertices[idx1][i];
			TexCoord& t2 = m_textureVertices[idx2][j];
			TexCoord vec12_2D = {t2.u - t1.u, t2.v - t1.v};
			// std::cout << "t1 idx : " << idx1 << ", " << i << "  " << "t2 idx : " << idx2 << ", " << j << std::endl;
			// std::cout << "dist12 : " << dist12 << "  " << "dist12_2D : " << distance(vec12_2D) << std::endl;
			if (equalCheck(distance(vec12_2D), dist12) == true) {
				ret.push_back(m_textureVertices[idx1][i]);
				ret.push_back(m_textureVertices[idx2][j]);
				return ret;
			}
		}
	}
	return ret;
}

bool Parse::triangulation(int faceIndex, int nonSetIndex, int criteriaIndex) {
	const std::vector<Face>& face = m_face[faceIndex];
	std::vector<int> index(2, 0);
	if (nonSetIndex == 0) {
		index[0] = 1;
		index[1] = 2;
	}
	else if (nonSetIndex == 1) {
		index[0] = 2;
		index[1] = 0;
	}
	else if (nonSetIndex == 2) {
		index[0] = 0;
		index[1] = 1;
	}
	const Vertex& p1 = m_vertices[face[index[0]].vertexIndex];
	const Vertex& p2 = m_vertices[face[index[1]].vertexIndex];
	const Vertex& p3 = m_vertices[face[nonSetIndex].vertexIndex];
	const Vertex vec12 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
	const Vertex vec13 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };
	const float dist12 = distance(vec12);
	const float dist13 = distance(vec13);

	std::vector<TexCoord> UVCoordList = retUVCoord(face[index[0]].vertexIndex, face[index[1]].vertexIndex);
	if (UVCoordList.size() == 0) {
		return false;
	}
	const TexCoord& t1 = UVCoordList[0];
	const TexCoord& t2 = UVCoordList[1];

	const TexCoord vec12_2D = {t2.u - t1.u, t2.v - t1.v};
	const float dist12_2D = distance(vec12_2D);
	const TexCoord normal_2D = {vec12_2D.u / dist12_2D, vec12_2D.v / dist12_2D};
	const float cosine = dotProduct(vec12, vec13) / (dist12 * dist13);
	const float sine = equalCheck(cosine, 1.0f) ? 0 : sqrt(1 - cosine * cosine);
	const TexCoord newTexCoord = {(cosine * normal_2D.u + sine * normal_2D.v) * dist13 + t1.u, 
								  (-sine * normal_2D.u + cosine * normal_2D.v) * dist13 + t1.v};

	if (face[nonSetIndex].vertexIndex != criteriaIndex && \
		m_textureVertices[face[nonSetIndex].vertexIndex][0].u == 0.0f && m_textureVertices[face[nonSetIndex].vertexIndex][0].v == 0.0f)
		m_textureVertices[face[nonSetIndex].vertexIndex][0] = newTexCoord;
	else
		m_textureVertices[face[nonSetIndex].vertexIndex].push_back(newTexCoord);

	FaceVertices faceVertices;
	setNonSetIdx(face, UVCoordList, newTexCoord, faceIndex, nonSetIndex);
	return true;
}

void Parse::textureClipping(std::vector<int> index) {
	TexCoord highCoord = {0.0f, 0.0f};
	TexCoord lowCoord = {0.0f, 0.0f};
	for (const int& idx : index)
	{
		std::vector<FaceVertices>& faceVertices = m_faceVertices[idx];
		for (int i = 0 ; i < 3 ; i++)
		{
			TexCoord& coord = m_faceVertices[idx][i].texCoord;
			if (coord.u > highCoord.u)
				highCoord.u = coord.u;
			if (coord.v > highCoord.v)
				highCoord.v = coord.v;
			if (coord.u < lowCoord.u)
				lowCoord.u = coord.u;
			if (coord.v < lowCoord.v)
				lowCoord.v = coord.v;
		}
	}

	const float width = highCoord.u - lowCoord.u;
	const float height = highCoord.v - lowCoord.v;

	for (const int& idx : index)
	{
		std::vector<FaceVertices>& faceVertices = m_faceVertices[idx];

		for (int i = 0 ; i < 3 ; i++)
		{
			TexCoord& coord = m_faceVertices[idx][i].texCoord;
			coord.u = (coord.u - lowCoord.u) / width;
			coord.v = (coord.v - lowCoord.v) / height;
		}
	}
}

void Parse::makeNormals() {
	// if (m_vertexNormals.size() > 0)
	// {
	// 	std::vector<Normal> newNormals(m_vertices.size(), {0.0f, 0.0f, 0.0f});
	// 	for (int i = 0 ; i < m_face.size() ; i++) {
	// 		if (m_face[i].size() < 3) {
	// 			continue;
	// 		}
	// 		const std::vector<Face>& face = m_face[i];
	// 		for (int j = 0 ; j < face.size() ; j++) {
	// 			newNormals[face[j].vertexIndex] = m_vertexNormals[face[j].normalIndex];
	// 		}
	// 	}
	// 	m_vertexNormals = newNormals;
	// 	return ;
	// }
	m_vertexNormals.clear();
	m_vertexNormals.assign(m_vertices.size(), {0.0f, 0.0f, 0.0f});
	for (const std::vector<Face>& face : m_face)
	{
		if (face.size() < 3)
			continue;
		
		const Vertex& p1 = m_vertices[face[0].vertexIndex];
		const Vertex& p2 = m_vertices[face[1].vertexIndex];
		const Vertex& p3 = m_vertices[face[2].vertexIndex];

		const Vertex vec1 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
		const Vertex vec2 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };

		Normal normal = { 
							vec1.y * vec2.z - vec1.z * vec2.y,
							vec1.z * vec2.x - vec1.x * vec2.z,
							vec1.x * vec2.y - vec1.y * vec2.x
						};
		const float length = sqrt(normal.nx * normal.nx + normal.ny * normal.ny + normal.nz * normal.nz);
		for (const Face& faceVertex : face) {
			m_vertexNormals[faceVertex.vertexIndex].nx += (normal.nx / length);
			m_vertexNormals[faceVertex.vertexIndex].ny += (normal.ny / length);
			m_vertexNormals[faceVertex.vertexIndex].nz += (normal.nz / length);
    	}
	}
	for (Normal& normal : m_vertexNormals) {
		const float length = sqrt(normal.nx * normal.nx + normal.ny * normal.ny + normal.nz * normal.nz);
		if (length > 0) {
			normal.nx /= length;
			normal.ny /= length;
			normal.nz /= length;
		}
	}
}

ParseUPtr Parse::Load(const std::string& filepath) {
	auto parse = ParseUPtr(new Parse());
	if (!parse->LoadFile(filepath))
		return nullptr;
	return std::move(parse);
}


bool Parse::LoadFile(const std::string& filepath)
{
	std::ifstream file(filepath.data());
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
		return false;
	}
	std::string newline;
	while (std::getline(file, newline)) {
		std::istringstream lineStream(newline);
		std::string token;

		lineStream >> token;
		if (token == "v") {
			Vertex vertex;
			lineStream >> vertex.x >> vertex.y >> vertex.z;
			m_vertices.push_back(vertex);
		}
		else if (token == "vt") {
			TexCoord texCoord;
			lineStream >> texCoord.u >> texCoord.v;
			m_textureVertices.push_back(std::vector<TexCoord>(1, texCoord));
		}
		else if (token == "vn") {
			Normal normal;
			lineStream >> normal.nx >> normal.ny >> normal.nz;
			m_vertexNormals.push_back(normal);
		}
		else if (token == "f") {
			std::string buffer;
			std::vector<Face> face;
			while (lineStream >> buffer)
			{
				std::istringstream bufferStream(buffer);
				std::string tmpBuffer;
				Face faceBuffer;
				size_t idx = 0;
				while (getline(bufferStream, tmpBuffer, '/'))
				{
					int index = -1;
					if (buffer != "")
						index = std::stoi(buffer) - 1;
					if (idx == 0)
						faceBuffer.vertexIndex = index;
					else if (idx == 1)
						faceBuffer.normalIndex = index;
					else if (idx == 2)
						faceBuffer.textureVertexIndex = index;
					idx++;
				}
				face.push_back(faceBuffer);
			}
			if (face.size() == 3)
				m_face.push_back(face);
			else if (face.size() == 4)
			{
				m_face.push_back(std::vector<Face>({face[0], face[1], face[2]}));
				m_face.push_back(std::vector<Face>({face[0], face[2], face[3]}));
			}
		}
		else if (token == "mtllib") {
			std::string mtlPath;
			lineStream >> mtlPath;
			if (!mtlLoad(mtlPath))
				return false;
		}
	}
	file.close();
	makeNormals();
	makeTexCoord();
	return true;
}

bool Parse::mtlLoad(const std::string& mtlPath)
{
    Material currentMaterial;

    std::ifstream file("./resource/" + mtlPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << mtlPath << std::endl;
        return false;
    }

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token;

		if (token == "newmtl") {
            // Start a new material
            if (!currentMaterial.name.empty()) {
                // Store the previous material
                m_materials.push_back(currentMaterial);
            }
            // Initialize the new material
            currentMaterial = Material();
            lineStream >> currentMaterial.name;
        }
		else if (token == "Ka") {
            lineStream >> currentMaterial.ambient.x >> currentMaterial.ambient.y >> currentMaterial.ambient.z;
        }
		else if (token == "Kd") {
            lineStream >> currentMaterial.diffuse.x >> currentMaterial.diffuse.y >> currentMaterial.diffuse.z;
        } 
		else if (token == "Ks") {
            lineStream >> currentMaterial.specular.x >> currentMaterial.specular.y >> currentMaterial.specular.z;
        } 
		else if (token == "Ns") {
            lineStream >> currentMaterial.shininess;
        } 
		else if (token == "map_Ka") {
            lineStream >> currentMaterial.ambientTexture;
        } 
		else if (token == "map_Kd") {
            lineStream >> currentMaterial.diffuseTexture;
        } 
		else if (token == "map_Ks") {
            lineStream >> currentMaterial.specularTexture;
        } 
		else if (token == "map_Ns") {
            lineStream >> currentMaterial.shininessTexture;
        }
        // You can add more material properties as needed
    }

    // Store the last material
    if (!currentMaterial.name.empty()) {
        m_materials.push_back(currentMaterial);
    }

    return true;
}

std::unique_ptr<float[]> Parse::getVBO() const {
	const size_t size = m_faceVertices.size();
	auto VBO = std::unique_ptr<float[]>(new float[size * 3 * 8]);
	for (int idx = 0 ; idx < size ; idx++)
	{
		for(int idx2 = 0 ; idx2 < 3 ; idx2++)
		{
			VBO[(idx * 8 * 3) + (idx2 * 8)] = m_faceVertices[idx][idx2].vertex.x;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 1] = m_faceVertices[idx][idx2].vertex.y;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 2] = m_faceVertices[idx][idx2].vertex.z;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 3] = m_faceVertices[idx][idx2].normal.nx;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 4] = m_faceVertices[idx][idx2].normal.ny;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 5] = m_faceVertices[idx][idx2].normal.nz;
			VBO[(idx * 8 * 3) + (idx2 * 8) + 6] = m_faceVertices[idx][idx2].texCoord.u; 
			VBO[(idx * 8 * 3) + (idx2 * 8) + 7] = m_faceVertices[idx][idx2].texCoord.v;
		}
	}
	return std::move(VBO);
}

std::unique_ptr<uint32_t[]> Parse::getIndices() const {
	const uint32_t size = m_face.size();
	auto indices = std::unique_ptr<uint32_t[]>(new uint32_t[size * 3]);
	for (int i = 0 ; i < size ; i++)
	{
		for (int j = 0 ; j < 3 ; j++)
		{
			indices[(i * 3) + j] = m_face[i][j].vertexIndex;
		}
	}
	return std::move(indices);
}

void Parse::setNonSetIdx(const std::vector<Face>& face, const std::vector<TexCoord>& newTexCoords, const TexCoord& nonSetTexCoord, int faceIndex, int exceptIdx)
{
	if (exceptIdx == 0) {
		m_faceVertices[faceIndex].push_back({ m_vertices[face[0].vertexIndex], m_vertexNormals[face[0].vertexIndex], nonSetTexCoord });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[1].vertexIndex], m_vertexNormals[face[1].vertexIndex], newTexCoords[0] });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[2].vertexIndex], m_vertexNormals[face[2].vertexIndex], newTexCoords[1] });
	}
	else if (exceptIdx == 1) {
		m_faceVertices[faceIndex].push_back({ m_vertices[face[0].vertexIndex], m_vertexNormals[face[0].vertexIndex], newTexCoords[1] });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[1].vertexIndex], m_vertexNormals[face[1].vertexIndex], nonSetTexCoord });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[2].vertexIndex], m_vertexNormals[face[2].vertexIndex], newTexCoords[0] });
	}
	else if (exceptIdx == 2) {
		m_faceVertices[faceIndex].push_back({ m_vertices[face[0].vertexIndex], m_vertexNormals[face[0].vertexIndex], newTexCoords[0] });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[1].vertexIndex], m_vertexNormals[face[1].vertexIndex], newTexCoords[1] });
		m_faceVertices[faceIndex].push_back({ m_vertices[face[2].vertexIndex], m_vertexNormals[face[2].vertexIndex], nonSetTexCoord });
	}
}

bool Parse::newTriangel(std::vector<TexCoord>& newTexCoords, std::vector<int>& idx, int exceptIdx, int faceIndex) {
	const std::vector<Face>& face = m_face[faceIndex];
	const Vertex& p1 = m_vertices[face[idx[0]].vertexIndex];
	const Vertex& p2 = m_vertices[face[idx[1]].vertexIndex];
	const Vertex& p3 = m_vertices[face[exceptIdx].vertexIndex];
	const Vertex vec12 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
	const Vertex vec13 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };
	const TexCoord& newT1 = newTexCoords[0];
	const TexCoord& newT2 = newTexCoords[1];
	for (int i = 0 ; i < m_textureVertices[face[2].vertexIndex].size() ; i++)
	{
		const TexCoord& newT3 = m_textureVertices[face[exceptIdx].vertexIndex][i];
		const TexCoord newVec12_2D = { newT2.u - newT1.u, newT2.v - newT1.v };
		const TexCoord newVec13_2D = { newT3.u - newT1.u, newT3.v - newT1.v };
		if (equalTriangleCheck(vec12, vec13, newVec12_2D, newVec13_2D)) {
			setNonSetIdx(face, newTexCoords, m_textureVertices[face[exceptIdx].vertexIndex][i], faceIndex, exceptIdx);
			return true;
		}
	}
	return false;
}

TriangleCheck Parse::triangleCheck(int faceIndex, int& nonSetIndex) {
	const std::vector<Face>& face = m_face[faceIndex];
	const Vertex& p1 = m_vertices[face[0].vertexIndex];
	const Vertex& p2 = m_vertices[face[1].vertexIndex];
	const Vertex& p3 = m_vertices[face[2].vertexIndex];
	const Vertex vec12 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
	const Vertex vec13 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };

	const TexCoord t1 = m_textureVertices[face[0].vertexIndex][0];
	const TexCoord t2 = m_textureVertices[face[1].vertexIndex][0];
	const TexCoord t3 = m_textureVertices[face[2].vertexIndex][0];
	const TexCoord vec12_2D = { t2.u - t1.u, t2.v - t1.v };
	const TexCoord vec13_2D = { t3.u - t1.u, t3.v - t1.v };

	if (equalTriangleCheck(vec12, vec13, vec12_2D, vec13_2D)){
		for (int j = 0 ; j < 3 ; j++) {
			FaceVertices faceVertices;
			faceVertices = {m_vertices[face[j].vertexIndex], m_vertexNormals[face[j].vertexIndex], m_textureVertices[face[j].vertexIndex][0]};
			m_faceVertices[faceIndex].push_back(faceVertices);
		}
		return LOG;
	}
	std::vector<TexCoord> newTexCoords;
	for (int nonSetIdx = 0 ; nonSetIdx < 3 ; nonSetIdx++) {
		std::vector<int> idx(2, 0);
		if (nonSetIdx == 0) {
			idx[0] = 1;
			idx[1] = 2;
		}
		else if (nonSetIdx == 1) {
			idx[0] = 2;
			idx[1] = 0;
		}
		else if (nonSetIdx == 2) {
			idx[0] = 0;
			idx[1] = 1;
		}
		newTexCoords = retUVCoord(face[idx[0]].vertexIndex, face[idx[1]].vertexIndex);
		if (newTexCoords.size() != 0)
		{
			if (newTriangel(newTexCoords, idx, nonSetIdx, faceIndex))
				return LOG;
			nonSetIndex = nonSetIdx;
			return TRIANGULATION;
		}
	}
	return RETRY;
}

bool Parse::inCriteriaIndexList(int idx)
{
	if (find(m_criteriaIndexList.begin(), m_criteriaIndexList.end(), idx) == m_criteriaIndexList.end())
		return false;
	return true;
}

void Parse::makeTexCoord() {
	// if (m_textureVertices.size() > 0)
	// {
	// 	for (int idx = 0 ; idx < m_face.size() ; idx++) {
	// 		std::vector<Face>& face = m_face[idx];
	// 		if (face.size() < 3)
	// 			continue;
	// 		std::vector<FaceVertices> faceVertices;
	// 		std::cout<< idx << std::endl;
	// 		for (int i = 0 ; i < face.size() ; i++) {
	// 			std::cout << i << " ";
	// 			faceVertices.push_back({m_vertices[face[i].vertexIndex], m_vertexNormals[face[i].vertexIndex],
	// 										{0.0f, 0.0f}});
	// 		}
	// 		m_faceVertices.push_back(faceVertices);
	// 	}
	// 	return ;
	// }
	m_textureVertices.clear();
	m_textureVertices.assign(m_vertices.size(), std::vector<TexCoord>(1, {0.0f, 0.0f}));
	m_faceVertices.clear();
	m_faceVertices.assign(m_face.size(), std::vector<FaceVertices>());

	std::vector<int> faceCount;
	for (int i = 0 ; i < m_face.size() ; i++) {
		faceCount.push_back(i);
	}

	while (faceCount.size() != 0) {
		std::vector<Face>& criteriaFace = m_face[faceCount[0]];
		int criteriaIndex = criteriaFace[0].vertexIndex;
		m_criteriaIndexList.push_back(criteriaIndex);
		Vertex& p1 = m_vertices[criteriaIndex];
		Vertex& p2 = m_vertices[criteriaFace[1].vertexIndex];
		Vertex vec12 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
		// std::cout << "retUVCoord" << std::endl;
		if (retUVCoord(criteriaIndex, criteriaFace[1].vertexIndex).size() == 0) {
			if ((m_textureVertices[criteriaFace[1].vertexIndex].size() == 1 && !inCriteriaIndexList(criteriaFace[1].vertexIndex)) && m_textureVertices[criteriaFace[1].vertexIndex][0].u == 0.0f && m_textureVertices[criteriaFace[1].vertexIndex][0].v == 0.0f) {
				m_textureVertices[criteriaFace[1].vertexIndex][0] = {m_textureVertices[criteriaIndex][0].u + distance(vec12), m_textureVertices[criteriaIndex][0].v};
			}
			else
				m_textureVertices[criteriaFace[1].vertexIndex].push_back({m_textureVertices[criteriaIndex][0].u + distance(vec12), m_textureVertices[criteriaIndex][0].v});
		}
		if (!triangulation(faceCount[0], 2, criteriaIndex))
		{
			// printNowIdx(m_faceVertices, m_face, faceCount[0]);
			std::cerr << "삼각측량 오류" << std::endl;
			std::exit(-1);
		}
		std::vector<int> clipping_index;
		while (true)
		{
			std::vector<int> newFaceCount;
			for (int idx = 0 ; idx < faceCount.size() ; idx++) {
				int nowFaceIndex = faceCount[idx];
				if (m_face[nowFaceIndex].size() < 3)
					continue;
				// uv 좌표를 구할 수 있는지 확인
				int nonSetCount = 0;
				int nonSetIndex = 0;
				for (int i = 0 ; i < 3 ; i++) {
					if (m_face[nowFaceIndex][i].vertexIndex != criteriaIndex && \
						(m_textureVertices[m_face[nowFaceIndex][i].vertexIndex][0].u == 0.0f && m_textureVertices[m_face[nowFaceIndex][i].vertexIndex][0].v == 0.0f)) {
						nonSetCount++;
						nonSetIndex = i;
					}
				}
				if (nonSetCount == 0) {
					TriangleCheck triangleChecking = triangleCheck(nowFaceIndex, nonSetIndex);
					if (triangleChecking == LOG) {
						clipping_index.push_back(nowFaceIndex);
						continue;
					}
					else if (triangleChecking == RETRY) {
						newFaceCount.push_back(nowFaceIndex);
						continue;
					}
				}
				else if (nonSetCount > 1) {
					newFaceCount.push_back(nowFaceIndex);
					continue;
				}
				// 실질적 삼각측량 시작
				if (triangulation(nowFaceIndex, nonSetIndex, criteriaIndex) == false) {
					newFaceCount.push_back(nowFaceIndex);
					continue;
				}
				clipping_index.push_back(nowFaceIndex);
			}
			if (newFaceCount.size() == faceCount.size()) {
				break;
			}
			faceCount = newFaceCount;
		}
		textureClipping(clipping_index);
	}
}

Parse::~Parse() {
}