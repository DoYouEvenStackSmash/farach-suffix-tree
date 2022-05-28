#ifndef COMPACTED_TRIE_H
#define COMPACTED_TRIE_H
#include "basic_blocks.h"

using namespace std;

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

void eulerTour(Node* root, int suffix_count);

#endif
