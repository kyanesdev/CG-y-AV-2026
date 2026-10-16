#include "Hud.h"
#include "Primitives.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
using Glyph = std::array<const char*,7>;
const std::map<char,Glyph>& font() {
    static const std::map<char,Glyph> glyphs{
        {'0',{"01110","10001","10011","10101","11001","10001","01110"}},
        {'1',{"00100","01100","00100","00100","00100","00100","01110"}},
        {'2',{"01110","10001","00001","00010","00100","01000","11111"}},
        {'3',{"11110","00001","00001","01110","00001","00001","11110"}},
        {'4',{"00010","00110","01010","10010","11111","00010","00010"}},
        {'5',{"11111","10000","11110","00001","00001","10001","01110"}},
        {'6',{"00110","01000","10000","11110","10001","10001","01110"}},
        {'7',{"11111","00001","00010","00100","01000","01000","01000"}},
        {'8',{"01110","10001","10001","01110","10001","10001","01110"}},
        {'9',{"01110","10001","10001","01111","00001","00010","11100"}},
        {'B',{"11110","10001","10001","11110","10001","10001","11110"}},
        {'F',{"11111","10000","10000","11110","10000","10000","10000"}},
        {'J',{"00111","00010","00010","00010","10010","10010","01100"}},
        {'A',{"01110","10001","10001","11111","10001","10001","10001"}},
        {'C',{"01111","10000","10000","10000","10000","10000","01111"}},
        {'D',{"11110","10001","10001","10001","10001","10001","11110"}},
        {'E',{"11111","10000","10000","11110","10000","10000","11111"}},
        {'G',{"01111","10000","10000","10111","10001","10001","01110"}},
        {'H',{"10001","10001","10001","11111","10001","10001","10001"}},
        {'I',{"11111","00100","00100","00100","00100","00100","11111"}},
        {'K',{"10001","10010","10100","11000","10100","10010","10001"}},
        {'L',{"10000","10000","10000","10000","10000","10000","11111"}},
        {'M',{"10001","11011","10101","10101","10001","10001","10001"}},
        {'N',{"10001","11001","10101","10011","10001","10001","10001"}},
        {'O',{"01110","10001","10001","10001","10001","10001","01110"}},
        {'P',{"11110","10001","10001","11110","10000","10000","10000"}},
        {'R',{"11110","10001","10001","11110","10100","10010","10001"}},
        {'S',{"01111","10000","10000","01110","00001","00001","11110"}},
        {'T',{"11111","00100","00100","00100","00100","00100","00100"}},
        {'U',{"10001","10001","10001","10001","10001","10001","01110"}},
        {'Y',{"10001","10001","01010","00100","00100","00100","00100"}},
        {'V',{"10001","10001","10001","10001","10001","01010","00100"}},
        {'%',{"11001","11010","00010","00100","01000","01011","10011"}},
        {'X',{"10001","10001","01010","00100","01010","10001","10001"}},
        {'-',{"00000","00000","00000","11111","00000","00000","00000"}},
        {'+',{"00000","00100","00100","11111","00100","00100","00000"}},
        {'/',{"00001","00001","00010","00100","01000","10000","10000"}},
        {'.',{"00000","00000","00000","00000","00000","01100","01100"}}
    };
    return glyphs;
}
void square(MeshData& mesh, float x,float y,float size) {
    const unsigned index=static_cast<unsigned>(mesh.vertices.size());
    const glm::vec3 n{0,0,1};
    mesh.vertices.push_back({{x,y,0},n,{0,0}});
    mesh.vertices.push_back({{x+size,y,0},n,{1,0}});
    mesh.vertices.push_back({{x+size,y+size,0},n,{1,1}});
    mesh.vertices.push_back({{x,y+size,0},n,{0,1}});
    mesh.indices.insert(mesh.indices.end(),{index,index+1,index+2,index,index+2,index+3});
}
void write(MeshData& mesh,float x,float y,const std::string& message,float px) {
    for (char ch:message) {
        auto f=font().find(ch);
        if (f!=font().end()) {
            for (int row=0;row<7;++row) for(int col=0;col<5;++col) {
                if (f->second[row][col]=='1') square(mesh,x+col*px,y+row*px,px);
            }
        }
        x+=px*6.4f;
    }
}
glm::mat4 rect(float x,float y,float w,float h,float angle=0) {
    glm::mat4 m=glm::translate(glm::mat4(1.0f),{x,y,0});
    m=glm::rotate(m,angle,{0,0,1});
    return glm::scale(m,{w,h,0.01f});
}
}

void Hud::init() { cube_.load(primitives::cube()); }

void Hud::update_text(float speed_kmh,float altitude_m,float roll_degrees,
                      unsigned checkpoints,int score,bool paused,
                      bool looping_done,bool tonel_done,float throttle_pct,bool school_mode,
                      float accel_kmh_s,bool ground_contact,bool boundary_reached) {
    const int vel=static_cast<int>(std::round(std::max(0.0f,speed_kmh)));
    const int alt=static_cast<int>(std::round(altitude_m));
    const int roll=static_cast<int>(std::round(std::remainder(roll_degrees,360.0f)));
    MeshData mesh;
    write(mesh,22,15,"VEL " + std::to_string(vel) + " KMH",2.5f);
    write(mesh,22,41,"ALT " + std::to_string(alt) + " M",2.5f);
    write(mesh,22,67,"ROL " + std::to_string(roll) + " DEG",2.5f);
    write(mesh,22,93,"ARO " + std::to_string(checkpoints) + "/5 PTS " + std::to_string(score),2.5f);
    write(mesh,22,122,std::string("LOOP ") + (looping_done?"1/1":"0/1") +
                       " TON " + (tonel_done?"1/1":"0/1"),2.3f);
    write(mesh,22,148,"POT " + std::to_string(static_cast<int>(std::round(throttle_pct))) +
        "% " + (school_mode?"SIM 0.6X":"SIM 1X"),2.2f);
    const int accel=static_cast<int>(std::round(accel_kmh_s));
    write(mesh,22,174,"ACC " + std::string(accel>=0?"+":"") + std::to_string(accel) +
         " KMH/S",2.1f);
    if(ground_contact) write(mesh,22,199,"SUELO - MENU",2.4f);
    else if(boundary_reached) write(mesh,22,199,"LIMITE - MENU",2.4f);
    else if(paused) write(mesh,22,199,"PAUSA",2.4f);
    text_.load(mesh);
    has_text_=true;
}

void Hud::collect(std::vector<RenderItem>& items,int width,float pitch,float roll) const {
    // HUD dibujado en pixels 2D con proyeccion ortografica; no es UI externa.
    items.push_back({&cube_,rect(172,121,330,241),{0.06f,0.11f,0.18f}});
    if(has_text_) items.push_back({&text_,glm::mat4(1.0f),{0.97f,0.97f,0.86f}});
    const float x=static_cast<float>(width)-94.0f;
    items.push_back({&cube_,rect(x,72,160,130),{0.06f,0.11f,0.18f}});
    // Indicador de actitud (horizonte): pitch desplaza, roll inclina.
    const float offset = std::fmax(-34.f,std::fmin(34.f,pitch*45.0f));
    items.push_back({&cube_,rect(x,72+offset,116,4,-roll),{0.85f,0.65f,0.24f}});
    items.push_back({&cube_,rect(x,72,20,3),{0.96f,0.96f,0.93f}});
    items.push_back({&cube_,rect(x,72,3,15),{0.96f,0.96f,0.93f}});
}

void Hud::update_menu(int width,int height,int selected,bool game_started) {
    MeshData mesh;
    const float center = width * 0.5f;
    const float top = height * 0.5f - 173.0f;
    auto centered=[&](const std::string& label,float y,float pixel){
        const float total = static_cast<float>(label.size())*6.4f*pixel;
        write(mesh,center-total*0.5f,y,label,pixel);
    };
    centered("AEROIUA",top,5.0f);
    centered("MATEANDO EN LAS NUBES",top+68.0f,2.2f);
    const char* labels[] = {game_started?"CONTINUAR":"JUGAR", "REINICIAR", "SALIR"};
    for(int i=0;i<3;++i) centered(labels[i],top+136+i*62,3.0f);
    centered("FLECHAS Y ENTER",top+347,1.9f);
    menu_text_.load(mesh);
    (void)selected;
}

void Hud::collect_menu(std::vector<RenderItem>& items,int width,int height,int selected) const {
    const float center=width*0.5f, middle=height*0.5f;
    items.push_back({&cube_,rect(center,middle,610,490),{0.06f,0.11f,0.19f}});
    items.push_back({&cube_,rect(center,middle-207,570,4),{0.23f,0.59f,0.81f}});
    for(int i=0;i<3;++i) {
        const float y=middle-25+i*62;
        items.push_back({&cube_,rect(center,y,340,48),
                         i==selected?glm::vec3(0.19f,0.39f,0.53f):glm::vec3(0.10f,0.18f,0.27f)});
    }
    items.push_back({&menu_text_,glm::mat4(1.0f),{0.98f,0.97f,0.87f}});
}
