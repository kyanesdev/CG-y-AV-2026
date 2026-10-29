// AgujeroNegro - Ray tracing por CPU (C++17)
// Computacion Grafica y Ambientes Virtuales - CRUC/IUA, 2026
// Modelo visual inspirado en un agujero negro supermasivo como TON 618.
// Sin CUDA, RTX, OpenGL ni texturas externas: cada pixel se calcula en CPU.
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

namespace {
constexpr double PI = 3.14159265358979323846;
constexpr double HORIZON = 1.0;   // Radio de Schwarzschild en unidades de escena
constexpr double DISK_IN = 2.3;   // Disco estilizado (no escala fisica de TON 618)
constexpr double DISK_OUT = 8.7;

struct Vec3 {
    double x=0, y=0, z=0;
    Vec3() = default;
    Vec3(double a,double b,double c):x(a),y(b),z(c){}
    Vec3 operator+(Vec3 v) const {return {x+v.x,y+v.y,z+v.z};}
    Vec3 operator-(Vec3 v) const {return {x-v.x,y-v.y,z-v.z};}
    Vec3 operator*(double s) const {return {x*s,y*s,z*s};}
    Vec3 operator/(double s) const {return {x/s,y/s,z/s};}
    Vec3& operator+=(Vec3 v) {x+=v.x;y+=v.y;z+=v.z;return *this;}
};
double dot(Vec3 a, Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
Vec3 cross(Vec3 a, Vec3 b) {return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
double len(Vec3 v) {return std::sqrt(dot(v,v));}
Vec3 norm(Vec3 v) {return v / std::max(1e-12,len(v));}
Vec3 mix(Vec3 a,Vec3 b,double t) {return a*(1-t)+b*t;}
double clamp(double x,double a=0.0,double b=1.0){return std::max(a,std::min(b,x));}
double smooth(double a,double b,double x){double t=clamp((x-a)/(b-a));return t*t*(3-2*t);}
double fract(double v){return v-std::floor(v);}
uint32_t scramble(uint32_t x){x^=x>>16; x*=0x7feb352du; x^=x>>15; x*=0x846ca68bu; x^=x>>16; return x;}
double hash2(int x,int y,int seed=0){return scramble(uint32_t(x)*0x9e3779b9u ^ uint32_t(y)*0x85ebca6bu ^ uint32_t(seed)*0xc2b2ae35u) / 4294967295.0;}
double noise2(double x,double y){
    int ix=int(std::floor(x)), iy=int(std::floor(y));
    double fx=smooth(0,1,fract(x)), fy=smooth(0,1,fract(y));
    double a=hash2(ix,iy),b=hash2(ix+1,iy),c=hash2(ix,iy+1),d=hash2(ix+1,iy+1);
    return (a*(1-fx)+b*fx)*(1-fy)+(c*(1-fx)+d*fx)*fy;
}
double fbm(double x,double y){double s=0, a=.5;for(int i=0;i<4;++i){s+=a*noise2(x,y); x=x*2.03+7.8; y=y*2.02-3.1;a*=.5;}return s/.9375;}

// Luz de fondo: estrellas procedurales + nubosidad tenue. Se evalua
// en la direccion que toma el rayo DESPUES de atravesar la lente gravitatoria.
Vec3 sky(Vec3 d){
    d=norm(d);
    double lon=std::atan2(d.x,-d.z), lat=std::asin(clamp(d.y,-1,1));
    double u=(lon+PI)/(2*PI), v=(lat+PI/2)/PI;
    double n=fbm(u*17.0+2.0,v*9.0+4.0);
    double wisps=fbm(u*42.0+11.0,v*22.0-1.0);
    double lane=std::exp(-std::pow((d.y+0.32*d.x-0.2*d.z)*2.0,2));
    Vec3 base={0.0025,0.0035,0.009};
    double cloud=std::pow(clamp((n-.35)*2.0),2.0) * lane;
    base+=Vec3{.030,.010,.058}*cloud;
    base+=Vec3{.007,.024,.051}*(std::pow(clamp(wisps),2.4)*lane);
    base+=Vec3{.004,.007,.013}*(.2+.6*wisps);
    // Celda angular: puntos estelares aislados de distintos tamanos.
    for(int layer=0;layer<2;++layer){
        double scale=layer==0?520.0:160.0;
        double sx=u*scale, sy=v*scale;
        int ix=int(std::floor(sx)),iy=int(std::floor(sy));
        for(int oy=-1;oy<=1;++oy) for(int ox=-1;ox<=1;++ox){
            int cx=ix+ox, cy=iy+oy;
            double rnd=hash2(cx,cy,layer+51);
            if(rnd > (layer==0?.12:.055)) continue;
            double dx=sx-(cx+.12+.76*hash2(cx,cy,layer+101));
            double dy=sy-(cy+.12+.76*hash2(cx,cy,layer+151));
            double r2=dx*dx+dy*dy;
            double core=std::exp(-r2/(layer==0?.004:.0022));
            double halo=std::exp(-r2/(layer==0?.075:.035));
            double strength=layer==0?.42:1.15;
            double tint=hash2(cx,cy,layer+211);
            Vec3 col=mix(Vec3{.65,.77,1.0},Vec3{1,.78,.53},tint);
            base+=col * (core*strength*(.5+1.5*rnd)+halo*strength*.017);
        }
    }
    return base;
}

// Funcion que curva la trayectoria del rayo. Integramos una ecuacion
// de orbita nula aproximada para una metrica Schwarzschild sin rotacion:
// r'' = -1.5 * rs * L^2 * r / |r|^5  (unidades normalizadas).
// Sirve para demostrar lente gravitacional; no es un simulador relativista
// de TON 618 con disco hidrodinamico, Kerr, transporte espectral, etc.
Vec3 gravity(Vec3 p,double L2){
    double r2=dot(p,p), r=std::sqrt(std::max(r2,1e-12));
    return p*(-1.5*HORIZON*L2/(r2*r2*r));
}
struct State {Vec3 p,v;};
void advance(State& s,double h,double L2){
    // Punto medio explicito: 2 evaluaciones de aceleracion por segmento.
    Vec3 a=gravity(s.p,L2);
    Vec3 mp=s.p+s.v*(.5*h);
    Vec3 mv=s.v+a*(.5*h);
    s.p+=mv*h;
    s.v+=gravity(mp,L2)*h;
}

// La interseccion y=0 ocurre entre dos pasos del rayo curvo.
// Se interpola para hallar el radio y colorear el disco de acrecion.
struct DiscSample {Vec3 light; double alpha;};
DiscSample disc(Vec3 p,Vec3 raydir){
    double r=std::sqrt(p.x*p.x+p.z*p.z);
    if(r<DISK_IN || r>DISK_OUT) return {{0,0,0},0};
    double a=std::atan2(p.z,p.x);
    double swirl=a+3.9*std::log(std::max(1.0,r));
    double turbulent=fbm(3.8*swirl+0.42*r, r*1.8);
    double filament=std::sin(24*swirl+13.0*turbulent+4.5*r);
    double ridges=std::pow(.5+.5*filament,3.0);
    double streak=std::pow(.5+.5*std::sin(65*swirl-9.0*r+12*turbulent),5);
    double ripples=.77+.22*std::sin(r*13+9*turbulent)+.18*ridges+.14*streak;
    double inner=smooth(DISK_IN,DISK_IN+.45,r);
    double outer=1.0-smooth(DISK_OUT-1.5,DISK_OUT,r);
    double energy=std::pow(DISK_IN/r,1.1) * inner * outer * ripples;
    double t=clamp((r-DISK_IN)/(DISK_OUT-DISK_IN));
    Vec3 hot={1.90,1.52,1.14};
    Vec3 warm={1.7,.47,.12};
    Vec3 cool={.66,.10,.11};
    Vec3 color=mix(hot,warm,smooth(.07,.58,t));
    color=mix(color,cool,smooth(.45,1,t));
    Vec3 tangent=norm(Vec3{-p.z,0,p.x});
    double approach=dot(tangent,norm(raydir));
    double doppler=std::pow(clamp(1.0-.55*approach,.50,1.8),3.1);
    double alpha=clamp((.23+.53*energy)*inner*outer,0,.83);
    return {color*(.28+3.8*energy)*doppler,alpha};
}

struct TraceResult {Vec3 rgb;};
TraceResult trace(Vec3 origin,Vec3 direction){
    State s{origin,norm(direction)};
    double L2=dot(cross(origin,s.v),cross(origin,s.v));
    Vec3 light{0,0,0};
    double trans=1;
    bool captured=false;
    double minR=len(s.p);
    int crossings=0;
    for(int step=0;step<1100;++step){
        double r=len(s.p); minR=std::min(minR,r);
        if(r <= HORIZON*1.025){captured=true;break;}
        if(r>45.0 && dot(s.p,s.v)>0)break;
        double speed=std::max(.5,len(s.v));
        double h=clamp(.065*r/speed,.008,.48);
        State old=s;
        advance(s,h,L2);
        if((old.p.y>0 && s.p.y<=0)||(old.p.y<0&&s.p.y>=0)){
            double f=old.p.y/(old.p.y-s.p.y);
            Vec3 hit=old.p+(s.p-old.p)*f;
            DiscSample d=disc(hit,old.v);
            light+=d.light*(d.alpha*trans);
            trans*=1.0-d.alpha;
            ++crossings;
        }
        if(trans<.015)break;
    }
    if(!captured){light+=sky(s.v)*trans;}
    // Halo angosto para hacer visible la region proxima a la esfera de fotones.
    // Se considera un realce artistico, NO una solucion fisica espectral.
    double ring=std::exp(-std::pow((minR-1.54)/.14,2));
    light+=Vec3{1.45,.53,.18} * ring * .13 * (captured?.52:1.0);
    // Velo suave alrededor del disco (solo realce estetico de alto contraste).
    double halo=std::exp(-std::pow((minR-2.65)/.65,2));
    light+=Vec3{.06,.020,.026}*halo*.10;
    (void)crossings;
    return {light};
}

double tonemap(double x){ // Curva filmica ACES aproximada.
    x=std::max(0.0,x);
    return clamp((x*(2.51*x+.03))/(x*(2.43*x+.59)+.14));
}
uint8_t byte(double x){return (uint8_t)std::lround(255.0*std::pow(tonemap(x),1.0/2.2));}

// PNG RGB8 de biblioteca estandar, sin dependencias externas.
// IDAT contiene bloques DEFLATE almacenados (sin compresion) validos.
void writeU32(std::vector<uint8_t>& v,uint32_t n){v.push_back((n>>24)&255);v.push_back((n>>16)&255);v.push_back((n>>8)&255);v.push_back(n&255);}
uint32_t crc32(const uint8_t* p,size_t n){
    uint32_t c=0xffffffffu;
    for(size_t i=0;i<n;++i){c^=p[i];for(int j=0;j<8;++j)c=(c>>1)^(0xedb88320u & uint32_t(-(int(c&1))));}
    return c^0xffffffffu;
}
void chunk(std::vector<uint8_t>& png,const char type[4],const std::vector<uint8_t>& data){
    writeU32(png,(uint32_t)data.size());
    size_t begin=png.size();
    png.insert(png.end(),type,type+4);
    png.insert(png.end(),data.begin(),data.end());
    writeU32(png,crc32(png.data()+begin,png.size()-begin));
}
void savePNG(const std::string& filename,int w,int h,const std::vector<uint8_t>& rgb){
    std::vector<uint8_t> raw;raw.reserve(size_t(h)*(1+3*size_t(w)));
    for(int y=0;y<h;++y){raw.push_back(0);raw.insert(raw.end(),rgb.begin()+3*size_t(y)*w,rgb.begin()+3*size_t(y+1)*w);}
    std::vector<uint8_t> deflate{0x78,0x01}; // zlib, sin diccionario, fastest
    for(size_t pos=0;pos<raw.size();){
        size_t len=std::min(size_t(65535),raw.size()-pos);
        bool last=(pos+len==raw.size());
        deflate.push_back(last?1:0); // bloque almacenado (BFINAL, BTYPE=00)
        uint16_t n=uint16_t(len),inv=uint16_t(~n);
        deflate.push_back(n&255);deflate.push_back(n>>8);
        deflate.push_back(inv&255);deflate.push_back(inv>>8);
        deflate.insert(deflate.end(),raw.begin()+pos,raw.begin()+pos+len);pos+=len;
    }
    uint32_t a=1,b=0;
    for(uint8_t x:raw){a=(a+x)%65521;b=(b+a)%65521;}
    writeU32(deflate,(b<<16)|a);
    std::vector<uint8_t> png{137,80,78,71,13,10,26,10};
    std::vector<uint8_t> ihdr;writeU32(ihdr,w);writeU32(ihdr,h);
    ihdr.insert(ihdr.end(),{8,2,0,0,0}); // 8 bits, RGB, sin entrelazado
    chunk(png,"IHDR",ihdr);chunk(png,"IDAT",deflate);chunk(png,"IEND",{});
    std::filesystem::path path(filename);
    if(path.has_parent_path())std::filesystem::create_directories(path.parent_path());
    std::ofstream out(filename,std::ios::binary);
    if(!out)throw std::runtime_error("No se puede abrir: "+filename);
    out.write((const char*)png.data(),std::streamsize(png.size()));
    if(!out)throw std::runtime_error("Fallo al escribir PNG: "+filename);
}

int positive(const std::string& s,int max,const char* arg){
    size_t idx=0;long long n=0;
    try{n=std::stoll(s,&idx);}catch(...){throw std::runtime_error(std::string("Valor invalido para ")+arg);}
    if(idx!=s.size() || n<1||n>max)throw std::runtime_error(std::string("Fuera de rango para ")+arg);
    return (int)n;
}
struct Args{int width=1280,height=720,samples=2,threads=0;std::string output="renders/AgujeroNegro.png";};
Args parse(int argc,char** argv){
    Args a;
    for(int i=1;i<argc;++i){
        std::string key=argv[i];
        if(key=="--help"||key=="-h"){
            std::cout<<"AgujeroNegro - Ray tracing en CPU (C++17)\n"
             <<"Uso: ./bin/AgujeroNegro [--width N] [--height N] [--samples N] [--threads N] [--output archivo.png]\n"
             <<"Ejemplo: ./bin/AgujeroNegro --width 1280 --height 720 --samples 2 --output renders/AgujeroNegro.png\n";
            std::exit(0);
        }
        if(i+1>=argc)throw std::runtime_error("Falta valor para "+key);
        std::string value=argv[++i];
        if(key=="--width")a.width=positive(value,4096,"width");
        else if(key=="--height")a.height=positive(value,4096,"height");
        else if(key=="--samples")a.samples=positive(value,16,"samples");
        else if(key=="--threads")a.threads=positive(value,128,"threads");
        else if(key=="--output")a.output=value;
        else throw std::runtime_error("Opcion desconocida: "+key);
    }
    if(a.output.size()<4||a.output.substr(a.output.size()-4)!=".png")throw std::runtime_error("--output debe terminar en .png");
    return a;
}

void render(const Args& args){
    const int w=args.width,h=args.height,spp=args.samples;
    const Vec3 eye={0,5.2,17.8};
    const Vec3 forward=norm(Vec3{0,0,0}-eye);
    const Vec3 right=norm(cross(forward,Vec3{0,1,0}));
    const Vec3 up=norm(cross(right,forward));
    const double focal=std::tan(49.0*PI/360.0),aspect=double(w)/h;
    std::vector<uint8_t> rgb(size_t(w)*h*3);
    const int threads=args.threads?args.threads:std::max(1u,std::thread::hardware_concurrency());
    std::atomic<int> nextRow{0};
    auto work=[&](){
        for(;;){
            int y=nextRow.fetch_add(1);
            if(y>=h)break;
            for(int x=0;x<w;++x){
                Vec3 color;
                for(int s=0;s<spp;++s){
                    // Subpixeles deterministas para suavizar bordes; spp=1 usa centro.
                    double jx=spp==1?.5:fract(.5+(s+1)*.618033988749895);
                    double jy=spp==1?.5:fract(.5+(s+1)*.754877666246693);
                    double nx=(2.0*(x+jx)/w-1.0)*aspect*focal;
                    double ny=(1.0-2.0*(y+jy)/h)*focal;
                    Vec3 ray=norm(forward+right*nx+up*ny);
                    color+=trace(eye,ray).rgb;
                }
                color=color/double(spp);
                size_t i=(size_t(y)*w+x)*3;
                rgb[i]=byte(color.x);rgb[i+1]=byte(color.y);rgb[i+2]=byte(color.z);
            }
        }
    };
    auto t0=std::chrono::steady_clock::now();
    std::cout<<"Renderizando "<<w<<"x"<<h<<", "<<spp<<" muestra(s)/pixel, "<<threads<<" hilo(s) en CPU...\n";
    std::vector<std::thread> pool;
    for(int i=0;i<threads;++i)pool.emplace_back(work);
    for(auto& th:pool)th.join();
    savePNG(args.output,w,h,rgb);
    double secs=std::chrono::duration<double>(std::chrono::steady_clock::now()-t0).count();
    std::cout<<"Imagen creada: "<<args.output<<" ("<<std::fixed<<std::setprecision(2)<<secs<<" s)\n";
}
}

int main(int argc,char** argv){
    try{render(parse(argc,argv));}
    catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<"\n";return 1;}
    return 0;
}
