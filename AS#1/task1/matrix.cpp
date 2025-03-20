#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main() {
    srand(time(0));
    int random_num = rand() % 99 + 10; // 10 to 99
    int genNum = random_num * 4;
    int rollNo = 230944;
    genNum = rollNo / genNum;
    genNum = genNum % 25;
    if (genNum < 10) {
        genNum += 15;
    }
    cout << "genNumber is: " << genNum << endl;

    // Create and populate matrix1 and matrix2
    int matrix1[genNum][genNum], matrix2[genNum][genNum];
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            matrix1[i][j] = rand() % 10 + 1;
            matrix2[i][j] = rand() % 10 + 1;
        }
    }

    // Print Matrix 1
    cout << "\nMatrix 1 (" << genNum << "x" << genNum << "):\n";
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            cout << matrix1[i][j] << " ";
        }
        cout << endl;
    }

    // Print Matrix 2
    cout << "\nMatrix 2 (" << genNum << "x" << genNum << "):\n";
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            cout << matrix2[i][j] << " ";
        }
        cout << endl;
    }

    // Save both matrices to matrix.txt
    ofstream outfile("matrix.txt");
    if (!outfile) {
        cerr << "Error opening file!\n";
        return 1;
    }

    outfile << "Matrix 1:\n";
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            outfile << matrix1[i][j] << " ";
        }
        outfile << "\n";
    }

    outfile << "Matrix 2:\n";
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            outfile << matrix2[i][j] << " ";
        }
        outfile << "\n";
    }
    outfile.close();
    cout << "Matrices saved to matrix.txt\n";

    // Create genNum child processes for matrix multiplication
    pid_t pids[genNum];
    for (int i = 0; i < genNum; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            cerr << "Fork failed for child " << i << "!\n";
            return 1;
        }

        if (pids[i] == 0) {
            // Child process: Compute row i of the result matrix
            int row_result[genNum];
            for (int j = 0; j < genNum; j++) {
                row_result[j] = 0;
                for (int k = 0; k < genNum; k++) {
                    row_result[j] += matrix1[i][k] * matrix2[k][j];
                }
            }

            // Save result to file named after child PID
            string filename = to_string(getpid()) + ".txt";
            ofstream rowfile(filename);
            rowfile << "Child PID: " << getpid() << "\n";
            rowfile << "Row " << i << " result: ";
            for (int j = 0; j < genNum; j++) {
                rowfile << row_result[j] << " ";
            }
            rowfile << "\n";
            rowfile.close();

            exit(0);
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < genNum; i++) {
        waitpid(pids[i], nullptr, 0);
    }

    // Parent reads all row files and constructs the result matrix
    int result_matrix[genNum][genNum];
    for (int i = 0; i < genNum; i++) {
        string filename = to_string(pids[i]) + ".txt";
        ifstream rowfile(filename);
        if (!rowfile) {
            cerr << "Error opening file " << filename << "!\n";
            return 1;
        }

        string line;
        getline(rowfile, line); // Skip "Child PID" line
        rowfile >> line >> i; // Skip "Row" and row number
        rowfile >> line; // Skip "result:"

        for (int j = 0; j < genNum; j++) {
            rowfile >> result_matrix[i][j];
        }
        rowfile.close();
    }

    // Print the final result matrix
    cout << "\nResultant Matrix (" << genNum << "x" << genNum << "):\n";
    for (int i = 0; i < genNum; i++) {
        for (int j = 0; j < genNum; j++) {
            cout << result_matrix[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
