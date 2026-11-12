#ifndef ESCENAS_H
#define ESCENAS_H
#include <camera.h>
#include <scene.h>
using BuildScene = void (*)(Scene& scene, Camera& camera);
struct SceneEntry { int id; const char* name; BuildScene build; };
void build_scene_00(Scene& scene, Camera& camera);
void build_scene_01(Scene& scene, Camera& camera);
void build_scene_02(Scene& scene, Camera& camera);
void build_scene_03(Scene& scene, Camera& camera);
void build_scene_04(Scene& scene, Camera& camera);
void build_scene_05(Scene& scene, Camera& camera);
void build_scene_06(Scene& scene, Camera& camera);
void build_scene_07(Scene& scene, Camera& camera);
void build_scene_08(Scene& scene, Camera& camera);
void build_scene_11(Scene& scene, Camera& camera);
void build_scene_12(Scene& scene, Camera& camera);
extern const SceneEntry kScenes[];
extern const int kSceneCount;
#endif
