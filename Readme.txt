g++# Honkai Star Rail C++ Project

## Kompilasi (Windows)
Jika .exe tidak bisa dimainkan, compile dengan:
```
g++ Honkai-Star-Rail.cpp -o "Game (mainkan .Exe)/Honkai-Star-Rail.exe" -lwinmm
```

---

## Struktur Data & Algoritma (untuk laporan)

### Referensi Lokasi di Kode (Honkai-Star-Rail.cpp)
- **BFS & DFS (Riwayat Battle)**
  - Deklarasi: `struct BattleAction` (sekitar line 76)
  - Implementasi: `class BattleHistory` (sekitar line 86)
- **AVL Tree (Shop)**
  - Deklarasi & implementasi: `class AVLShopTree` (sekitar line 765)
- **BST (Search Everything)**
  - Deklarasi & implementasi: `class CharacterTree` (sekitar line 1182)
- **Circular Linked List (Cycling Karakter)**
  - Deklarasi: `struct CircleNode` (sekitar line 856)

---

- **BST = Search Everything**
  - Binary Search Tree untuk pencarian karakter dan lightcone di library.

- **AVL Tree**
  - AVL Tree untuk fitur shop (daftar item shop otomatis seimbang dan urut nama).

- **Graph = Battle Performance**
  - Visualisasi performa battle (grafik performa serangan).

- **BFS = Riwayat Serangan Battle Urut**
  - Breadth-First Search (BFS) menampilkan riwayat serangan battle urut dari awal.

- **DFS = Riwayat Battle dari Last Hit/Akhir**
  - Depth-First Search (DFS) menampilkan riwayat battle dari aksi terakhir.

- **Hashing & Hash Table = unordered_map**
  - Penyimpanan cepat data (status karakter, lightcone, dsb).

- **Sorting = Merge Sort**
  - Pengurutan karakter di tree berdasarkan alfabet.

- **Exception Handling**
  - try-catch untuk penanganan error file, input, dsb.

- **STL (Standard Template Library)**
  - Penggunaan vector, map, unordered_map, queue, stack, dsb.

- **Iterator**
  - Iterator eksplisit pada pengelompokan karakter, AVL, dsb.

- **Sort, Find, Count**
  - Pengurutan karakter di manage team, pencarian dan perhitungan karakter di histori.

- **File Handling**
  - Save/load data game menggunakan file teks.

- **Lambda Expression**
  - Menghitung karakter Eidolon di recent character history (lambda).

---

Dokumentasi ini dapat digunakan untuk laporan praktikum/TA/UTS.