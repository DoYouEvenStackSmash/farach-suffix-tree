#include "basic_blocks.h"
#include "compacted_trie.h"

using namespace std;

/*
  inserts a substring, id = suffix_index
*/
void insertSuffix(Node* root, int* start, int suffix_index) {
  Node* curr_node = root;
  int* curr_char = start;
  int lv = 1;

  while (*curr_char != TERM) {
    // curr_node cannot be a leaf node, therefore must have edge_table
    if (!curr_node->edge_table[*curr_char]) {
      curr_node->edge_table[*curr_char] = createTrieEdge(createTrieNode(lv), curr_char, 1);
    }
    curr_node = curr_node->edge_table[*curr_char]->target_node;
    curr_char++;
    lv++;
  }

  if (*curr_char != TERM) {
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
  transform input string t_str to a ranked string
  bucket sort using nested maps, keys of length 2
  iterate over map, assign monotonic increasing int
  Eliminates duplicates: otherwise the counter becomes much larger than the string
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

/*
  replace simple path p = {u, ..., p} with a single edge(u,v)
  maximal branch-free paths are replaced by edges labeled
    by the appropriate substring
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
  Traverses the Trie in DFS manner
  builds sort_array containing suffix positions in lexicographic order
  builds LCP array, least common ancestor of adjacent strings in sort_array

  Derivative of the visit leaf depth first traversal
*/
void buildArrays(Node* root, vector<int> *sort_array, vector<int> *LCP_array){
  int last_peak = -1;
  bool peak_set = true;
  stack<Node*> node_s;
  node_s.push(root);
  Node* curr = nullptr;

  while (!node_s.empty()) {
    curr = node_s.top();
    if (curr->_next_edge < curr->children.size()) {
      if (!peak_set) {
        //save highest point
        last_peak = curr->Lv;
        peak_set = true;
      }
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
      continue;

    } else if (curr->suffix_index != 0){
      //making sure LCP.size() == sort_array.size() - 1
      if (last_peak != -1)
        LCP_array->push_back(last_peak);
      sort_array->push_back(curr->suffix_index);
      peak_set = false;
    } else {
      curr->_next_edge = 0;
    }
    node_s.pop();
  }
}

/*
  expand lexicographically ordered suffix array
*/
void expand_sorted_suffix_array(vector<int>* A_ext, vector<int>* A) {
  for (int i = 0; i < A->size(); i++) {
    A_ext->push_back((2 * (*A)[i]) - 1);
  }
}

/*
  expand and correct longest common prefix array
*/
void expand_LCP_array(vector<int>* LCP_ext, vector<int>* LCP, vector<int> *A_ext, int* S) {
  int val = 0;
  for (int i = 0; i < LCP->size(); i++) {
    val = 2 * (*LCP)[i] + (S[((*A_ext)[i] + 2 * (*LCP)[i]) - 1] == S[((*A_ext)[i + 1] + 2 * (*LCP)[i]) - 1] ? 1 : 0);
    LCP_ext->push_back(val);
  }
}

/*
  construct NEW full odd trie using LCP and sorted array
*/

void constructFullOddTrie(Node* root, vector<int>& LCP, vector<int>& A, int* S) {
  Node* curr_node = nullptr;
  int curr_depth = 0;
  int LHS = 0;
  int END = A[A.size() - 1];

  stack<Node*> node_stack;
  node_stack.push(root);
  curr_node = node_stack.top();

  while (LHS < LCP.size()) {
    while (curr_depth < LCP[LHS]) {
      //create node as leaf to avoid edge_table leaks
      Edge* e = createTrieEdge(createTrieNode(curr_depth + 1, true),
                              S + (A[LHS] - 1 + curr_depth),
                              1);
      curr_node->children.push_back(e);
      node_stack.push(e->target_node);
      curr_node = node_stack.top();
      curr_depth++;
    }
    if (curr_depth == LCP[LHS]) {
      Edge* e = createTrieEdge(createTrieNode(curr_depth + 1, true, A[LHS]),
                              S + (A[LHS] - 1 + curr_depth),
                              END - (A[LHS] - 1 + curr_depth));
      curr_node->children.push_back(e);
      LHS++;
    }
    if (LHS == LCP.size()) {
      break;
    }
    if (curr_depth > LCP[LHS]) {
      Edge* e = createTrieEdge(createTrieNode(curr_depth + 1, true, A[LHS]),
                              S + (A[LHS] - 1 + curr_depth),
                              END - (A[LHS] - 1 + curr_depth));
      curr_node->children.push_back(e);
    }
    while (curr_depth > LCP[LHS]) {
      node_stack.pop();
      curr_node = node_stack.top();

      curr_depth--;
      if (curr_depth == LCP[LHS]) {
        LHS++;
        break;
      }
    }
  }
  Edge* e = createTrieEdge(createTrieNode(curr_depth + 1, true, A[LHS]),
                          S + (A[LHS] - 1 + curr_depth),
                          END - (A[LHS] - 1 + curr_depth));
  curr_node->children.push_back(e);
}


void eulerTour(Node* root, int suffix_count) {
  vector<Node*> ET;
  vector<int> L;
  vector<int> R(suffix_count, -1);

  Node* curr = nullptr;
  stack<Node*> node_s;
  node_s.push(root);

  while(!node_s.empty()) {
    curr = node_s.top();

    ET.push_back(curr);
    L.push_back(curr->Lv);

    if (curr->_next_edge < curr->children.size()) {
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
    } else {
      if (curr->suffix_index)
        R[curr->suffix_index] = ET.size() - 1;

      curr->_next_edge = 0;
      node_s.pop();
    }
  }

  cout << "number of visited nodes: " << ET.size() << endl;

  for(auto a : L) {
    cout << "level: " << a << endl;
  }
}
