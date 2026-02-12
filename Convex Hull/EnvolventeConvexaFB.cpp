#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <set>

using namespace std;

const float PI = 3.14159265358979323846f;
const float EPS = 1e-6;
#define TEST
//#define RUN

float dist(pair<float,float> u, pair<float,float> v){
    float dx = v.first - u.first;
    float dy = v.second - u.second;
    return sqrt(dx*dx+dy*dy);
}

float angulo(const std::pair<float, float>& u, const std::pair<float, float>& v,const pair<float,float> direction) {
    float dx = v.first - u.first;   // v_x - u_x
    float dy = v.second - u.second; // v_y - u_y
    
    float angle = acos((dx*direction.first + dy*direction.second)/((sqrt(direction.first*direction.first+direction.second*direction.second))*(sqrt(dx*dx+dy*dy))));
    return angle;
}

int mod(int a, int n){
    return (a % n + n) % n;
}

pair<float,float> buscaMinAngulo(pair<float,float> p, vector<pair<float,float>> points,pair<float,float> direction,int posIni, int posFin){
    int posmin=posIni;
    float minAngulo = 2*PI;
    float anguloAux;
    for(int i=posIni; i <= posFin; i++){
        if(points[i] != p){
            if((anguloAux = angulo(p,points[i],direction)) < minAngulo){
                minAngulo = anguloAux;
                posmin = i;
            }
            else if(anguloAux == minAngulo){
                posmin = (dist(p,points[i]) > dist(p,points[posmin])) ? i : posmin;
            }
        }
    }
    
    return points[posmin];
}

pair<float,float> buscaMin(vector<pair<float,float>> points,int posIni, int posFin){
    float min = points[posIni].second;
    int pos = posIni;
    for(int i = posIni+1; i < posFin; i++){
        if(points[i].second < min){
            pos = i;
            min = points[i].second;
        }
        else if(points[i].second == min){
            if(points[i].first < points[pos].first){
                pos = i;
            }
        }
    }

    return points[pos];
}

int posX(vector<pair<float,float>> V,int inicio, int final,  bool max){
    float x = V[inicio].first;
    int pos = inicio;
    if(max){
        for(int i = inicio+1;i<= final;i++){
            if(V[i].first > x){
                x = V[i].first;
                pos = i;
            }
        }
    }
    else{
        for(int i = 1;i <= final ;i++){
            if(V[i].first < x){
                x = V[i].first;
                pos = i;
            }
        }
    }

    return pos;
}

vector<pair<float,float>> EnvolventeConvexa(vector<pair<float,float>> points,int posIni, int posFin){
    vector<pair<float,float>> envolvente;
	pair<float,float> inicio,current,direction = {1,0};
    inicio = buscaMin(points,posIni,posFin);
    current = inicio;
    envolvente.push_back(inicio);
    current = buscaMinAngulo(current,points,direction,posIni,posFin);
    while(current != inicio){
        direction = {current.first-envolvente.back().first,current.second-envolvente.back().second};
        envolvente.push_back(current);
        current = buscaMinAngulo(current,points,direction,posIni,posFin);
        
    }
    
    return envolvente;
}


void drawPointCloud(const std::vector<std::pair<float, float>>& points,const std::vector<std::pair<float, float>>& envolvente){
     // Crear la ventana
     sf::RenderWindow window(sf::VideoMode(800, 600), "Envolvente Convexa ");

     while (window.isOpen()) {
         sf::Event event;
         while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed)
                 window.close();
         }

         window.clear(sf::Color::White);

         // Dibujar los puntos originales (en negro)
        for (const auto& point : points) {
            sf::CircleShape dot(3);
            dot.setFillColor(sf::Color::Black);
            dot.setPosition(point.first, point.second);
            window.draw(dot);
        }
        // Dibujar la envolvente convexa (en amarillo)
        if (!envolvente.empty()) {
            sf::VertexArray lines(sf::LineStrip);
            for (const auto& point : envolvente) {
                lines.append(sf::Vertex(sf::Vector2f(point.first, point.second), sf::Color::Red));
            }
            // Cerrar la envolvente uniendo el último punto con el primero
            lines.append(sf::Vertex(sf::Vector2f(envolvente.front().first, envolvente.front().second), sf::Color::Red));
            window.draw(lines);
        }

        window.display();
     }
}

/**
 * Compilar: g++ -o EnvolventeConvexaFB EnvolventeConvexaFB.cpp -lsfml-graphics -lsfml-window -lsfml-system
 */ 
int main() {
    #ifdef RUN
    const int num_puntos = 32; 
    std::vector<std::pair<float, float>> points,izda,dcha;

    // Configuración del generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_x(0.0, 800.0);
    std::uniform_real_distribution<float> dist_y(0.0, 600.0);
    
    for (int i = 0; i < num_puntos; ++i) {
        float x = dist_x(gen);
        float y = dist_y(gen);
        points.push_back({x,y});
    }
    //Ordenamos el vector O(nlogn)
    sort(points.begin(),points.end());
    vector<pair<float,float>> envolvente = EnvolventeConvexa(points,0,points.size()-1);
    drawPointCloud(points,envolvente);
    #endif
    #ifdef TEST
      ofstream fsalida;
      chrono::time_point<std::chrono::high_resolution_clock> t0, tf;
      unsigned long int duracion;
      int n,digitos = 10;

      fsalida.open("FB.txt");

      int v[] = {128,256,512,1024,2048,4096,8192,16384,32768,65536};

      std::vector<std::pair<float, float>> points,envolvente;

        // Configuración del generador de números aleatorios
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist_x(0.0, 800.0);
        std::uniform_real_distribution<float> dist_y(0.0, 600.0);

        for (int i = 0; i < v[9]; ++i) {
            float x = dist_x(gen);
            float y = dist_y(gen);
            points.push_back({x,y});
        }
        
      for(int i=0; i < 10; i++){
        n = v[i];
        //Ordenamos el vector O(nlogn)
        sort(points.begin(),points.end());

        t0= std::chrono::high_resolution_clock::now();
        envolvente = EnvolventeConvexa(points,0,n-1);
        tf= std::chrono::high_resolution_clock::now();
        duracion = std::chrono::duration_cast<std::chrono::microseconds>(tf - t0).count();

        fsalida << "|TAM=" << setw(digitos) << n << "    " << "TIEMPO=" << setw(digitos) << duracion << "ms|" << endl;
      }
    #endif
    
    return 0;
}