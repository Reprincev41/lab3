#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;
double calculateY(double x) {
    if (x >= -3 && x < -2) {
        return -x - 2;
    } 
    else if (x >= -2 && x < 0) {
        return sqrt(1 - pow(x + 1, 2));
    } 
    else if (x >= 0 && x <= 4) {
        return -sqrt(4 - pow(x - 2, 2));
    } 
    else if (x > 4 && x <= 6) {
        return -0.5 * x + 2;
    } 
    else if (x > 6 && x <= 7) {
        return -1;
    }
    return NAN;
}

void printResults(double x_start, double x_end, double dx) {
    cout << fixed << setprecision(2);
    cout << "X\tY\n";
    for (double x = x_start; x <= x_end; x += dx) {
        double y = calculateY(x);
        if (!isnan(y)) {
            cout << "||" << x << "||" << "\t" << "||" << y << "||" << "\n";
        }
    }
}
int main() {
    double x_start = -3.0;
    double x_end = 7.0;
    double dx = 0.1;
    printResults(x_start, x_end, dx);
    return 0;
}
