#pragma once
#include "ModelClass.h"
#include <string>

namespace OBJImporter
{
	bool ImportMesh(std::vector<ModelClass::VertexType>* vertices, std::vector<unsigned long>* indices, std::string name);
}