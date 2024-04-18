#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct ItemNode
{
    int row;
    int col;
    ItemNode *next;

    ItemNode(int r, int c) : row(r), col(c), next(nullptr) {}
};

void addItem(ItemNode *&items, int row, int col);
bool itemExists(ItemNode *items, int row, int col);
void removeItem(ItemNode *&items, int row, int col);

#endif
