#include <iostream>
#include <cstdlib>
#include <unistd.h>

//Move OS defines up here to be used in different places
#if defined(linux) || defined(__CYGWIN__)
  #define OS_LINUX
#elif (defined(__APPLE__) || defined(__OSX__) || defined(__MACOS__)) && defined(__MACH__)//To ensure that we are running on a mondern version of macOS
  #define OS_MAC
#endif

using namespace std;

class Gol {
    public:
      bool** old_board;
      bool** new_board;
      int board_size;

      Gol(int size, int coord_size, int* board_coordinate){
          // initialise the board for creating the game of life
          board_size = size;

          new_board = 0;
          new_board = new bool*[board_size];

          old_board = 0;
          old_board = new bool*[board_size];

          for(int i=0; i<board_size; i++){
              new_board[i] = new bool[size];
              old_board[i] = new bool[size];
              for(int j=0; j<board_size; j++){
                new_board[i][j] = false;
                old_board[i][j] = false;
              }
          }

          for(int i=0; i<coord_size; i=i+2){
              int r = board_coordinate[i];
              int c = board_coordinate[i+1];
                new_board[r][c] = true;
                old_board[r][c] = true;
          }
      }

      void print_board();
      int count_neighbours(int r, int c);
      void update_boards();
};

void Gol::print_board(){
    // print the board for creating the game of life
    for(int i=0; i<board_size; i++){
        for(int j=0; j<board_size; j++){
            if(new_board[i][j] == false){
                cout << "0 ";
            } else{
                cout << "+ ";
            }
        }
        cout << endl;
    }
}

int Gol::count_neighbours(int r, int c){
    int count = 0;
    int h_start = (c==0) ? 0: -1;
    int h_end = (c==board_size-1) ? 0: 1;
    int v_start = (r==0) ? 0: -1;
    int v_end = (r==board_size-1) ? 0: 1;

    // cout << '(' << r << ',' << c << ')' << endl;
    // cout << h_start << ',' << h_end << endl;
    // cout << v_start << ',' << v_end << endl;

    for(int i = v_start; i<=v_end; i++){
        for(int j = h_start; j<=h_end; j++){
            if(!(i==0 && j==0)){
                count += int(new_board[r+i][c+j]);
            }
        }
    }
    return count;
}

void Gol::update_boards(){
    // update the new board based and swap with the old one
    int neighb_count = 0;

    for(int i=0; i<board_size; i++){
        for(int j=0; j<board_size; j++){
            old_board[i][j] = false;
        }
    }

    for(int i=0; i<board_size; i++){
        for(int j=0; j<board_size; j++){
            neighb_count = count_neighbours(i, j);
            if (neighb_count==3){
                old_board[i][j] = true;
            }
            else if (new_board[i][j] && neighb_count==2){
                old_board[i][j] = true;
            }
            else{
                old_board[i][j] = false;
            }
        }
    }

    bool** tmp = new_board;
    new_board = old_board;
    old_board = tmp;
}


// void clearScreen(void) {
//   // Tested and working on Ubuntu and Cygwin
//   #if defined(OS_LINUX) || defined(OS_MAC)
//     cout << "\033[2J;" << "\033[1;1H"; // Clears screen and moves cursor to home pos on POSIX systems
//   #endif
// }
//
// int main(){
//     clearScreen();
//     int board_size = 30;
//     int board_coord[]  = {0, 1, 1, 2, 2, 0, 2, 1, 2, 2};
//     int coord_size = sizeof(board_coord)/sizeof(*board_coord);
//     Gol game (board_size, coord_size, board_coord);
//     while (true){
//         cout << endl;
//         game.print_board();
//         game.update_boards();
//         usleep(500000);
//         clearScreen();
//     }
// }
