#include "linked_list.h"

void addItem(ItemNode *&items, int row, int col)
{
    if (items == nullptr)
        items = new ItemNode(row, col);
    else
    {
        ItemNode *newNode = new ItemNode(row, col);
        newNode->next = items;
        items = newNode;
    }
}

bool itemExists(ItemNode *items, int row, int col)
{
    ItemNode *current = items;
    while (current != nullptr)
    {
        if (current->row == row && current->col == col)
            return true;
        current = current->next;
    }
    return false;
}

void removeItem(ItemNode *&items, int row, int col)
{
    if (items == nullptr)
        return;
    if (items->row == row && items->col == col)
    {
        ItemNode *temp = items;
        items = items->next;
        delete temp;
        return;
    }
    ItemNode *prev = items;
    ItemNode *current = items->next;
    while (current != nullptr)
    {
        if (current->row == row && current->col == col)
        {
            prev->next = current->next;
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}
