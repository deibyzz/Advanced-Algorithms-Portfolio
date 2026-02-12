#include <iostream>
#include <vector>
#include <set>
#include <string> // Para std::to_string en la serialización del tablero (opcional)
#include <numeric> // Para std::accumulate (opcional)

using namespace std;
struct Move {
    int r1, c1; // Ficha que se mueve
    int r2, c2; // Ficha que se salta
    int r3, c3; // Destino

    // Para imprimir el movimiento (opcional)
    void print() const {
        cout << "Mover de (" << r1 << "," << c1 << ") a (" << r3 << "," << c3 << "), saltando (" << r2 << "," << c2 << ")\n";
    }
};


class Board
{
private:
    const int ROWS = 7;
    const int COLS = 7;
    const char EMPTY = ' ';
    const char OOB = 'x';
    const char FICHA = 'o';
    vector<vector<char>> board;
public:
    Board() : board(ROWS, vector<char>(COLS)){
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if ((i < 2 || i > 4) && (j < 2 || j > 4)) { // Esquinas
                    board[i][j] = OOB;
                } else {
                    board[i][j] = FICHA; // Clavija
                }
            }
        }
        board[ROWS / 2][COLS / 2] = EMPTY; // Hueco central
    }

    Board(const Board& other): board(other.board){}

    bool isValidPos(int r, int c) {
        return r >= 0 && r < ROWS && c >= 0 && c < COLS;
    }

    int numFichas(){
        int count = 0;
        for (const auto& row : board) {
            for (int cell : row) {
                if (cell == FICHA) {
                    count++;
                }
            }
        }
        return count;
    }

    bool solucion() {
        return numFichas() == 1 && board[ROWS / 2][COLS / 2] == FICHA;
    }

    vector<Move> generaMovimientos(){
        vector<Move> moves;
        int dr[] = {-2, 2, 0, 0}; 
        int dc[] = {0, 0, -2, 2}; 

        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (board[r][c] == FICHA) { // Si hay una clavija en (r,c)
                    for (int i = 0; i < 4; ++i) { 
                        int r_jumped = r + dr[i] / 2;
                        int c_jumped = c + dc[i] / 2;
                        int r_dest = r + dr[i];
                        int c_dest = c + dc[i];

                        if (isValidPos(r_dest, c_dest) && board[r_dest][c_dest] == EMPTY && 
                            isValidPos(r_jumped, c_jumped) && board[r_jumped][c_jumped] == FICHA) { // Comprobacion de validez de destino y salto
                            moves.push_back({r, c, r_jumped, c_jumped, r_dest, c_dest});
                        }
                    }
                }
            }
        }
        return moves;
    }

    void hacerMovimiento(const Move& move){
        board[move.r1][move.c1] = EMPTY; // La clavija original se va
        board[move.r2][move.c2] = EMPTY; // La clavija saltada se elimina
        board[move.r3][move.c3] = FICHA; // La clavija original aterriza en el destino
    }

    void deshacerMovimiento(const Move& move){
        board[move.r1][move.c1] = FICHA;
        board[move.r2][move.c2] = FICHA;
        board[move.r3][move.c3] = EMPTY;
    }

    string toString(){
        string aux;
        for(int i = 0; i < ROWS; i++){
            for(int j = 0; j < COLS; j++){
                aux += board[i][j]; 
                if (j < COLS - 1) {   
                    aux += " | ";       
                }
            }
            aux += '\n';
        }
        return aux;
    }

    bool operator<(const Board& other) const{ // Necesario para el set
        return board < other.board;
    }
};

// ALGORITMO DE BACKTRACKING
bool senku_backtracking(Board& board, vector<Move>& solucion, set<Board>& estadosVisitados){
    if(board.solucion()){ // Hemos encontrado solucion
        return true;
    }

    if(estadosVisitados.count(board)){ // El tablero ya ha sido visitado (Evitamos ciclos)
        return false;
    }

    estadosVisitados.insert(board);
    vector<Move> movimientos = board.generaMovimientos();
    for(const auto& move : movimientos){
        board.hacerMovimiento(move);
        solucion.push_back(move);
        
        if(senku_backtracking(board,solucion,estadosVisitados)){ // Si siguiendo por este camino encontramos solucion
            return true;
        }

        // No hemos encontrado solucion, por tanto deshacemos
        board.deshacerMovimiento(move);
        solucion.pop_back();
    }

    return false;
}

vector<Move> solve_senku(const Board& board){
    Board aux(board);
    vector<Move> solucion;
    set<Board> estados;
    if(senku_backtracking(aux,solucion,estados)){
        cout << "Solucion encontrada\n";
    }
    else{
        cout << "No hay solucion\n";
        solucion.clear();
    }

    return solucion;
}


int main(int argc, char** argv){
    Board board;
    vector<Move> solucion = solve_senku(board);
    cout << board.toString();
    for(const auto& move : solucion){
        move.print();
        board.hacerMovimiento(move);
        cout << board.toString();
    }

    if(board.solucion()){
        cout << "Estado solucion:\n";
        cout << board.toString();
    }
    
    return 0;
}