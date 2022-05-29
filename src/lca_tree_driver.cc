#include <bits/stdc++.h>
#include "basic_blocks.h"
#include "compacted_trie.h"
using namespace std;

void displayLetterTrie(Node* root) {
  stack<Node*> node_s;
  node_s.push(root);
  Node* curr = nullptr;
  while (!node_s.empty()) {
    curr = node_s.top();
    if (!curr->_color) {
      cout << "node" << curr << " [label=" << '\"' << curr->Lv << "\"];" << endl;
      curr->_color = 1;
    }
    if (curr->_next_edge < curr->children.size()) {
      cout << "node" << curr << "->" << "node" << curr->children[curr->_next_edge]->target_node << "[label=\"";

      for (int i = 0; i < curr->children[curr->_next_edge]->label_len; i++) {
        if (*(curr->children[curr->_next_edge]->label + i) == INT_MAX) {
          cout << '$';
        } else {
          cout << char(*(curr->children[curr->_next_edge]->label + i));
        }
      }
      cout << "\"];" << endl;
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
      continue;
    } else if (curr->suffix_index != 0){
      cout << endl;
      // cout <<
      //cout << "arrived at leaf: " << curr->suffix_index << endl;
    } else {
      cout << "";
      curr->_next_edge = 0;
      //cout << "visited all nodes" << endl;
    }
    node_s.pop();
  }
}

void letterTrie(S &s) {
  Node* root = createTrieNode(0);

  transformString(s.input_string, s.rank_string);
  s.rank_string.push_back(TERM);

  for (int i = 0; i < s.rank_string.size(); i++) {
    insertSuffix(root, s.rank_string.data() + i, i + 1);
  }

  collectNodes(root);
  contractTrie(root);
  //displayTrie(root);
  vector<int> c_LCP;
  vector<int> c_A;
  buildArrays(root, &c_A, &c_LCP);
  deleteCollectedTrie(root);

  expand_sorted_suffix_array(&s.A_ext, &c_A);
  expand_LCP_array(&s.LCP_ext, &c_LCP, &s.A_ext, s.input_string.data());

  //create node as leaf to avoid edge_table leaks
  s.root = createTrieNode(0, true);

  constructFullOddTrie(s.root, s.LCP_ext, s.A_ext, s.input_string.data());
  //displayTrie(s.root);
  contractTrie(s.root);
  cout << "digraph G {" << endl;
  displayLetterTrie(s.root);
  cout << "}" << endl;
  deleteCollectedTrie(s.root);

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
  cout << "digraph G {" << endl;
  displayTrie(s.root);
  cout << "}" << endl;
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
  int max = s.input_string.size();
  s.input_string.push_back(TERM);
  constructFullOddTrie(se.root, se.LCP_ext, se.A_ext, s.input_string.data(), max);
  //s.input_string[s.input_string.size()]
  contractTrie(se.root);
  //displayTrie(se.root);
  deleteST(s.sp_tab);
  //displayTrie(s.root);
  deleteCollectedTrie(se.root);
  deleteCollectedTrie(s.root);
}

int main() {
  S s;
  s.input_string = {98,97,110,97,110,97,98,97,110,97,110,97, TERM};
    //,115, TERM, TERM};
    // 110,100,97,110,97,TERM, TERM};
  letterTrie(s);
  // LCA_trie();
  return 0;
}
