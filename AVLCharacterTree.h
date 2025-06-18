#ifndef AVL_CHARACTER_TREE_H
#define AVL_CHARACTER_TREE_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

// Node untuk AVL Tree
struct AVLNode {
    string character;
    int height;
    AVLNode* left;
    AVLNode* right;
    
    AVLNode(const string& name) : character(name), height(1), left(nullptr), right(nullptr) {}
};

class AVLCharacterTree {
private:
    AVLNode* root;
    
    // Mendapatkan tinggi node
    int getHeight(AVLNode* node) {
        if (!node) return 0;
        return node->height;
    }
    
    // Menghitung balance factor suatu node
    int getBalanceFactor(AVLNode* node) {
        if (!node) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }
    
    // Update tinggi dari suatu node
    void updateHeight(AVLNode* node) {
        if (!node) return;
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    }
    
    // Rotasi ke kanan
    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        // Rotasi
        x->right = y;
        y->left = T2;
        
        // Update tinggi
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    // Rotasi ke kiri
    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        // Rotasi
        y->left = x;
        x->right = T2;
        
        // Update tinggi
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    // Menyisipkan karakter baru ke dalam tree
    AVLNode* insertNode(AVLNode* node, const string& name) {
        // Langkah 1: Lakukan penyisipan BST normal
        if (!node) return new AVLNode(name);
        
        if (name < node->character)
            node->left = insertNode(node->left, name);
        else if (name > node->character)
            node->right = insertNode(node->right, name);
        else
            return node; // Tidak mengizinkan duplikasi
            
        // Langkah 2: Update tinggi ancestor node
        updateHeight(node);
        
        // Langkah 3: Dapatkan balance factor untuk mengetahui apakah node menjadi tidak seimbang
        int balance = getBalanceFactor(node);
        
        // Langkah 4: Jika node menjadi tidak seimbang, ada 4 kasus
        
        // Left Left Case
        if (balance > 1 && name < node->left->character)
            return rightRotate(node);
            
        // Right Right Case
        if (balance < -1 && name > node->right->character)
            return leftRotate(node);
            
        // Left Right Case
        if (balance > 1 && name > node->left->character) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        // Right Left Case
        if (balance < -1 && name < node->right->character) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        
        // Node tetap seimbang
        return node;
    }
    
    // Traversal inorder untuk mendapatkan daftar semua karakter
    void inorder(AVLNode* node, vector<string>& result) {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node->character);
        inorder(node->right, result);
    }
    
    // Mengelompokkan karakter berdasarkan awalan huruf menggunakan iterator eksplisit
    void collectByAlphabet(AVLNode* node, map<char, vector<string>>& groups) {
        if (!node) return;
        collectByAlphabet(node->left, groups);
        char initial = toupper(node->character[0]);
        groups[initial].push_back(node->character);
        collectByAlphabet(node->right, groups);
    }
    
    // Mencari karakter dalam tree
    AVLNode* searchNode(AVLNode* node, const string& name) {
        if (!node || node->character == name) return node;
        if (name < node->character)
            return searchNode(node->left, name);
        return searchNode(node->right, name);
    }
    
    // Menghapus tree secara rekursif
    void cleanupTree(AVLNode* node) {
        if (!node) return;
        cleanupTree(node->left);
        cleanupTree(node->right);
        delete node;
    }
    
public:
    AVLCharacterTree() : root(nullptr) {}
    
    ~AVLCharacterTree() {
        cleanupTree(root);
    }
    
    // Menambahkan karakter ke tree
    void addCharacter(const string& name) {
        root = insertNode(root, name);
    }
    
    // Mencari apakah karakter ada dalam tree
    bool findCharacter(const string& name) {
        return searchNode(root, name) != nullptr;
    }
    
    // Mendapatkan daftar semua karakter dalam tree
    vector<string> getAllCharacters() {
        vector<string> result;
        inorder(root, result);
        return result;
    }
    
    // Menampilkan dengan iterasi explisit berdasarkan alfabet
    void viewGroupedByAlphabet() {
        cout << "\n=== AVL Tree Diurut Berdasarkan Awalan Huruf ===\n";
        map<char, vector<string>> groups;
        collectByAlphabet(root, groups);
        
        // Menggunakan iterator eksplisit untuk map
        for (map<char, vector<string>>::iterator it = groups.begin(); it != groups.end(); ++it) {
            char c = it->first;
            vector<string>& vec = it->second;
            
            cout << "[" << c << "]\n";
            // Menggunakan iterator eksplisit untuk vector
            for (vector<string>::iterator v_it = vec.begin(); v_it != vec.end(); ++v_it) {
                cout << " - " << *v_it << "\n";
            }
        }
    }
    
    // Menampilkan tinggi dan balance tree
    void displayTreeProperties() {
        if (!root) {
            cout << "AVL Tree kosong.\n";
            return;
        }
        
        cout << "\n=== AVL Tree Properties ===\n";
        cout << "Tinggi: " << getHeight(root) << "\n";
        cout << "Balance Factor: " << getBalanceFactor(root) << "\n";
        cout << "Jumlah Karakter: " << getAllCharacters().size() << "\n";
    }
};

#endif // AVL_CHARACTER_TREE_H
