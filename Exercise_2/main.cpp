#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace PolygonalLib;

int main()
{
    PolygonalMesh mesh;
    string filepath ="PolygonalMesh";
    if(!ImportMesh(filepath, mesh))
        return 1;


    return 0;
}
