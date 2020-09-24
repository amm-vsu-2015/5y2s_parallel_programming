#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <omp.h>
#include "math.h"

using namespace std;

const double alpha = 0.3;
const double beta = 0.5;

double f(int x) {
  return 2 * cos((alpha + beta * x) / 2) * cos((alpha - x * beta) / 2);
}

// from a to b, n segments
double integral_right_rect(const double& a, const double& b, const double& nseg, const double& frac) {
    double dx = 1.0 * (b - a) / nseg;
    double sum = 0.0;
    double xstart = a + frac * dx;

    int threads = 0;
    #pragma omp parallel for schedule(static, 4)
    for(int i = 0; i < 1000; i++) {
      sum += f(xstart + i * dx);
    }

    return sum * dx;
}

void print_formatted(string label, double value) {
  stringstream stream;
  stream << fixed << setprecision(15) << value;
  cout << label << stream.str() << endl;
}

int main(int argc, char *argv[]) {
  double t_start, t_end, t_diff;

  double max_time = 0;
  double min_time = 1;

  for (int counter = 0; counter < 1000000; counter++) {
    t_start = omp_get_wtime();
    double result = integral_right_rect(0, 50, 1500, 1.0);
    t_end = omp_get_wtime();
    t_diff = t_end - t_start;

    if (t_diff > max_time) max_time = t_diff;
    if (t_diff < min_time) min_time = t_diff;
  }

  cout << "SCHEDULE TYPE: (static)" << '\n';

  print_formatted("min time: ", min_time);
  print_formatted("max time: ", max_time);
  print_formatted("delta time: ", max_time - min_time);

  return 0;
}
