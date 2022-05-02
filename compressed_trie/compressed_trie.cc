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

int main() {
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
  displayTrie(root);
  visitLeafNodes(root);
  contractTrie(root);
  displayTrie(root);
  visitLeafNodes(root);
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
