#include <bits/stdc++.h>
using namespace std;

#define ALPHABET_SIZE 256
#define END INT_MAX
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

/*
  Creates an Edge pointing to a Node
  if Edge has edge_label != nullptr, label_len > 0
*/
Edge* createTrieEdge(Node* target, int* edge_label = nullptr, int label_len = 0) {
  Edge* E = new Edge;

  E->target_node = target;
  E->label = edge_label;
  E->label_len = label_len;

  return E;
}
/*
  Creates a Node
*/
Node* createTrieNode(int level, bool IS_LEAF_NODE = false, int suffix_index = 0) {
  Node* N = new Node;

  N->Lv = level;
  N->_color = 0;
  N->_next_edge = 0;

  if (!IS_LEAF_NODE)
    N->edge_table = new Edge*[ALPHABET_SIZE]();

  N->suffix_index = suffix_index;

  return N;
}

/*
  inserts a substring, id = suffix_index
*/
void insertSuffix(Node* root, int* start, int suffix_index) {
  Node* curr_node = root;
  int* curr_char = start;
  int lv = 1;

  while (*curr_char != END) {
    // curr_node cannot be a leaf node, therefore must have edge_table
    if (!curr_node->edge_table[*curr_char]) {
      curr_node->edge_table[*curr_char] = createTrieEdge(createTrieNode(lv), curr_char, 1);
    }
    curr_node = curr_node->edge_table[*curr_char]->target_node;
    curr_char++;
    lv++;
  }

  if (*curr_char != END) {
    cout << "somehow not end, but end" << endl;
  } else if (curr_node->edge_table[ALPHABET_SIZE - 1]) {
    cout << "somehow inserted a duplicate suffix?" << endl;
  } else {
    curr_node->edge_table[ALPHABET_SIZE - 1] = createTrieEdge(createTrieNode(lv, true, suffix_index), curr_char, 1);
  }
}

/*
  consolidates the edges of each node in BFS manner
  moves Edges from Node.edge_table to Node.children
  deletes edge_table
*/
void collectNodes(Node* root) {
  queue<Node*> node_q;
  node_q.push(root);
  Node* curr = nullptr;
  while (!node_q.empty()) {
    curr = node_q.front();
    node_q.pop();
    if (curr->edge_table == nullptr) {
      //cout << "Leaf node encountered: " << curr->suffix_index << endl;
      continue;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
      if (curr->edge_table[i] != nullptr) {
        curr->children.push_back(curr->edge_table[i]);
        node_q.push(curr->edge_table[i]->target_node);
      }
    }
    delete[] curr->edge_table;
  }
}

/*
  deletes entire trie, in BFS manner
  NOTE: Trie must be collected FIRST, because this function iterates
    over Node.children edge objects.
*/
void deleteCollectedTrie(Node* root) {
  queue<Node*> node_q;
  node_q.push(root);
  Node* curr = nullptr;
  while (!node_q.empty()) {
    curr = node_q.front();
    node_q.pop();
    if (curr->children.size() == 0) {
      //cout << "deleting leaf node: " << curr->suffix_index << endl;
      delete curr;
      continue;
    }
    for (int i = 0; i < curr->children.size(); i++) {
      node_q.push(curr->children[i]->target_node);
      delete curr->children[i];
    }
    delete curr;
  }
}

/*
  Trie simple path contraction.
  Easier to explain diagramatically.
*/
void contractTrie(Node* root) {
  stack<Edge*> edge_s;
  Edge* base = createTrieEdge(root);
  edge_s.push(base);
  Node* curr = nullptr;

  while (!edge_s.empty()) {
    curr = edge_s.top()->target_node;

    if (curr->children.size() == 1) {
      edge_s.top()->label_len = edge_s.top()->label_len + curr->children[0]->label_len;
      edge_s.top()->target_node = curr->children[0]->target_node;
      delete curr->children[0];
      delete curr;

    } else if (curr->_next_edge < curr->children.size()) {
      edge_s.push(curr->children[curr->_next_edge]);
      curr->_next_edge++;

    } else {
      curr->_next_edge = 0;
      edge_s.pop();
    }
  }
  delete base;
}

/*
  Traverses the Trie in DFS manner, printing in dot format
  Edge labels are printed on respective edges
  Node level, (leaf level if contracted) is printed inside the node

  Derivative of the visit leaf depth first traversal
*/
void displayTrie(Node* root) {
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
          cout << *(curr->children[curr->_next_edge]->label + i);
        }
      }
      cout << "\"];" << endl;
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
      continue;
    } else if (curr->suffix_index != 0){
      cout << endl;

    } else {
      cout << "";
      curr->_next_edge = 0;

    }
    node_s.pop();
  }
}

/*
  transform input string t_str to a ranked string
  bucket sort using nested maps, keys of length 2
  iterate over map, assign monotonic increasing int
  copy into return vector rv
*/
void transformString(vector<int> &t_str, vector<int> &rv) {
  map<int, map<int, int>> bucket_map;
  vector<int*> rank_vec;
  int input_str_len = t_str.size();
  for (int i = 0; i < input_str_len/2; i++) {
    rank_vec.push_back(&(bucket_map[t_str[(2 * i)]][t_str[(2 * i) + 1]]));
  }
  int counter = 1;
  for (auto a : bucket_map) {
    for (auto b : a.second) {
      bucket_map[a.first][b.first] = counter;
      counter++;
    }
  }

  for (auto a : rank_vec)
    rv.push_back(*a);
}

void input_transform() {
  vector<int> T = {1,2,1,1,1,2,2,1,2,2,2,1,END};
  vector<int> rv;
  transformString(T, rv);
  for (auto a : rv)
    cout << a << ",";
}

void create_tree() {
  Node* root = createTrieNode(0);
  vector<int> T = {1,2,1,1,1,2,2,1,2,2,2,1,END};
  for (int i = 0; i < T.size(); i++) {
    insertSuffix(root, T.data() + i, i + 1);
  }
  collectNodes(root);
  contractTrie(root);
  displayTrie(root);
  deleteCollectedTrie(root);

}

int main() {
  input_transform();
  create_tree();
  return 0;
}
