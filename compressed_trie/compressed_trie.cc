
#define ALPHABET_SIZE 256

struct Edge;

struct Node {
  int Lv;
  Edge** edge_table = nullptr;
  vector <Edge*> children;
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
  cout << "new Edge: label " << *edge_label << endl;
  E->label_len = 1; //edge must have a label
  E->target_node = target;
  return E;
}

Node* createTrieNode(int level, bool IS_LEAF_NODE = false) {

  Node* N = new Node;
  N->Lv = level;
  if (!IS_LEAF_NODE) {
    N->edge_table = new Edge*[ALPHABET_SIZE]();
    cout << "new Node: level " << level << endl;
  } else {
    cout << "new Leaf: level " << level << endl;
  }
  N->_color = 0;
  N->_next_edge = 0;
  return N;
}


void insertSuffix(Node* root, int* suffix_start) {

  Node* curr_node = root;
  int* curr_char = suffix_start;
  int lv = 0;

  while (*curr_char != INT_MAX) {
    lv++;

    if (!curr_node->edge_table[*curr_char]) {
      curr_node->edge_table[*curr_char] = createTrieEdge(createTrieNode(lv, false), curr_char);
    }

    curr_node = curr_node->edge_table[*curr_char]->target_node;
    curr_char++;
  }

  if (*curr_char == INT_MAX) {
    if (!curr_node->edge_table[ALPHABET_SIZE - 1]) {
      curr_node->edge_table[ALPHABET_SIZE - 1] = createTrieEdge(createTrieNode(lv + 1, true), curr_char);
    }
  }
}


void contract(Node* root) {
  stack<Edge*> edge_stack;
  edge_stack.push(createEdge(nullptr));
  Node* curr = root; //assumption is that root has at least 2 children

  while (curr != nullptr) {
    if (curr->children.size() == 1) {
      edge_stack.top()->label_len = edge_stack.top()->label_len + curr->children[0]->label_len;
      edge_stack.top()->target = curr->children[0]->target;
      delete curr->children[0];
      delete curr;
      curr = edge_stack.top()->target;
    } else if (curr->next_edge < curr->children.size()) {
      edge_stack.push(curr->children[curr->next_edge]);
      curr->next_edge++;
      curr = edge_stack.top()->target;
    } else { //all children visited
      edge_stack.pop();
      curr = edge_stack.top()->target;
    }
  }
}

int main() {
  int sample[] = {3,1,3,1,2,INT_MAX};
  //char sample[] = {'2', '1', '2', '3', '4', '3', '$', '\0'};
  int k = 0;
  Node* root = createTrieNode(k);
  insertSuffix(root, sample);
  insertSuffix(root, sample + 1);
  insertSuffix(root, sample + 2);
  insertSuffix(root, sample + 5);
  //contract(root);
  // for (int i = 0; i < root->children.size(); i++) {
  //   cout << root->children[i]->label_len << '\t' << root->children[i]->target->L << endl;
  // }
  return 0;
}
