#include "suffix_tree.h"

Tree::~Tree() {
    std::queue<Node *> nodes;
    nodes.push(m_root);

    while (!nodes.empty()) {
        Node *cur = nodes.front();
        for (auto node : cur->children) {
            if (node.second != nullptr) {
                nodes.push(node.second);
            }
        }
        nodes.pop();
        delete cur;
    }
}

Node *Tree::m_newNode(size_t start, size_t *end) {
    Node *node = new Node;

    node->start = start;
    node->end = end;
    node->link = m_root;
    node->suffixIndex = -1;

    return node;
}

size_t Tree::m_edgeLength(Node *node) {
    return (*node->end - node->start + 1) * static_cast<int>(node != m_root);
}

bool Tree::m_walkDown(Node *node) {
    size_t len = m_edgeLength(node);
    if (m_activeLength >= len) {
        m_activeEdge += len;
        m_activeLength -= len;
        m_activeNode = node;
        return true;
    }
    return false;
}

void Tree::m_addLeaf(size_t pos) {
    m_activeNode->children[text[m_activeEdge]] = m_newNode(pos, &m_leafEnd);

    if (m_lastCreatedNode != nullptr) {
        m_lastCreatedNode->link = m_activeNode;
        m_lastCreatedNode = nullptr;
    }
}

bool Tree::m_checkForSuffixLink(size_t pos, Node *next) {
    if (text.at(next->start + m_activeLength) == text.at(pos)) {
        if (m_lastCreatedNode != nullptr && m_activeNode != m_root) {
            m_lastCreatedNode->link = m_activeNode;
            m_lastCreatedNode = nullptr;
        }
        m_activeLength++;
        return true;
    }
    return false;
}

void Tree::m_splitNode(size_t pos, Node *next) {

    m_splitEnd = next->start + m_activeLength - 1;
    Node *split = m_newNode(next->start, &m_splitEnd);
    m_activeNode->children[text[m_activeEdge]] = split;
    split->children[text.at(pos)] = m_newNode(pos, &m_leafEnd);
    next->start += m_activeLength;
    split->children[text.at(next->start)] = next;
    if (m_lastCreatedNode != nullptr) {
        m_lastCreatedNode->link = split;
    }
    m_lastCreatedNode = split;
}

void Tree::m_extendSuffix(size_t pos) {
    m_leafEnd = pos;
    m_remainingSuffixes++;
    m_lastCreatedNode = nullptr;

    while (m_remainingSuffixes > 0) {
        if (m_activeLength == 0) {
            m_activeEdge = pos;
        }

        Node *next = nullptr;

        if (m_activeNode->children.find(text[m_activeEdge]) !=
            m_activeNode->children.end()) {
            next = m_activeNode->children[text[m_activeEdge]];
        }

        if (next == nullptr) {
            m_addLeaf(pos);
        } else {
            if (m_walkDown(next)) {
                continue;
            }

            if (m_checkForSuffixLink(pos, next)) {
                break;
            }

            m_splitNode(pos, next);
        }

        m_remainingSuffixes--;
        if (m_activeNode == m_root && m_activeLength > 0) {
            m_activeLength--;
            m_activeEdge = pos - m_remainingSuffixes + 1;
        } else if (m_activeNode != nullptr) {
            m_activeNode = m_activeNode->link;
        }
    }
}

void Tree::m_buildSuffixTree() {

    m_size = text.size();
    m_root = m_newNode(-1, &m_rootEnd);
    m_root->link = m_root;
    m_activeNode = m_root;
    for (size_t i = 0; i < m_size; i++) {
        m_extendSuffix(i);
    }
}

int main() {
    std::ifstream file("sample.cpp.bak");
    assert(file.is_open());
    std::vector<char> text;
    if (!file.eof() && !file.fail()) {
        file.seekg(0, std::ios_base::end);
        std::streampos fileSize = file.tellg();
        text.resize(fileSize);

        file.seekg(0, std::ios_base::beg);
        file.read(text.data(), fileSize);
    }
    Tree tree = Tree(text);
    return 0;
}
