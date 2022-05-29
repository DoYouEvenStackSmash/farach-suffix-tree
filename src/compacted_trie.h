#ifndef COMPACTED_TRIE_H
#define COMPACTED_TRIE_H
constexpr size_t TERM = INT_MAX;
#include "basic_blocks.h"

using namespace std;

struct SparseTable {
  int max_n;
  int* log_memo;
  int** st;
  int* rep;
};

struct S {
  Node* root;
  vector<int> input_string;
  vector<int> rank_string;
  vector<int> LCP_ext;
  vector<int> A_ext;
  SparseTable* sp_tab;
};


void insertSuffix(Node* root, int* start, int suffix_index);

void collectNodes(Node* root);

void contractTrie(Node* root);

void deleteCollectedTrie(Node* root);

void displayTrie(Node* root);

void transformString(vector<int> &t_str, vector<int> &rv);

void buildArrays(Node* root, vector<int> *sort_array, vector<int> *LCP_array);

void expand_sorted_suffix_array(vector<int>* A_ext, vector<int>* A);

void expand_LCP_array(vector<int>* LCP_ext, vector<int>* LCP, vector<int> *A_ext, int* S);

void constructFullOddTrie(Node* root, vector<int>& LCP, vector<int>& A, int* S);

//void eulerTour(Node* root, int suffix_count);
void eulerTour(Node* root, vector<int> &level, vector<int> &rep);

void constructST(SparseTable* ST, vector<int> &L, vector<int> &R);

void preprocessLCA(S &s);

void deleteST(SparseTable* ST);

#endif
