#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <omp.h>
#include "math.h"

using namespace std;

const int MATRIX_SIZE = 10000;
const int MIN_VALUE = 0;
const int MAX_VALUE = 10000000;

int** makeEmptyArray() {
  int** array = new int*[MATRIX_SIZE];
  for (int i = 0; i < MATRIX_SIZE; ++i) array[i] = new int[MATRIX_SIZE];
  return array;
}

int** generateRandomArray(int minValue, int maxValue) {
  int** randomArray = makeEmptyArray();
  #pragma omp parallel for schedule(guided)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      randomArray[i][j] = rand() % (maxValue - minValue + 1) + minValue;
    }
  }
  return randomArray;
}

int** sumArrays(int** array1, int** array2) {
  int** sumArray = makeEmptyArray();
  #pragma omp parallel for schedule(guided)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      sumArray[i][j] = array1[i][j] * array2[i][j];
    }
  }
  return sumArray;
}

int getMaxFromArray(int** array) {
  int max_value = MIN_VALUE;
  #pragma omp parallel for schedule(guided)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      if (max_value < array[i][j]) {
        max_value = array[i][j];
      }
    }
  }
  return max_value;
}

int getMinFromArray(int** array) {
  int min_value = MAX_VALUE;
  #pragma omp parallel for schedule(guided)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      if (min_value > array[i][j]) {
        min_value = array[i][j];
      }
    }
  }
  return min_value;
}

void print_formatted(string label, double value) {
  stringstream stream;
  stream << fixed << setprecision(2) << value;
  cout << label << stream.str() << endl;
}

int main(int argc, char *argv[]) {
  double t_start, t_end, t_diff;
  double max_time = 0;
  double min_time = 20;
  double max_value = MIN_VALUE;
  double min_value = MAX_VALUE;

  for (int counter = 0; counter < 40; counter++) {
    t_start = omp_get_wtime();
    int** firstArray = generateRandomArray(10000, 35000);
    int** secondArray = generateRandomArray(5000, 15000);
    int** sumArray = sumArrays(firstArray, secondArray);
    int min = getMinFromArray(sumArray);
    int max = getMaxFromArray(sumArray);
    t_end = omp_get_wtime();
    t_diff = t_end - t_start;

    if (t_diff > max_time) max_time = t_diff;
    if (t_diff < min_time) min_time = t_diff;

    if (max > max_value) max_value = max;
    if (min < min_value) min_value = min;
  }

  print_formatted("min value: ", max_value);
  print_formatted("max value: ", min_value);

  print_formatted("min time: ", min_time);
  print_formatted("max time: ", max_time);
  print_formatted("diff time: ", max_time - min_time);

  return 0;
}
