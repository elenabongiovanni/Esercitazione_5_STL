#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLib{
bool ImportCell0D(const string &filename, PolygonalMesh& mesh);
bool ImportCell1D(const string &filename, PolygonalMesh& mesh);
bool ImportCell2D(const string &filename, PolygonalMesh& mesh);
bool ImportMesh(const string& filepath, PolygonalMesh& mesh);



}
