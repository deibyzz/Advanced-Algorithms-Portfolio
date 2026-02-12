#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>

using namespace std;

const float PI = 3.14159265358979323846f;
const float EPSILON = 1e-6;
//#define TEST
#define RUN

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
bool encima(pair<float,float>check,pair<float,float>p1,pair<float,float>p2){
    //y-y0 = m(x-x0)
    //y = dy/dx(x-x0)+y0
    return check.second > ((p2.second-p1.second)/(p2.first-p1.first)*(check.first-p1.first) + p1.second);
}

bool debajo(pair<float,float>check,pair<float,float>p1,pair<float,float>p2){
    return check.second < ((p2.second-p1.second)/(p2.first-p1.first)*(check.first-p1.first) + p1.second);
}

vector<pair<float,float>> EnvolventeConvexaDV(vector<pair<float,float>> &V, int inicio, int fin){
    //Caso base
	if((fin-inicio+1) <= 2){
        vector<pair<float,float>> envolvente;
        for(int i = inicio; i <= fin;i++){
            envolvente.push_back(V[i]);
        }
        return envolvente;
    }
    //LLamadas recursivas para resolver las mitades
    int mitad = (inicio + fin) / 2;
    vector<pair<float,float>>izda = EnvolventeConvexaDV(V,inicio,mitad);
    vector<pair<float,float>>dcha = EnvolventeConvexaDV(V,mitad+1,fin);
    
    int base_izda = posX(izda,0,izda.size()-1,1); //Punto más a la derecha de la izda
    int base_dcha = posX(dcha,0,dcha.size()-1,0); //Punto más a la izquierda de la dcha

    int p1 = base_izda,p2=base_dcha;

    // Unión superior

    while(encima(izda[mod(p1+1,izda.size())],izda[p1],dcha[p2]) || encima(dcha[mod(p2-1,dcha.size())],izda[p1],dcha[p2])){
        if(encima(izda[mod(p1+1,izda.size())],izda[p1],dcha[p2])){
            p1 = mod(p1+1,izda.size());
        }
        if(encima(dcha[mod(p2-1,dcha.size())],izda[p1],dcha[p2])){
            p2 = mod(p2-1,dcha.size());
        }
    }
    int UL = p1, UR = p2; //Puntos que enlazan la envolvente por arriba
    
    p1 = base_izda;
    p2 = base_dcha;

    //Unión inferior

    while (debajo(izda[mod(p1-1,izda.size())],izda[p1],dcha[p2]) || debajo(dcha[mod(p2+1,dcha.size())],izda[p1],dcha[p2]))
    {
        if(debajo(izda[mod(p1-1,izda.size())],izda[p1],dcha[p2])){
            p1 = mod(p1-1,izda.size());
        }
        if(debajo(dcha[mod(p2+1,dcha.size())],izda[p1],dcha[p2])){
            p2 = mod(p2+1,dcha.size());
        }
    }

    int DL = p1, DR = p2; //Puntos que enlazan la envolvente por debajo

    //Guardamos la envolvente total
    p1 = UL;
    p2 = DR;
    
    vector<pair<float,float>> Envolvente;

    while(p1 != DL){
        Envolvente.push_back(izda[p1]);
        p1 = mod(p1+1,izda.size());
    }
    Envolvente.push_back(izda[DL]);

    while(p2 != UR){
        Envolvente.push_back(dcha[p2]);
        p2 = mod(p2+1,dcha.size());
    }

    Envolvente.push_back(dcha[UR]);

    return Envolvente;

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

// Función para dibujar una nube de puntos y su envolvente en una misma ventana
void drawPointCloud(const std::vector<std::pair<float, float>>& points, const std::vector<std::pair<float, float>>& envolvente_izda,const std::vector<std::pair<float, float>>& envolvente_dcha,const std::vector<std::pair<float, float>>& envolvente) {
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

        // Dibujar la envolvente convexa (en rojo)
        if (!envolvente_izda.empty()) {
            sf::VertexArray lines(sf::LineStrip);
            for (const auto& point : envolvente_izda) {
                lines.append(sf::Vertex(sf::Vector2f(point.first, point.second), sf::Color::Red));
            }
            // Cerrar la envolvente uniendo el último punto con el primero
            lines.append(sf::Vertex(sf::Vector2f(envolvente_izda.front().first, envolvente_izda.front().second), sf::Color::Red));
            window.draw(lines);
        }

        // Dibujar la envolvente convexa (en azul)
        if (!envolvente_dcha.empty()) {
            sf::VertexArray lines(sf::LineStrip);
            for (const auto& point : envolvente_dcha) {
                lines.append(sf::Vertex(sf::Vector2f(point.first, point.second), sf::Color::Blue));
            }
            // Cerrar la envolvente uniendo el último punto con el primero
            lines.append(sf::Vertex(sf::Vector2f(envolvente_dcha.front().first, envolvente_dcha.front().second), sf::Color::Blue));
            window.draw(lines);
        }

        // Dibujar la envolvente convexa (en amarillo)
        if (!envolvente.empty()) {
            sf::VertexArray lines(sf::LineStrip);
            for (const auto& point : envolvente) {
                lines.append(sf::Vertex(sf::Vector2f(point.first, point.second), sf::Color::Green));
            }
            // Cerrar la envolvente uniendo el último punto con el primero
            lines.append(sf::Vertex(sf::Vector2f(envolvente.front().first, envolvente.front().second), sf::Color::Green));
            window.draw(lines);
        }

        window.display();
    }
}

/**
 * Compilar: g++ -o EnvolventeConvexaDV EnvolventeConvexaDV.cpp -lsfml-graphics -lsfml-window -lsfml-system
 */ 
int main() {
    #ifdef RUN
    const int num_puntos = 16384; 
    unsigned long int duracion;
    chrono::time_point<std::chrono::high_resolution_clock> t0, tf;
    std::vector<std::pair<float, float>> points,izda,dcha,envolvente;

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
    t0= std::chrono::high_resolution_clock::now();
    sort(points.begin(),points.end());
    envolvente = EnvolventeConvexaDV(points,0,num_puntos-1);
    tf= std::chrono::high_resolution_clock::now();
    duracion = std::chrono::duration_cast<std::chrono::microseconds>(tf - t0).count();
    cout << "|TAM=" << setw(10) << num_puntos << "    " << "TIEMPO=" << setw(10) << duracion << "ms|" << endl;
    drawPointCloud(points,envolvente);
    #endif
    #ifdef TEST
      ofstream fsalida;
      chrono::time_point<std::chrono::high_resolution_clock> t0, tf;
      unsigned long int duracion;
      int n,digitos = 15;

      fsalida.open("DV.txt");

      
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
        t0= std::chrono::high_resolution_clock::now();
        sort(points.begin(),points.end());
        envolvente = EnvolventeConvexaDV(points,0,n-1);
        tf= std::chrono::high_resolution_clock::now();
        duracion = std::chrono::duration_cast<std::chrono::microseconds>(tf - t0).count();

        fsalida << "|TAM=" << setw(digitos) << n << "    " << "TIEMPO=" << setw(digitos) << duracion << "ms|" << endl;
      }
    #endif
    
    return 0;
}