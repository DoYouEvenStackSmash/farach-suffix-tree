#include <bits/stdc++.h>

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

/*
  display function for printing Trie in dot language to stdout.
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

void constructFullOddTrie(Node* root, vector<int>& LCP, vector<int>& A, int* S, int max) {
  Node* curr_node = nullptr;
  int curr_depth = 0;
  int LHS = 0;
  int END = (max == 0 ? A[A.size() - 1] : max);

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


/*
  acts as a bucket sort, treating the elements of A_ext as indexes for input_string
  puts return values in rv
*/
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

/*
  Range Minimum Query function, sparse table. constant time lookup.
*/

int RMQ(SparseTable* lca, int x, int y) {
  int L = lca->rep[x];
  int R = lca->rep[y];
  if (R < L)
    swap(R, L);
  int j = lca->log_memo[R - L + 1];
  return min(lca->st[L][j], lca->st[R - (1 << j) + 1][j]);
}

/*
  reducing LCA to RMQ via a precomputed Sparse Table data structure
*/
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

/*
  Euler tour of a constructed tree. visits all nodes in depth first search.
  2n - 1 visits
  Saves level(distance from root node) to return vector
    size 2n - 1
  saves representative index of first visit of each leaf node in the tree.
    corresponds to index in level, size n

  In the more general case, rep would save the first visit of EVERY node in the tree
*/
void eulerTour(Node* root, vector<int> &level, vector<int> &rep) {

  Node* curr = nullptr;
  stack<Node*> node_s;
  node_s.push(root);

  while(!node_s.empty()) {
    curr = node_s.top();
    level.push_back(curr->Lv);

    if (curr->_next_edge < curr->children.size()) {
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
    } else {
      if (curr->suffix_index)
        rep[curr->suffix_index] = level.size() - 1;

      curr->_next_edge = 0;
      node_s.pop();
    }
  }
}

/*
  precomputes sparse table data structure using level array and representative array.

  In the more general case, this would also include a vector of visited nodes.
  nlogn
*/
void constructST(SparseTable* ST, vector<int> &L, vector<int> &R) {

  // length of euler tour, 2n - 1
  ST->max_n = L.size();

  // allocate memory for preprocessed log values
  // maximum interval size 2n - 1, maximum index 2n
  ST->log_memo = new int[ST->max_n + 1];
  ST->log_memo[1] = 0;
  for (int i = 2; i <= ST->max_n; i++)
    ST->log_memo[i] = ST->log_memo[i / 2] + 1;

  // k >= 2 ^ floor(binary_log(max_n))
  // set k to the largest log value
  int k = ST->log_memo[ST->max_n];

  // allocate memory for sparse table
  // sparse table is nlogn size, max_n * (k + 1)
  ST->st = new int*[ST->max_n];

  for (int i = 0; i < ST->max_n; i++) {
    ST->st[i] = new int[k + 1];
    // each element in the array is minimum of itself
    ST->st[i][0] = L[i];
  }

  /*
    populates sparse table with dp, column by column
    elements x are already populated from initial allocation.
    [x, 1, 4],
    [x, 2, 5],
    [x, 3, 6]
    (2 ^ j) is the current size of the window,
    covers two windows from previous column
  */
  for (int j = 1; j <= k; j++) {
    for (int i = 0; i + (1 << j) <= ST->max_n; i++) {
      ST->st[i][j] = min(ST->st[i][j - 1], ST->st[i + (1 << (j - 1))][j - 1]);
    }
  }

  ST->rep = new int[R.size()];
  // transfer euler tour representative array
  for (int i = 0; i < R.size(); i++) {
    ST->rep[i] = R[i];
  }
}

/*
  helper function for preprocessing a Tree
  builds a Sparse Table data structure.
*/
void preprocessLCA(S &s) {
  int last_suffix = s.A_ext[s.A_ext.size() - 1] + 1;
  vector<int> level;
  vector<int> representative(last_suffix, -1);

  eulerTour(s.root, level, representative);
  s.sp_tab = new SparseTable;
  constructST(s.sp_tab, level, representative);
}

/*
  deletes sparse table data structure.
*/
void deleteST(SparseTable* ST) {
  delete[] ST->log_memo;
  for (int i = 0; i < ST->max_n; i++){
    delete[] ST->st[i];
  }
  delete[] ST->st;
  delete[] ST->rep;
  delete ST;
}
