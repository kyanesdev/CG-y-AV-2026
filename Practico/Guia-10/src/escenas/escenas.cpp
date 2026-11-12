#include <escenas.h>
const SceneEntry kScenes[] = {
    {  0, "Taller 1: iluminacion local", build_scene_00 },
    {  1, "Luces y sombras de colores", build_scene_01 },
    {  2, "Espejos paralelos", build_scene_02 },
    {  3, "Pasillo de espejos", build_scene_03 },
    {  4, "Esferas de vidrio y de agua", build_scene_04 },
    {  5, "Banco de indices de refraccion", build_scene_05 },
    {  6, "Esfera maciza, hueca y burbuja", build_scene_06 },
    {  7, "La piscina", build_scene_07 },
    {  8, "Matrioshka de vidrio", build_scene_08 },
    { 11, "Spheres and Checkerboard (Whitted)", build_scene_11 },
    { 12, "Plantilla de practica de parcial", build_scene_12 },
};
const int kSceneCount = int(sizeof(kScenes) / sizeof(kScenes[0]));
