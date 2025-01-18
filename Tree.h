#pragma once
using namespace std;
#include<iostream>
#include<string.h>


// A class to create a node 
struct DATA
{
    string data;		//Foxx
    string path;		//file1
    int line_no;	//line no
    string key;		//foxx hash
    string MachineIdinDATA;		//machineidine no.

    DATA()
    {
        data = "";
        path = "";
        line_no = 0;
        key = "";
        MachineIdinDATA= "";
    }

    ~DATA() {}
    //DATA& operator=(const DATA& rhs);
};


const int MAX_KEYS = 160; // Define maximum keys in a B-tree node

//template<typename Type, typename Type1>
struct BTreeNode{
public:
    string* keys;          // Array to hold keys
    DATA* objects;         // Array to hold associated data
    BTreeNode** children;  // Array to hold child pointers
    int numKeys;           // Number of keys currently in node
    bool isLeaf;           // Flag to indicate if the node is a leaf

    BTreeNode() {
        keys = new string[MAX_KEYS];
        objects = new DATA[MAX_KEYS];
        children = new BTreeNode * [MAX_KEYS + 1];
        numKeys = 0;
        isLeaf = true;
        for (int i = 0; i < MAX_KEYS + 1; ++i)
            children[i] = nullptr;
    }

    ~BTreeNode() {
        delete[] keys;
        delete[] objects;
        delete[] children;
    }

    void displayNode(BTreeNode* node, int level = 0) {
        if (node != nullptr) {
            int i;
            for (i = 0; i < node->numKeys; ++i) {
                displayNode(node->children[i], level + 1);
                
                cout << "NData: " << node->objects[i].data
                     << " | Key: " << node->keys[i]
                    << " | Path: " << node->objects[i].path
                    << " | Line No: " << node->objects[i].line_no
                    << endl;
            }
            displayNode(node->children[i], level + 1);
        }
    }
};

class BTree {

public:

    BTreeNode* root; // Pointer to the root node
    int t;           // Minimum degree

    BTree(int degree) : root(nullptr), t(degree) {}
    BTree()
    {
        root = NULL;
        t = 5;
    }

    // Function to traverse the B-tree
    void traverse() {
        if (root != nullptr)
            traverse(root);
    }
    // Function to traverse the B-tree (internal)
    void traverse(BTreeNode* node) {
        if (node != nullptr) {
            int i;
            for (i = 0; i < node->numKeys; ++i) {
                if (!node->isLeaf)
                    traverse(node->children[i]);
                cout << node->keys[i] << " "; // Print key
            }
            if (!node->isLeaf)
                traverse(node->children[i]);
        }
    }

    // Other B-tree operations like insert, search, delete, etc., can be implemented similarly

    BTreeNode* splitChild(BTreeNode* parentNode, int index, BTreeNode* child) {
        BTreeNode* newChild = new BTreeNode();
        newChild->isLeaf = child->isLeaf;
        newChild->numKeys = t - 1;

        for (int i = 0; i < t - 1; ++i) {
            newChild->keys[i] = child->keys[i + t];
            newChild->objects[i] = child->objects[i + t];
        }

        if (!child->isLeaf) {
            for (int i = 0; i < t; ++i)
                newChild->children[i] = child->children[i + t];
        }

        child->numKeys = t - 1;

        for (int i = parentNode->numKeys; i >= index + 1; --i)
            parentNode->children[i + 1] = parentNode->children[i];

        parentNode->children[index + 1] = newChild;

        for (int i = parentNode->numKeys - 1; i >= index; --i) {
            parentNode->keys[i + 1] = parentNode->keys[i];
            parentNode->objects[i + 1] = parentNode->objects[i];
        }

        parentNode->keys[index] = child->keys[t - 1];
        parentNode->objects[index] = child->objects[t - 1];

        parentNode->numKeys++;

        return parentNode;
    }

    //helper function for insertion 
    BTreeNode* insertNonFull(BTreeNode* node, string key, DATA obj) {
        int i = node->numKeys - 1;

        if (node->isLeaf) {
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->objects[i + 1] = node->objects[i];
                --i;
            }

            node->keys[i + 1] = key;
            node->objects[i + 1] = obj;
            node->numKeys++;
        }
        else {
            while (i >= 0 && key < node->keys[i])
                --i;

            ++i;

            if (node->children[i]->numKeys == (2 * t) - 1) {
                node = splitChild(node, i, node->children[i]);

                if (key > node->keys[i])
                    ++i;
            }

            node->children[i] = insertNonFull(node->children[i], key, obj);
        }

        return node;
    }

    void insert_tree(BTree& tree, string key, DATA obj) {
        if (tree.root == nullptr) {
            tree.root = new BTreeNode();
            tree.root->keys[0] = key;
            tree.root->objects[0] = obj;
            tree.root->numKeys = 1;
        }
        else {
            if (tree.root->numKeys == (2 * t) - 1) {
                BTreeNode* newRoot = new BTreeNode();
                newRoot->isLeaf = false;
                newRoot->children[0] = tree.root;
                newRoot = splitChild(newRoot, 0, tree.root);

                int i = 0;
                if (newRoot->keys[0] < key)
                    ++i;

                newRoot->children[i] = insertNonFull(newRoot->children[i], key, obj);

                tree.root = newRoot;
            }
            else {
                tree.root = insertNonFull(tree.root, key, obj);
            }
        }
        // Display the tree after insertion
        /*cout << "Tree after insertion:\n";
        display(tree.root);*/
    }


    BTreeNode* searchBTree2(BTreeNode* node, string key) {

        if (node == nullptr)
            return nullptr;

        int i = 0;
        while (i < node->numKeys && key > node->objects[i].key)
            ++i;

        if (i < node->numKeys && key == node->objects[i].key)
            return node;

        if (node->isLeaf)
            return nullptr;

        return searchBTree2(node->children[i], key);
    }

    template<class T>
    BTreeNode* searchBTree(BTree& tree, string key) {
        return searchBTree2(tree.root, key);
    }


    void removeFromLeaf(BTreeNode* node, int index) {
        for (int i = index + 1; i < node->numKeys; ++i) {
            node->keys[i - 1] = node->keys[i];
            node->objects[i - 1] = node->objects[i];
        }
        node->numKeys--;
    }

    void removeFromNonLeaf(BTreeNode* node, int index) {
        string key = node->keys[index];

        if (node->children[index]->numKeys >= t) {
            // Find predecessor (rightmost) child of index
            BTreeNode* pred = node->children[index];
            while (!pred->isLeaf)
                pred = pred->children[pred->numKeys];

            string predKey = pred->keys[pred->numKeys - 1];
            DATA predObj = pred->objects[pred->numKeys - 1];

            node->keys[index] = predKey;
            node->objects[index] = predObj;
            removeFromBTree(pred, pred->numKeys - 1);
        }
        else if (node->children[index + 1]->numKeys >= t) {
            // Find successor (leftmost) child of index+1
            BTreeNode* succ = node->children[index + 1];
            while (!succ->isLeaf)
                succ = succ->children[0];

            string succKey = succ->keys[0];
            DATA succObj = succ->objects[0];

            node->keys[index] = succKey;
            node->objects[index] = succObj;
            removeFromBTree(succ, 0);
        }
        else {
            // Merge the two children
            merge(node, index);
            removeFromBTree(node->children[index], t - 1);
        }
    }

    void removeFromBTree(BTreeNode* node, int index) {
        if (node->isLeaf)
            removeFromLeaf(node, index);
        else
            removeFromNonLeaf(node, index);
    }

    void deleteBTree(BTree& tree, int index) {
        if (tree.root == nullptr)
            return;

        removeFromBTree(tree.root, index);

        if (tree.root->numKeys == 0) {
            BTreeNode* tmp = tree.root;
            if (tree.root->isLeaf)
                tree.root = nullptr;
            else
                tree.root = tree.root->children[0];
            delete tmp;
        }
    }

    void merge(BTreeNode* node, int index) {
        BTreeNode* child = node->children[index];
        BTreeNode* sibling = node->children[index + 1];

        child->keys[t - 1] = node->keys[index];
        child->objects[t - 1] = node->objects[index];

        for (int i = 0; i < sibling->numKeys; ++i) {
            child->keys[i + t] = sibling->keys[i];
            child->objects[i + t] = sibling->objects[i];
        }

        if (!child->isLeaf) {
            for (int i = 0; i <= sibling->numKeys; ++i)
                child->children[i + t] = sibling->children[i];
        }

        for (int i = index + 1; i < node->numKeys; ++i) {
            node->keys[i - 1] = node->keys[i];
            node->objects[i - 1] = node->objects[i];
        }

        for (int i = index + 2; i <= node->numKeys; ++i)
            node->children[i - 1] = node->children[i];

        child->numKeys += sibling->numKeys + 1;
        node->numKeys--;

        delete sibling;
    }

    int findKey(BTreeNode* node, const string& key) const {
        int idx = 0;
        while (idx < node->numKeys && node->keys[idx] < key)
            ++idx;
        return idx;
    }

    void Delete(BTreeNode* node, const string& key)
    {
        int index = searchBTree3(node, key);
        node = searchBTree2(node, key);
        //int index = findKey(node, key);
        if (index == -1)
        {
            cout << "Not Found" << endl;
            return;
        }

        removeFromBTree(node, index);
        // removeFromBTree(node, findKey(node, key));
    }

    int searchBTree3(BTreeNode* node, string key) {

        if (node == nullptr)
            return -1;

        int i = 0;
        while (i < node->numKeys && key > node->objects[i].key)
            ++i;

        if (i < node->numKeys && key == node->objects[i].key)
            return i;

        if (node->isLeaf)
            return -1;

        return searchBTree3(node->children[i], key);
    }

    void display(BTreeNode* node,/*MachineIDtoB*/ int level = 0) {
        
        if (node != nullptr) {
            int i;
            for (i = 0; i < node->numKeys; ++i) {
                display(node->children[i], level + 1);

                cout << "Data: " << node->objects[i].data
                    << " | Key: " << node->keys[i]
                    << " | Path: " << node->objects[i].path
                    << " | Line No: " << node->objects[i].line_no
                    << endl;

            }
        }
        
    }

    BTreeNode* retrieve(string d) {
        BTreeNode* temp = root;
        int i = 0;

        while (temp) {

            //while (i < temp->numKeys && d > temp->objects[i].key)
            //    ++i;

           // if (i < temp->numKeys && d == temp->objects[i].key)
            if (d == temp->objects[i].key)
                return temp;

/*            if (temp->isLeaf)
                break; */// Value not found in this node, exit loop

            temp = temp->children[i]; // Move to appropriate child node
            i++;
        }

        cout << "\nDoes Not Exist\n";
        return NULL;
    }


};


