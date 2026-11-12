#pragma once
#include "Mesh.h"
namespace primitives {
// Dimensiones a lo largo de X, Y y Z.
MeshData cube(float sx=1.0f, float sy=1.0f, float sz=1.0f);
// Eje axial Y, dimensiones en unidades de escena, gajos >= 3, anillos >= 1.
MeshData cylinder(float radio, float largo, unsigned gajos, unsigned anillos=1U);
// conicidad: SEMIANGULO respecto del eje Y, expresado en RADIANES.
// La altura se calcula como radio/tan(conicidad). Vertice superior en el eje Y.
MeshData cone(float radio, float conicidad, unsigned gajos, unsigned anillos=1U);
// Comprueba orientacion antihoraria de triangulos no degenerados.
bool check_winding(const MeshData& mesh);
}
