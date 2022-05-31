#include <bits/stdc++.h>
#include "basic_blocks.h"
#include "compacted_trie.h"
using namespace std;

bool findSequence(Node* root, int* seq) {
  stack<Edge*> edge_s;
  Edge* base = createTrieEdge(root);
  edge_s.push(base);
  Node* curr = nullptr;
  int* temp_seq = seq;
  int c = 0;
  int lc = 0;
  int h = 0;
  int END = -1;
  bool FOUND_FLAG = false;
  bool BREAK_FLAG = false;
  while (temp_seq[h] != END && !edge_s.empty()) {
    curr = edge_s.top()->target_node;
    while (c < curr->children.size()) {
      if (temp_seq[h] != curr->children[c]->label[0]) {
        c++;
      } else {
        lc = 0;
        for (int i = 0; i < curr->children[c]->label_len; i++) {
          if (temp_seq[h + i] == curr->children[c]->label[i]) {
            lc++;
            continue;
          }
          if (temp_seq[h + i] == END) {
            FOUND_FLAG = true;
            h = h + i;
            break;
          } else if (curr->children[c]->label[i] == TERM) {
            BREAK_FLAG = true;
            break;
          } else {
            BREAK_FLAG = true;
            break;
          }
        }
        if (BREAK_FLAG) {
          BREAK_FLAG = false;
          c++;
        } else if (FOUND_FLAG) {
          break;
        } else if (lc == curr->children[c]->label_len) {
          h = h + lc;
          edge_s.push(curr->children[c]);
          c = 0;
          break;
        }

        // implies the temp_sequence matched the edge label

      }
    }
    if (c == curr->children.size()) {
      break;
      // edge_s.pop();
    }

  }
  if (FOUND_FLAG) {
    cout << "STRING FOUND" << endl;
  } else {
    cout << "STRING NOT FOUND" << endl;
  }
  //cout << "H val: " << h << endl;
  while (!edge_s.empty()) {
    //cout << "popping!" << endl;
    edge_s.pop();
  }
  delete base;
  return FOUND_FLAG;
}

void searchRankTrie(S &s) {
  Node* root = createTrieNode(0);

  transformString(s.input_string, s.rank_string);
  s.rank_string.push_back(TERM);

  for (int i = 0; i < s.rank_string.size(); i++) {
    insertSuffix(root, s.rank_string.data() + i, i + 1);
  }

  collectNodes(root);
  contractTrie(root);
  displayTrie(root);
  vector<int> search_string;
  vector<vector<int>> search_strings = {{3,4,3,4,-1},{3,4,-1},{4,4,-1}, {2,3,4,-1}};
  for (int i = 0; i < search_strings.size(); i++) {
    search_string = search_strings[i];
    cout << "searching for:\t";
    for (auto a : search_string)
      cout << a << " ";
    cout << endl;
    if (findSequence(root, search_string.data()))
      cout << "found string!" << endl;
    else
      cout << "did not find string." << endl;
  }
  deleteCollectedTrie(root);
}

void LCA_trie(S &s) {
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
  contractTrie(s.root);
  displayTrie(s.root);
  deleteCollectedTrie(s.root);
}

void prep_search(S &s) {
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
  contractTrie(s.root);
}

int main() {
  S s;
  s.input_string = {1,2,1,1,1,2,2,1,2,2,2,1,TERM};
  prep_search(s);
  vector<vector<int>> suffixes = {{1,2,1,1,1,2,2,1,2,2,2,1,-1}, {2,1,1,1,2,2,1,2,2,2,1,-1}, {1,1,1,2,2,1,2,2,2,1,-1}, {1,1,2,2,1,2,2,2,1,-1}, {1,2,2,1,2,2,2,1,-1}, {2,2,1,2,2,2,1,-1}, {2,1,2,2,2,1,-1}, {1,2,2,2,1,-1}, {2,2,2,1,-1}, {2,2,1,-1}, {2,1,-1}};
  // findSequence(s.root, suffixes[1]);
  vector<int> search_string = suffixes[1];
  for (int i = 0; i < suffixes.size(); i++) {
    search_string = suffixes[i];
    cout << "searching for: " << i << "\t";
    for (auto a : search_string)
      cout << a << " ";
    cout << endl;
    if (findSequence(s.root, search_string.data()))
      cout << "found string!" << endl;
    else
      cout << "did not find string." << endl;
  }
  deleteCollectedTrie(s.root);
  // searchRankTrie(s);
  // LCA_trie(s);
  return 0;
}
