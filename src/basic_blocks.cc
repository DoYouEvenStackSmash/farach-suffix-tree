#include <bits/stdc++.h>
#include "basic_blocks.h"

using namespace std;

/*
  Creates an Edge pointing to a Node
  if Edge has edge_label != nullptr, label_len > 0
*/
Edge* createTrieEdge(Node* target, int* edge_label, int label_len) {
  Edge* E = new Edge;

  E->target_node = target;
  E->label = edge_label;
  E->label_len = label_len;

  return E;
}

/*
  Creates a Node
*/
Node* createTrieNode(int level, bool IS_LEAF_NODE, int suffix_index) {
  Node* N = new Node;

  N->Lv = level;
  N->_color = 0;
  N->_next_edge = 0;

  if (!IS_LEAF_NODE)
    N->edge_table = new Edge*[ALPHABET_SIZE]();

  N->suffix_index = suffix_index;

  return N;
}
