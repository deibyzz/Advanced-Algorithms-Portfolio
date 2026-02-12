#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
using namespace std;

const int INF = numeric_limits<int>::max();


vector<pair<int,int>> construirSolucion(const vector<vector<int>> &mapa, const vector<vector<int>> costes_minimos){
    int f = mapa.size(), c = mapa[0].size();
    vector<pair<int,int>> sol;
    vector<pair<int,int>> posibles_movs = {{-1,0},{0,-1},{-1,-1}};
    pair<int,int> pos_actual = {f-1,c-1};
    do{
        sol.push_back(pos_actual);
        int valor_actual = costes_minimos[pos_actual.first][pos_actual.second] - mapa[pos_actual.first][pos_actual.second];
        if(pos_actual.first>0 && costes_minimos[pos_actual.first+posibles_movs[0].first][pos_actual.second+posibles_movs[0].second] == valor_actual){
            pos_actual.first += posibles_movs[0].first;
            pos_actual.second += posibles_movs[0].second;
        }
        else if(pos_actual.second>0 && costes_minimos[pos_actual.first+posibles_movs[1].first][pos_actual.second+posibles_movs[1].second] == valor_actual){
            pos_actual.first += posibles_movs[1].first;
            pos_actual.second += posibles_movs[1].second;
        }
        else if(pos_actual.first>0 && pos_actual.second>0 && costes_minimos[pos_actual.first+posibles_movs[2].first][pos_actual.second+posibles_movs[2].second] == valor_actual){
            pos_actual.first += posibles_movs[2].first;
            pos_actual.second += posibles_movs[2].second;
        }
    }while(pos_actual != pair<int,int>(0,0));

    return sol;
}

int costeMinimoSonda(const vector<vector<int>> &mapa, vector<pair<int,int>> &sol){
    int f = mapa.size(), c = mapa[0].size();
    vector<vector<int>> C(f,vector<int>(c,INF));
    C[0][0] = mapa[0][0];

    for(int i = 0; i < f;i++){
        for(int j = 0; j < c; j++){
            if(i == 0 && j == 0){
                continue;
            }
            int min_coste = INF;
            if(i>0){
                min_coste = min(min_coste,C[i-1][j]);
            }
            if(j>0){
                min_coste = min(min_coste,C[i][j-1]);
            }
            if(i>0 && j>0){
                min_coste = min(min_coste,C[i-1][j-1]);
            }

            C[i][j] = min_coste + mapa[i][j];
        }
    }
    sol = construirSolucion(mapa,C);
    reverse(sol.begin(),sol.end());

    return C[f-1][c-1];
}

int main(int argc, char** argv){
    vector<char> terrenos = {'T','P','A','L','M'};
    if(argc != 3){
        cerr << "El programa lleva 2 argumentos, el nº de filas y el nº de columnas del mapa en dicho orden." << endl;
        exit(1);
    }
    int filas = atoi(argv[1]), columnas = atoi(argv[2]);
    vector<vector<char>> mapa(filas,vector<char>(columnas,'X'));
    vector<vector<int>> costes(filas,vector<int>(columnas,-1));
    int aux;
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            aux = rand()%terrenos.size();
            mapa[i][j] = terrenos[aux];
            costes[i][j] = aux+1;
        }
    }

    mapa[0][0] = 'R';
    mapa[filas-1][columnas-1] = 'E';
    costes[0][0] = 0;

    cout << "Mapa:" << endl;

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            cout << mapa[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl << "Costes:" << endl;

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            cout << costes[i][j] << " ";
        }
        cout << endl;
    }

    vector<pair<int,int>> sol;
    int coste_min = costeMinimoSonda(costes, sol);
    cout << "Coste minimo: " << coste_min << " obtenido siguiendo el camino: {";

    for(int i = 0; i < sol.size()-1;i++){
        cout << "(" << sol[i].first << "," << sol[i].second << "),";
    }
    cout << "(" << sol[sol.size()-1].first << "," << sol[sol.size()-1].second << ")}" << endl;

}