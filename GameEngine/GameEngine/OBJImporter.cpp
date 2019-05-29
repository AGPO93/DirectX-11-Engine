#include "OBJImporter.h"

#include <sstream>
#include <istream>
#include <fstream>
#include <algorithm>

bool OBJImporter::ImportMesh(std::vector<ModelClass::VertexType>* vertices, std::vector<unsigned long>* indices, std::string name)
{
	std::ifstream file("Resources/" + name + ".obj");

	if (!file)
	{
		return false;
	}

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> uvs;

	std::string line;
	std::string input;

	float x = 0;
	float y = 0;
	float z = 0;

	int id = vertices->size();

	while (std::getline(file, line))
	{
		std::stringstream line_stream(line);

		line_stream >> input;

		if (input == "v")
		{
			line_stream >> x;
			line_stream >> y;
			line_stream >> z;

			positions.push_back(DirectX::XMFLOAT3(x, y, z));
		}
		else if (input == "vt")
		{
			line_stream >> x;
			line_stream >> y;

			uvs.push_back(DirectX::XMFLOAT2(x, y));
		}
		else if (input == "vn")
		{
			line_stream >> x;
			line_stream >> y;
			line_stream >> z;

			normals.push_back(DirectX::XMFLOAT3(x, y, z));
		}
		else if (input == "f")
		{
			int pos_i;
			int uv_i;
			int nor_i;

			for (int i = 0; i < 3; ++i)
			{
				line_stream >> input;

				for (int j = 0; j < input.size(); ++j)
					if (input[j] == '/') input[j] = ' ';

				std::stringstream vertex_stream(input);

				vertex_stream >> pos_i;
				vertex_stream >> uv_i;
				vertex_stream >> nor_i;

				vertices->push_back({ positions[pos_i - 1], uvs[uv_i - 1], normals[nor_i - 1] });
				indices->push_back(indices->size());
			}
		}
	}

	return true;
}