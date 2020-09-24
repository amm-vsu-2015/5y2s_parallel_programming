#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <omp.h>
#include "math.h"
#include <vector>

using namespace std;

void print_formatted(string label, double value) {
  stringstream stream;
  stream << fixed << setprecision(9) << value;
  cout << label << stream.str() << endl;
}

struct PointD {
  double x;
  double y;

  PointD(double X, double Y) {
      x = X; y = Y;
  }
};

template<class T> bool isPointInPolygon(const T &test, const vector<PointD> &polygon) {
  static const int q_patt[2][2]= { {0,1}, {3,2} };

  if (polygon.size() < 3) return false;

  vector<PointD>::const_iterator end = polygon.end();

  T pred_pt = polygon.back();
  pred_pt.x -= test.x;
  pred_pt.y -= test.y;

  int pred_q = q_patt[pred_pt.y < 0][pred_pt.x < 0];
  int w = 0;

  #pragma omp parallel for schedule(static)
  for(vector<PointD>::const_iterator iter=polygon.begin(); iter!=end; ++iter) {
    T cur_pt = *iter;

    cur_pt.x -= test.x;
    cur_pt.y -= test.y;

    int q = q_patt[cur_pt.y < 0][cur_pt.x < 0];

    switch (q - pred_q) {
      case -3: ++w; break;
      case 3: --w; break;
      case -2: if(pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x) ++w; break;
      case 2: if(!(pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x)) --w; break;
    }

    pred_pt = cur_pt;
    pred_q = q;

  }
  return w != 0;
}

int main() {

  vector<PointD> shape;
  shape.push_back(PointD(20, 0));
  shape.push_back(PointD(0,  30));
  shape.push_back(PointD(40, 80));
  shape.push_back(PointD(150,30));
  shape.push_back(PointD(60, 0));
  shape.push_back(PointD(60, 0));

  double inner_points_counter = 0;
  double all_points_counter = 0;

  double t_start, t_end, t_diff;
  double max_time = 0;
  double min_time = 35;

  int random_range = 2000;
  int experiments = 1000;

  omp_set_max_active_levels(2);
  #pragma omp parallel for schedule(static) lastprivate(t_start, t_end, t_diff) reduction(+:inner_points_counter,all_points_counter)
  for (unsigned exp_idx = 0; exp_idx < experiments; exp_idx++) {
    srand((unsigned)time(NULL));
    t_start = omp_get_wtime();

    #pragma omp parallel for schedule(static) reduction(+:inner_points_counter,all_points_counter)
    for (unsigned i = 0; i < 1000000; i++) {
       unsigned x = static_cast<int>((double)(rand() % random_range));
       unsigned y = static_cast<int>((double)(rand() % random_range));
       PointD point = PointD(x,y);
       bool value = isPointInPolygon(point, shape);

       if (value == 1)
         inner_points_counter += 1;

       all_points_counter += 1;
     }
     t_end = omp_get_wtime();
     t_diff = t_end - t_start;

     #pragma omp critical(min_val)
     {
       if (t_diff > max_time) max_time = t_diff;
       if (t_diff < min_time) min_time = t_diff;
     }
   }

  print_formatted("area raw: ", (inner_points_counter / all_points_counter));
  print_formatted("area of shape: ", (inner_points_counter / all_points_counter) * pow(random_range, 2));
  print_formatted("inner points: ", inner_points_counter);
  print_formatted("total points: ", all_points_counter);

  print_formatted("min time: ", min_time);
  print_formatted("max time: ", max_time);
  print_formatted("diff time: ", max_time - min_time);
  return 0;
}
