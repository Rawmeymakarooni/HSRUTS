# Honkai Star Rail Gacha Simulator (HSRUTS)

## Deskripsi
Simulator Gacha dan Inventory berbasis C++ dengan fitur lengkap:
- Shop AVL Tree (Character & Lightcone)
- Inventory & Gacha
- Battle, History, dan Team Management
- Educational Data Structures (AVL, BST, Graph, BFS, DFS, Hash Table, dsb)

## Fitur Utama & Struktur Data

| Fitur/Data Structure           | Implementasi (Lokasi Sekunder)                      |
|-------------------------------|-----------------------------------------------------|
| **BST = Search Everything**    | `CharacterTree` (library, team, search karakter)    |
| **AVL Tree**                  | `AVLShopTree` (shop character/lightcone, AVL view)  |
| **Graph**                     | Battle performance graph (ASCII/bar chart)          |
| **BFS**                       | Riwayat serangan battle urut dari awal (history)    |
| **DFS**                       | Riwayat battle dari akhir/last hit (history)        |
| **Hashing/Hash Table**        | `unordered_map` (inventory, ownedCharacters, dsb)   |
| **Sorting (Merge Sort)**      | Pengurutan karakter/lightcone di tree/shop          |
| **Exception Handling**        | try-catch pada save/load file                       |
| **STL**                       | vector, unordered_map, queue, stack, set, dll       |
| **Iterator**                  | Loop STL & tree traversal                           |
| **Sort/Find/Count**           | Pengurutan, pencarian, dan counting di team/history  |
| **File Handling**             | Save/load game (`ifstream`, `ofstream`)             |
| **Lambda Expression**         | count_if (misal: jumlah eidolon di recent history)  |

## Kompilasi
```
g++ Honkai-Star-Rail.cpp -o HSRUTS.exe -std=c++14 -lwinmm
```

## Cara Main
1. Jalankan `HSRUTS.exe`
2. Ikuti menu utama:
   - Gacha, Shop, Library, Team, Battle, History, dsb
3. Shop: Pilih Character/Lightcone, toggle view (AVL, stats, dsb)
4. Battle: Lihat history urut (BFS/DFS), cek performa (graph)

## Catatan
- Project ini menekankan edukasi struktur data di aplikasi nyata
- Fitur AVL, BST, Graph, Hash Table, dsb dapat dilihat langsung di menu terkait
- Kode sudah menggunakan best practice STL, exception, dan file handling

---

Jika ada pertanyaan atau ingin kontribusi, silakan kontak pembuat.
