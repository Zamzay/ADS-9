#include <iostream>
#include <fstream>
#include <locale>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "tree.h"

Node::~Node() {
  for (Node* kid : children) delete kid;
}

int64_t PMTree::getTotalPerms() const {
  return root ? root->subtree_perm_count : 0;
}

Node* PMTree::createNode(char val, const std::vector<char>& leftovers) {
  Node* fresh = new Node(val);
  if (leftovers.empty()) {
    fresh->subtree_perm_count = 1;
    return fresh;
  }
  for (char sym : leftovers) {
    std::vector<char> rest;
    rest.reserve(leftovers.size() - 1);
    for (char x : leftovers) {
      if (x != sym) rest.push_back(x);
    }
    Node* branch = createNode(sym, rest);
    fresh->children.push_back(branch);
  }
  fresh->subtree_perm_count = fresh->children.size() *
                               fresh->children[0]->subtree_perm_count;
  return fresh;
}

PMTree::PMTree(const std::vector<char>& src) : root(nullptr), n(src.size()) {
  if (n == 0) {
    root = new Node('\0');
    root->subtree_perm_count = 0;
    return;
  }
  std::vector<char> ordered = src;
  std::sort(ordered.begin(), ordered.end());
  root = new Node('\0');
  for (size_t pos = 0; pos < ordered.size(); ++pos) {
    char cur_char = ordered[pos];
    std::vector<char> remainder;
    remainder.reserve(ordered.size() - 1);
    for (size_t idx = 0; idx < ordered.size(); ++idx) {
      if (idx != pos) remainder.push_back(ordered[idx]);
    }
    Node* branch = createNode(cur_char, remainder);
    root->children.push_back(branch);
  }
  if (!root->children.empty()) {
    root->subtree_perm_count = root->children.size() *
                               root->children[0]->subtree_perm_count;
  } else {
    root->subtree_perm_count = 0;
  }
}

PMTree::~PMTree() {
  delete root;
}

void traverse(Node* ptr, std::vector<char>& buffer,
              std::vector<std::vector<char>>& storage, bool skip_first) {
  if (!skip_first) buffer.push_back(ptr->value);
  if (ptr->children.empty()) {
    storage.push_back(buffer);
  } else {
    for (Node* branch : ptr->children) {
      traverse(branch, buffer, storage, false);
    }
  }
  if (!skip_first) buffer.pop_back();
}

std::vector<std::vector<char>> getAllPerms(const PMTree& obj) {
  std::vector<std::vector<char>> storage;
  Node* top = obj.getRoot();
  if (!top) return storage;
  std::vector<char> buffer;
  traverse(top, buffer, storage, true);
  return storage;
}

bool finder(Node* ptr, std::vector<char>& buffer, int& cnt,
            int need, bool skip_first, std::vector<char>& out) {
  if (!skip_first) buffer.push_back(ptr->value);
  if (ptr->children.empty()) {
    ++cnt;
    if (cnt == need) {
      out = buffer;
      if (!skip_first) buffer.pop_back();
      return true;
    }
  } else {
    for (Node* branch : ptr->children) {
      if (finder(branch, buffer, cnt, need, false, out)) {
        if (!skip_first) buffer.pop_back();
        return true;
      }
    }
  }
  if (!skip_first) buffer.pop_back();
  return false;
}

std::vector<char> getPerm1(const PMTree& obj, int k) {
  if (k < 1) return {};
  Node* top = obj.getRoot();
  if (!top || top->subtree_perm_count < k) return {};
  std::vector<char> buffer, out;
  int cnt = 0;
  finder(top, buffer, cnt, k, true, out);
  return out;
}

std::vector<char> getPerm2(const PMTree& obj, int k) {
  if (k < 1) return {};
  Node* top = obj.getRoot();
  if (!top || top->subtree_perm_count < k) return {};
  std::vector<char> out;
  Node* cur = top;
  int64_t remaining = k;
  while (!cur->children.empty()) {
    int64_t block = cur->children[0]->subtree_perm_count;
    int64_t slot = (remaining - 1) / block;
    Node* nxt = cur->children[slot];
    out.push_back(nxt->value);
    remaining = (remaining - 1) % block + 1;
    cur = nxt;
  }
  return out;
}
