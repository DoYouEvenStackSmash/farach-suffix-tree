#ifndef BASIC_BLOCKS_H
#define BASIC_BLOCKS_H
using namespace std;
constexpr size_t ALPHABET_SIZE = 256;
struct Edge;

/*
  Lv            | depth of Node in Tree
  edge_table    | array used for constant time lookup during pre contract ops
  children      | vector containing all child edges of Node
  suffix_index  | identifier for leaf, 0 for internal
  _color        | internal use during contract
  _next_edge    | internal use during traversal ops
*/

struct Node {
  int Lv;
  Edge** edge_table = nullptr;
  vector <Edge*> children;
  int suffix_index;
  int _color;
  int _next_edge;
};

/*
  label       | edge label start address
  label_len   | size of edge label
  target_node | target of directed edge
*/
struct Edge {
  int* label;
  int label_len;
  Node* target_node;
};

Edge* createTrieEdge(Node* target, int* edge_label = nullptr, int label_len = 0);

Node* createTrieNode(int level, bool IS_LEAF_NODE = false, int suffix_index = 0);

#endif
