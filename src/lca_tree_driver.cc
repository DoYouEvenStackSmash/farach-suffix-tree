#include <bits/stdc++.h>
#include "basic_blocks.h"
#include "compacted_trie.h"
using namespace std;

void even_sort(int* input_string, vector<int> A_ext, vector<int> &rv) {
  map<int, vector<int>> m;
  for(int i = 0; i < A_ext.size(); i++) {
    if (A_ext[i] - 2 < 0)
      continue;
    m[input_string[A_ext[i] - 2]].push_back(A_ext[i]);
  }

  for (auto a : m) {
    for (auto b : a.second)
      rv.push_back(b - 1);
  }
}

int RMQ(SparseTable* lca, int x, int y) {
  int L = lca->rep[x];
  int R = lca->rep[y];
  if (R < L)
    swap(R, L);
  int j = lca->log_memo[R - L + 1];
  return min(lca->st[L][j], lca->st[R - (1 << j) + 1][j]);
}

void evenLCP(int* str, SparseTable* lca, vector<int> &A_ext, vector<int> &LCP_ext) {
  int L;
  int R;
  for (int i = 1; i < A_ext.size(); i++) {
    L = A_ext[i - 1];
    R = A_ext[i];
    if (str[L - 1] != str[R - 1]) {
      LCP_ext.push_back(0);
    } else {
      LCP_ext.push_back(RMQ(lca, L + 1, R + 1) + 1);
    }
  }
}

void LCA_trie() {
  S s;

  Node* root = createTrieNode(0);

  s.input_string = {1,2,1,1,1,2,2,1,2,2,2,1,TERM};

  transformString(s.input_string, s.rank_string);
  s.rank_string.push_back(TERM);

  for (int i = 0; i < s.rank_string.size(); i++) {
    insertSuffix(root, s.rank_string.data() + i, i + 1);
  }

  collectNodes(root);
  contractTrie(root);

  vector<int> c_LCP;
  vector<int> c_A;
  buildArrays(root, &c_A, &c_LCP);
  deleteCollectedTrie(root);

  expand_sorted_suffix_array(&s.A_ext, &c_A);
  expand_LCP_array(&s.LCP_ext, &c_LCP, &s.A_ext, s.input_string.data());

  //create node as leaf to avoid edge_table leaks
  s.root = createTrieNode(0, true);

  constructFullOddTrie(s.root, s.LCP_ext, s.A_ext, s.input_string.data());

  preprocessLCA(s);
  S se;
  even_sort(s.input_string.data(), s.A_ext, se.A_ext);
  /*
  cout << "A_ext" << endl;
  for (auto a : se.A_ext)
    cout << a << " ";
  cout << endl;
  */
  int* str = s.input_string.data();
  SparseTable* lca = s.sp_tab;
  evenLCP(str, s.sp_tab, se.A_ext, se.LCP_ext);
  /*
  cout << "LCP_ext" << endl;
  for (auto a : se.LCP_ext)
    cout << a << " ";
  cout << endl;
  */
  se.root = createTrieNode(0, true);
  constructFullOddTrie(se.root, se.LCP_ext, se.A_ext, s.input_string.data());
  //s.input_string[s.input_string.size()]
  contractTrie(se.root);
  displayTrie(se.root);
  deleteST(s.sp_tab);
  //displayTrie(s.root);
  deleteCollectedTrie(se.root);
  deleteCollectedTrie(s.root);
}

int main() {
  LCA_trie();
  return 0;
}
