#include <bits/stdc++.h>
using namespace std;
void transformString(int* t_str, int* r_str, int input_str_len) {
  map<int, map<int, int>> bucket_map;
  vector<int*> rank_vec;
  //int** t_str = ps->input_str;
  for (int i = 0; i < input_str_len/2; i++) {
    // cout << t_str[(2 * i)] << t_str[(2 * i) + 1] << endl;
    rank_vec.push_back(&(bucket_map[t_str[(2 * i)]][t_str[(2 * i) + 1]]));
  }
  int counter = 1;
  for (auto a : bucket_map) {
    for (auto b : a.second) {
      cout << a.first << " " << b.first << endl;
      // b.second = counter;
      bucket_map[a.first][b.first] = counter;
      counter++;
    }
  }
  for (int i = 0; i < input_str_len/2; i++) {
  // for (auto a : rank_vec) {
    r_str[i] = *rank_vec[i];
    // cout << *a << " ";
  }
}

int main() {
  int a[] = {2,1,2,3,4,3,INT_MAX,INT_MAX};
  int b[] = {1,2,1,1,1,2,2,1,2,2,2,1,INT_MAX,INT_MAX};
  int* r = new int[7];
  transformString(b, r, 12);
  for (int i = 0; i < 6; i++) {
    cout << r[i];
  }
    //std::cout << "Hello World!\n";
}
