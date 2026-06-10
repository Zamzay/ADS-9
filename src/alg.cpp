#include <iostream>
#include <fstream>
#include <locale>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "tree.h"

Node::~Node() {
  for (Node* kid : kids) delete kid;
}

int64_t PMTree::getTotalPerms() const {
  return head ? head->perm_count : 0;
}

Node* PMTree::build(char v, const std::vector<char>& rest) {
  Node* fresh = new Node(v);
  if (rest.empty()) {
    fresh->perm_count = 1;
    return fresh;
  }
  for (char sym : rest) {
    std::vector<char> leftover;
    leftover.reserve(rest.size() - 1);
    for (char x : rest) {
      if (x != sym) leftover.push_back(x);
    }
    Node* branch = build(sym, leftover);
    fresh->kids.push_back(branch);
  }
  fresh->perm_count = fresh->kids.size() * fresh->kids[0]->perm_count;
  return fresh;
}

PMTree::PMTree(const std::vector<char>& src) : head(nullptr), len(src.size()) {
  if (len == 0) {
    head = new Node('\0');
    head->perm_count = 0;
    return;
  }
  std::vector<char> ordered = src;
  std::sort(ordered.begin(), ordered.end());
  head = new Node('\0');
  for (size_t pos = 0; pos < ordered.size(); ++pos) {
    char cur = ordered[pos];
    std::vector<char> remainder;
    remainder.reserve(ordered.size() - 1);
    for (size_t idx = 0; idx < ordered.size(); ++idx) {
      if (idx != pos) remainder.push_back(ordered[idx]);
    }
    Node* branch = build(cur, remainder);
    head->kids.push_back(branch);
  }
  if (!head->kids.empty()) {
    head->perm_count = head->kids.size() * head->kids[0]->perm_count;
  } else {
    head->perm_count = 0;
  }
}

PMTree::~PMTree() {
  delete head;
}

void walk(Node* ptr, std::vector<char>& buf, std::vector<std::vector<char>>& store, bool skip) {
  if (!skip) buf.push_back(ptr->val);
  if (ptr->kids.empty()) {
    store.push_back(buf);
  } else {
    for (Node* branch : ptr->kids) {
      walk(branch, buf, store, false);
    }
  }
  if (!skip) buf.pop_back();
}

std::vector<std::vector<char>> collectAll(const PMTree& obj) {
  std::vector<std::vector<char>> store;
  Node* top = obj.getRoot();
  if (!top) return store;
  std::vector<char> buf;
  walk(top, buf, store, true);
  return store;
}

bool seek(Node* ptr, std::vector<char>& buf, int& cnt, int need, bool skip, std::vector<char>& out) {
  if (!skip) buf.push_back(ptr->val);
  if (ptr->kids.empty()) {
    ++cnt;
    if (cnt == need) {
      out = buf;
      if (!skip) buf.pop_back();
      return true;
    }
  } else {
    for (Node* branch : ptr->kids) {
      if (seek(branch, buf, cnt, need, false, out)) {
        if (!skip) buf.pop_back();
        return true;
      }
    }
  }
  if (!skip) buf.pop_back();
  return false;
}

std::vector<char> getByOrder(const PMTree& obj, int idx) {
  if (idx < 1) return {};
  Node* top = obj.getRoot();
  if (!top || top->perm_count < idx) return {};
  std::vector<char> buf, out;
  int cnt = 0;
  seek(top, buf, cnt, idx, true, out);
  return out;
}

std::vector<char> getByRank(const PMTree& obj, int idx) {
  if (idx < 1) return {};
  Node* top = obj.getRoot();
  if (!top || top->perm_count < idx) return {};
  std::vector<char> out;
  Node* cur = top;
  int64_t rem = idx;
  while (!cur->kids.empty()) {
    int64_t block = cur->kids[0]->perm_count;
    int64_t slot = (rem - 1) / block;
    Node* nxt = cur->kids[slot];
    out.push_back(nxt->val);
    rem = (rem - 1) % block + 1;
    cur = nxt;
  }
  return out;
}
