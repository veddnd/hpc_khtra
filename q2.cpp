// parallel_sudoku_solver.cpp
// Compile: g++ -fopenmp -std=c++17 parallel_sudoku_solver.cpp -o sudoku_solver
// Run: ./sudoku_solver

#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

#define N 9

// Check if placing num at board[row][col] is valid
bool isSafe(int board[N][N], int row, int col, int num) {
    for(int x = 0; x < N; x++) {
        if(board[row][x] == num || board[x][col] == num) return false;
    }
    int startRow = row - row % 3, startCol = col - col % 3;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(board[i + startRow][j + startCol] == num) return false;
    return true;
}

// Solve Sudoku using parallel backtracking
bool solveSudoku(int board[N][N]) {
    int row = -1, col = -1;
    bool empty = false;
    for(int i = 0; i < N && !empty; i++) {
        for(int j = 0; j < N && !empty; j++) {
            if(board[i][j] == 0) { row = i; col = j; empty = true; }
        }
    }

    if(!empty) return true; // no empty cell, solved

    bool solved = false;

    // Try all numbers 1-9 in parallel
    #pragma omp parallel for shared(solved)
    for(int num = 1; num <= 9; num++) {
        if(solved) continue; // skip if already solved
        if(isSafe(board, row, col, num)) {
            int board_copy[N][N];
            memcpy(board_copy, board, sizeof(board_copy));
            board_copy[row][col] = num;

            if(solveSudoku(board_copy)) {
                #pragma omp critical
                {
                    memcpy(board, board_copy, sizeof(board_copy));
                    solved = true;
                }
            }
        }
    }
    return solved;
}

// Print Sudoku board
void printBoard(int board[N][N]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            cout << board[i][j] << " ";
        cout << "\n";
    }
}

int main() {
    int board[N][N] = {
        {5,3,0, 0,7,0, 0,0,0},
        {6,0,0, 1,9,5, 0,0,0},
        {0,9,8, 0,0,0, 0,6,0},

        {8,0,0, 0,6,0, 0,0,3},
        {4,0,0, 8,0,3, 0,0,1},
        {7,0,0, 0,2,0, 0,0,6},

        {0,6,0, 0,0,0, 2,8,0},
        {0,0,0, 4,1,9, 0,0,5},
        {0,0,0, 0,8,0, 0,7,9}
    };

    cout << "Original Sudoku:\n";
    printBoard(board);

    double start = omp_get_wtime();
    if(solveSudoku(board)) {
        double end = omp_get_wtime();
        cout << "\nSolved Sudoku:\n";
        printBoard(board);
        cout << "\nTime taken: " << (end - start) << "s\n";
    } else {
        cout << "No solution exists.\n";
    }

    return 0;
}
