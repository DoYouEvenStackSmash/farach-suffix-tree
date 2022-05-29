#include <bits/stdc++.h>
#include "basic_blocks.h"
#include "compacted_trie.h"
using namespace std;

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
  deleteST(s.sp_tab);
  //displayTrie(s.root);

  deleteCollectedTrie(s.root);
}

int main() {
  LCA_trie();
  return 0;
}
