#include <bits/stdc++.h>
using namespace std;

#define ALPHABET_SIZE 256

struct Edge;

struct Node {
  int Lv;
  Edge** edge_table = nullptr;
  vector <Edge*> children;
  int suffix_index = 0;
  int _color;
  int _next_edge;
};

struct Edge {
  int* label;
  int label_len;
  Node* target_node;
};

Edge* createTrieEdge(Node* target, int* edge_label) {
  Edge* E = new Edge;
  E->label = edge_label;
  //cout << "new Edge: label " << *edge_label << endl;
  E->label_len = 1; //edge must have a label
  E->target_node = target;
  return E;
}

Node* createTrieNode(int level, int suffix_index, bool IS_LEAF_NODE = false) {

  Node* N = new Node;
  //cout << N << endl;
  N->Lv = level;
  // cout << "node" << N << " [label=" << '\"' << N->Lv;
  if (!IS_LEAF_NODE) {
    N->edge_table = new Edge*[ALPHABET_SIZE]();
    // cout << "\"];" << endl;
    //cout << "new Node: level " << level << endl;
  } else {
    //cout << "new Leaf: level " << level << endl;
    // cout << "$\"];" << endl;
    N->suffix_index = suffix_index;
  }
  N->_color = 0;
  N->_next_edge = 0;
  return N;
}

Edge* createFullTrieEdge(Node* target, int* edge_label = nullptr) {
  Edge* E = new Edge;
  E->label = edge_label;
  //cout << "new Edge: label " << *edge_label << endl;
  E->label_len = 1; //edge must have a label
  E->target_node = target;
  return E;
}

Node* createFullTrieNode(int level, int suffix_index, bool IS_LEAF_NODE = false) {
  Node* N = new Node;
  //cout << N << endl;
  N->Lv = level;
  // cout << "node" << N << " [label=" << '\"' << N->Lv;
  if (IS_LEAF_NODE) {
    N->suffix_index = suffix_index;
  }
  N->_color = 0;
  N->_next_edge = 0;
  return N;
}
void insertSuffix(Node* root, int* suffix_start, int suffix_index = 0) {

  Node* curr_node = root;
  int* curr_char = suffix_start;
  int lv = 0;

  while (*curr_char != INT_MAX) {
    lv++;

    if (!curr_node->edge_table[*curr_char]) {
      curr_node->edge_table[*curr_char] = createTrieEdge(createTrieNode(lv, 0 ,false), curr_char);
    }

    curr_node = curr_node->edge_table[*curr_char]->target_node;
    curr_char++;
  }

  if (*curr_char == INT_MAX) {
    if (!curr_node->edge_table[ALPHABET_SIZE - 1]) {
      curr_node->edge_table[ALPHABET_SIZE - 1] = createTrieEdge(createTrieNode(lv + 1, suffix_index ,true), curr_char);
    }
  }
}

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
    // sort(curr->children.begin(), curr->children.end(), compareEdge);
    delete[] curr->edge_table;
  }
}

void visitLeafNodes(Node* root) {
  stack<Node*> node_s;
  node_s.push(root);
  Node* curr = nullptr;
  while (!node_s.empty()) {
    curr = node_s.top();
    if (curr->_next_edge < curr->children.size()) {
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
      continue;
    } else if (curr->suffix_index != 0){
      cout << "arrived at leaf: " << curr->suffix_index << endl;
    } else {
      cout << "visited all nodes" << endl;
      curr->_next_edge = 0;
    }
    node_s.pop();
  }
}

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

void contractTrie(Node* root) {
  stack<Edge*> edge_s;
  Edge* base = createTrieEdge(root,nullptr);
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
      //curr = edge_s
    } else {
      curr->_next_edge = 0;
      edge_s.pop();
    }
  }
  delete base;
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
        last_peak = curr->Lv;
        peak_set = true;
      }
      node_s.push(curr->children[curr->_next_edge]->target_node);
      curr->_next_edge++;
      continue;
    } else if (curr->suffix_index != 0){
      sort_array->push_back(curr->suffix_index);
      LCP_array->push_back(last_peak);
      peak_set = false;
      cout << "arrived at leaf: " << curr->suffix_index << endl;
    } else {
      cout << "visited all nodes" << endl;
      curr->_next_edge = 0;
    }
    node_s.pop();
  }
}

/*
void construct_collect_delete_test()

void construct_collect_contract_delete_test()

void construct_collect_contract_arrays_delete_test()
*/

void construct_full_odd_tree(Node* root, vector<int>* LCP_ext, vector<int> *A_ext) {
  stack<Node*> stk;
  int curr_depth = 0;
  Node* curr = root;
  int LHS = 0;
  int RHS = 0;
  stk.push(root);
  RHS = 1;
  LHS = RHS - 1;
  do {
    while (curr_depth < (*LCP_ext)[LHS]) {
      Edge* e = createFullTrieEdge(createFullTrieNode(curr_depth,0));
      curr->children.push_back(e);
      stk.push(e->target_node);
      curr = stk.top();
      curr_depth++;
      if (curr_depth == (*LCP_ext)[LHS]) {
        Edge* e = createFullTrieEdge(createFullTrieNode(curr_depth,(*A_ext)[LHS],true));
        curr->children.push_back(e);
        LHS++;
      }
    }
    if (curr_depth > (*LCP_ext)[LHS]) {
      Edge* e = createFullTrieEdge(createFullTrieNode(curr_depth,(*A_ext)[LHS],true));
      curr->children.push_back(e);
    }
    while (curr_depth > (*LCP_ext)[LHS]) {
      curr = stk.top();
      stk.pop();
      curr_depth--;
      if (curr_depth == (*LCP_ext)[LHS]) {
        LHS++;
        break;
      }
    }
    if (LHS == LCP_ext->size()) {
      break;
    } else if (curr_depth == (*LCP_ext)[LHS]) {
      Edge* e = createFullTrieEdge(createFullTrieNode(curr_depth,(*A_ext)[LHS],true));
      curr->children.push_back(e);
      LHS++;
    }
  } while(true);
  root->children.push_back(createFullTrieEdge(createFullTrieNode(0,(*A_ext)[LHS],true)));

}
// expand sorted suffix array
void expand_sorted_suffix_array(vector<int>* A_ext, vector<int>* A) {
  for (int i = 0; i < A->size(); i++) {
    A_ext->push_back((2 * (*A)[i]) - 1);
  }
}
// expand and correct longest common prefix array
void expand_LCP_array(vector<int>* LCP_ext, vector<int>* LCP, vector<int> *A_ext, vector<int>* S) {
  int val = 0;
  //tern = ((*S)[(*A_ext)[i] + 2 * (*LCP)[i]] == (*S)[(*A_ext)[i + 1] + 2 * (*LCP)[i]] ? 1 : 0);
  for (int i = 0; i < LCP->size(); i++) {
    val = 2 * (*LCP)[i] + ((*S)[((*A_ext)[i] + 2 * (*LCP)[i]) - 1] == (*S)[((*A_ext)[i + 1] + 2 * (*LCP)[i]) - 1] ? 1 : 0);
    LCP_ext->push_back(val);
  }
}
void workflow() {
    //vector<int> a = {1};
    vector<int> s = {1,2,1,1,1,2,2,1,2,2,2,1,INT_MAX};
    int S[] = {1,2,1,1,1,2,2,1,2,2,2,1,INT_MAX,INT_MAX};
    vector<int> a = {2,1,3,4,6,5,7};
    vector<int> a_ext;
    vector<int> LCP = {0, 1, 0, 1, 0, 0};
    vector<int> LCP_ext;
    expand_sorted_suffix_array(&a_ext, &a);
    expand_LCP_array(&LCP_ext, &LCP, &a_ext, &s);
    int k = 0;
    Node* root = createFullTrieNode(k, 0);
    construct_full_odd_tree(root, &LCP_ext, &a_ext);//, S);
    visitLeafNodes(root);
    //displayTrie(root);
    deleteCollectedTrie(root);
    // return 0;
}

int main() {
  workflow();
  return 0;
  int sample[] = {3,1,3,1,2,INT_MAX};
  int sample2[] = {2, 1, 2, 3, 4, 3, INT_MAX};
  // int sample_size = 6;
  int sample_size = 7;
  //char sample[] = {'2', '1', '2', '3', '4', '3', '$', '\0'};
  int k = 0;
  Node* root = createTrieNode(k, 0);
  for (int i = 0; i < sample_size; i++) {
    insertSuffix(root, sample2 + i, i + 1);
  }
  collectNodes(root);
  cout << endl;
  //displayTrie(root);
  //visitLeafNodes(root);
  contractTrie(root);
  //displayTrie(root);
  //visitLeafNodes(root);
  vector<int> A;
  vector<int> LCP;
  buildArrays(root, &A, &LCP);
  for (auto a: LCP) {
    cout << a << " ";
  }
  cout << endl;
  deleteCollectedTrie(root);
  //delete root;
  // insertSuffix(root, sample + 1);
  // insertSuffix(root, sample + 2);
  // insertSuffix(root, sample + 5);
  //contract(root);
  // for (int i = 0; i < root->children.size(); i++) {
  //   cout << root->children[i]->label_len << '\t' << root->children[i]->target->L << endl;
  // }
  return 0;
}
