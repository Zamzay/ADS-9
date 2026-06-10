// Copyright 2022 NNTU-CS
#ifndef INCLUDE_TREE_H_
#define INCLUDE_TREE_H_
#include <vector>
#include <cstdint>

struct Node {
  char val;
  std::vector<Node*> kids;
  int64_t perm_count;
  explicit Node(char v) : val(v), perm_count(0) {}
  ~Node();
};

class PMTree {
 private:
  Node* head;
  int len;
  Node* build(char v, const std::vector<char>& rest);
 public:
  explicit PMTree(const std::vector<char>& src);
  ~PMTree();
  Node* getRoot() const { return head; }
  int getN() const { return len; }
  int64_t getTotalPerms() const;
};

std::vector<std::vector<char>> collectAll(const PMTree& obj);
std::vector<char> getByOrder(const PMTree& obj, int idx);
std::vector<char> getByRank(const PMTree& obj, int idx);
