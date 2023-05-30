#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <unistd.h>
#include <vector>

struct Node {
    Node *link;
    std::map<char, Node *> children;
    size_t start = 0;
    size_t *end;
    size_t suffixIndex;
};

class Tree {
  public:
    Tree(std::vector<char> &text) : text(text) { m_buildSuffixTree(); }
    ~Tree();
    std::vector<int> indices(std::vector<char> substring);
    std::vector<char> &text;

  private:
    Node *m_root;
    Node *m_activeNode;
    Node *m_lastCreatedNode;
    size_t m_activeEdge;
    size_t m_remainingSuffixes = 0;
    size_t m_size = -1;
    size_t m_rootEnd = -1;
    size_t m_leafEnd = 0;
    size_t m_activeLength = 0;
    size_t m_splitEnd;

    Node *m_newNode(size_t start, size_t *end);
    size_t m_edgeLength(Node *node);
    bool m_walkDown(Node *node);
    void m_extendSuffix(size_t pos);
    void m_buildSuffixTree();
    void m_addLeaf(size_t pos);
    bool m_checkForSuffixLink(size_t pos, Node *next);
    void m_splitNode(size_t pos, Node *next);
};
