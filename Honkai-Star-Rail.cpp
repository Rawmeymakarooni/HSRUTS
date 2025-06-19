#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <algorithm>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <regex>
#include <cmath>
#include <queue>
#include <set>
#include <limits>
#include "AVLCharacterTree.h"
#include <stdexcept> 
#pragma comment(lib, "winmm.lib")

using namespace std;

enum BannerType { RATE_UP, STANDARD, LIGHTCONE };
BannerType currentBanner = STANDARD;
enum CharacterType { DPS, BUFFER, DEBUFFER, HEALER, SHIELDER };

unordered_map<string, string> equippedLightcones; 
unordered_map<string, int> lightconeSuperimposition;

vector<string> fiveStarCharacters = {
    "Bronya", "Clara", "Gepard", "Himeko", "Seele", "Yanqing", "Bailu", "Welt", "Fu Xuan", "Sparkle", "Silver Wolf", "Black Swan", "Luocha"
};

vector<string> fourStarCharacters = {
    "Arlan", "Asta", "Dan Heng", "Hook", "March 7th", "Natasha", "Pela", "Sampo", "Serval", "Tingyun"
};

string rateUpCharacter = "Castorice";
string rateUpLightcones = "Make Farewell More Beautiful";
vector<string> lightcones = {"Before Dawn", "Night on the Milky Way", "Incessant Rain", "Earthly Escapade",
    "She Already Shut Her Eyes", "Reforged Remembrance", "Echoes of the Coffin"};
vector<string> standardLightcones = {"Sleep Like the Dead", "The Battle Isn't Over", "Moment of Victory"};

vector<string> threeStarLightcones = {
    "Darting Arrow",
    "Meshing Cogs",
    "Chorus",
    "So Adversarial",
    "Passkey",
    "Loop",
    "Multiplication",
    "Collapsing Sky",
    "Shattered Home",
    "Void"
};

vector<string> shopCharacters = {
    "Castorice", "Silver Wolf", "Sparkle", "Fu Xuan", "Black Swan", "Luocha"
};

vector<string> shopLightcones = {
    "Make Farewell More Beautiful", "Incessant Rain", "Earthly Escapade",
    "She Already Shut Her Eyes", "Reforged Remembrance", "Echoes of the Coffin"
};

map<string, int> ownedCharacters;
int starglitter = 0;
vector<string> team;

int pityCounter5RateUp = 0, pityCounter4RateUp = 0;
int pityCounter5Standard = 0, pityCounter4Standard = 0;
int pityCounter5Lightcone = 0, pityCounter4Lightcone = 0;
static bool guaranteedLightconeNext = false;

int getValidatedChoice(int min, int max);
int getChoice1to2();
int getChoice0to3();
int getChoice1to3();
int getChoice1to4();
char getCharNS();
char getYesOrNo();
char getCommand();
vector<int> getFourValidIndexes(int maxIndex);

// Deklarasi struktur data untuk battle history
// ====== BFS & DFS: Riwayat Battle ======
struct BattleAction {
    string actor;
    string target;
    string action;
    int damage;
    BattleAction* next = nullptr;
    
    BattleAction(string actor, string target, string action, int damage)
        : actor(actor), target(target), action(action), damage(damage), next(nullptr) {}
};

class BattleHistory {
public:
    string bossName;
    int totalActions = 0;
    BattleAction* head = nullptr; // First action
    
    BattleHistory() : head(nullptr) {}
    BattleHistory(string boss) : bossName(boss), head(nullptr) {}
    
    void addAction(string actor, string target, string action, int damage, int turn = 0, string desc = "") {
        BattleAction* newAction = new BattleAction(actor, target, action, damage);
        if (!head) {
            head = newAction;
        } else {
            BattleAction* curr = head;
            while (curr->next) curr = curr->next;
            curr->next = newAction;
        }
        totalActions++;
    }
    
    void clear() {
        while (head) {
            BattleAction* temp = head;
            head = head->next;
            delete temp;
        }
        totalActions = 0;
    }
    
    void showBFS() {
        cout << "\n=== Kronologi Battle (Urut dari awal) ===\n";
        if (!head) {
            cout << "Tidak ada aksi dalam riwayat.\n";
            return;
        }
        
        int count = 1;
        BattleAction* curr = head;
        while (curr) {
            cout << count << ". " << curr->actor << " " << curr->action << " " 
                 << curr->target << ", damage: " << curr->damage << "\n";
            curr = curr->next;
            count++;
        }
    }
    
    void showDFS() {
        cout << "\n=== Kronologi Battle (Urut dari akhir) ===\n";
        if (!head) {
            cout << "Tidak ada aksi dalam riwayat.\n";
            return;
        }
        
        // Collect actions in a vector for reverse iteration
        vector<pair<string, int>> actions;
        BattleAction* curr = head;
        while (curr) {
            string actionDesc = curr->actor + " " + curr->action + " " + curr->target + ", damage: " + to_string(curr->damage);
            actions.push_back({actionDesc, curr->damage});
            curr = curr->next;
        }
        
        // Display actions in reverse order
        for (int i = actions.size() - 1; i >= 0; i--) {
            cout << (actions.size() - i) << ". " << actions[i].first << "\n";
        }
    }
    
    void showPerformanceGraph() {
        cout << "\n=== Performance Graph ===\n";
        if (!head) {
            cout << "Tidak ada aksi dalam riwayat.\n";
            return;
        }
        
        // Collect damage info by actor
        map<string, int> totalDamage;
        map<string, int> actionCount;
        map<string, bool> isBoss; // Track if an actor is a boss
        
        // Assume the target of player characters is always a boss
        string bossName = "";
        BattleAction* curr = head;
        while (curr) {
            totalDamage[curr->actor] += curr->damage;
            actionCount[curr->actor]++;
            
            // If this is the first action, the target is likely the boss
            if (bossName.empty() && !curr->target.empty()) {
                bossName = curr->target;
                isBoss[bossName] = true;
            }
            
            curr = curr->next;
        }
        
        // Find max damage for scaling
        int maxDamage = 0;
        for (const auto& pair : totalDamage) {
            maxDamage = max(maxDamage, pair.second);
        }
        
        // Display damage distribution
        cout << "Damage Distribution:\n";
        for (const auto& pair : totalDamage) {
            string displayName = pair.first;
            if (isBoss[pair.first]) displayName += " (boss)";
            
            // Calculate bar length (max 40 characters)
            int barLength = (pair.second * 40) / maxDamage;
            string bar;
            for (int i = 0; i < barLength; i++) bar += "-";
            
            cout << left << setw(16) << displayName << ": " << bar << " " << pair.second << " damage\n";
        }
        
        // Display action counts
        cout << "\nAction Counts:\n";
        for (const auto& pair : actionCount) {
            string displayName = pair.first;
            if (isBoss[pair.first]) displayName += " (boss)";
            
            cout << left << setw(16) << displayName << ": " << pair.second << " actions\n";
        }
        
        // Display average damage per action
        cout << "\nAverage Damage Per Action:\n";
        for (const auto& pair : totalDamage) {
            string displayName = pair.first;
            if (isBoss[pair.first]) displayName += " (boss)";
            
            float avg = (float)pair.second / actionCount[pair.first];
            cout << left << setw(16) << displayName << ": " << fixed << setprecision(1) << avg << "\n";
        }
    }
    
    ~BattleHistory() {
        clear();
    }
};

vector<BattleHistory> allBattles;
vector<string> bossNames;
BattleHistory battleHistory;

struct Lightcone {
    string name;
    int bonusHP, bonusATK, bonusDEF;
    float bonusCritRate, bonusCritDamage;
};

struct Character {
    string name;
    int baseHP, baseATK, baseDEF;
    float baseCritRate, baseCritDamage;
    int currentHP = 0;
    int shield = 0;
    int skillCount = 0;
    CharacterType type;
    Lightcone* equippedLightcone = nullptr;

    int getTotalHP() const { return baseHP + (equippedLightcone ? equippedLightcone->bonusHP : 0); }
    int getTotalATK() const { return baseATK + (equippedLightcone ? equippedLightcone->bonusATK : 0); }
    int getTotalDEF() const { return baseDEF + (equippedLightcone ? equippedLightcone->bonusDEF : 0); }
    float getTotalCritRate() const { return baseCritRate + (equippedLightcone ? equippedLightcone->bonusCritRate : 0.0f); }
    float getTotalCritDamage() const { return baseCritDamage + (equippedLightcone ? equippedLightcone->bonusCritDamage : 0.0f); }

    void initHP() { currentHP = getTotalHP(); }
};

CharacterType getCharacterType(const string& name) {
    static set<string> dps = {"Castorice", "Clara", "Himeko", "Seele", "Yanqing", "Arlan", "Dan Heng", "Hook", "Serval", "Welt"};
    static set<string> buffer = {"Bronya", "Asta", "Tingyun", "Sparkle"};
    static set<string> debuffer = {"Pela", "Sampo", "Silver Wolf", "Black Swan"};
    static set<string> healer = {"Natasha", "Bailu", "Luocha"};
    static set<string> shielder = {"Gepard", "March 7th", "Fu Xuan"};

    if (dps.count(name)) return DPS;
    if (buffer.count(name)) return BUFFER;
    if (debuffer.count(name)) return DEBUFFER;
    if (healer.count(name)) return HEALER;
    if (shielder.count(name)) return SHIELDER;
    return DPS;
}

void applyEidolonStatBoosts(Character& ch, int eidolon) {
    if (eidolon <= 0) return;

    switch (ch.type) {
        case DPS:
            ch.baseCritDamage += 10.0f * eidolon; 
            break;
        case BUFFER:
            ch.baseATK += 10 * eidolon;  
            break;
        case DEBUFFER:
            ch.baseATK += 10 * eidolon;  
            break;
        case HEALER:
            ch.baseHP += 80 * eidolon;   
            break;
        case SHIELDER:
            ch.baseDEF += 40 * eidolon; 
            break;
    }
}

inline int getEidolonLevel(const string& name) {
    size_t pos = name.find("E");
    if (pos != string::npos)
        return stoi(name.substr(pos + 1));
    return 0;
}

string getBaseName(const string& fullName) {
    string name = fullName;
    // 1) Hapus suffix " E<number>" jika ada
    static const regex eidolonRe(R"(^(.*)\sE[1-6]$)");
    smatch m;
    if (regex_match(name, m, eidolonRe)) {
        name = m[1].str();
    }
    // 2) Hapus suffix "(Starglitter ...)" jika ada
    size_t pos = name.find(" (Starglitter");
    if (pos != string::npos) {
        name = name.substr(0, pos);
    }
    // 3) Trim spasi di ujung
    while (!name.empty() && isspace(name.back())) 
        name.pop_back();
    return name;
}

Character createCharacter(const string& rawName) {
    string name = getBaseName(rawName);
    if (name == "Castorice") // Rate-on (DPS)
        return {"Castorice", 1500, 380, 220, 20.0f, 70.0f}; // High crit
    if (name == "Clara" || name == "Seele" || name == "Yanqing" || name == "Himeko" || name == "Welt") // DPS 5
        return {name, 1350, 360, 210, 30.0f, 60.0f};
    if (name == "Gepard" || name == "Fu Xuan") // Shielder 5
        return {name, 1400, 310, 420, 6.0f, 50.0f}; 
    if (name == "Bronya" || name == "Sparkle") // Buffer 5
        return {name, 1250, 390, 190, 5.0f, 40.0f};
    if (name == "Silver Wolf" || name == "Black Swan") // Debuffer 5
        return {name, 1250, 370, 200, 10.0f, 50.0f};
    if (name == "Bailu" || name == "Luocha") // Healer 5
        return {name, 1700, 320, 180, 5.0f, 40.0f};
    if (name == "Arlan" || name == "Dan Heng" || name == "Hook" || name == "Serval") // DPS 4
        return {name, 1100, 300, 180, 20.0f, 50.0f};
    if (name == "Asta" || name == "Tingyun") // Buffer 4
        return {name, 1050, 320, 160, 4.0f, 30.0f};
    if (name == "Pela" || name == "Sampo") // Debuffer 4
        return {name, 1050, 310, 160, 8.0f, 30.0f};
    if (name == "Natasha") // Healer 4
        return {name, 1250, 270, 160, 3.0f, 20.0f};
    if (name == "March 7th") // Shielder 4
        return {name, 1200, 260, 280, 3.0f, 25.0f};    
    return {name, 900, 250, 150, 2.5f, 25.0f}; // Default untuk karakter yang tidak dikenali
}

Lightcone createLightcone(const string& name) {
    if (name == "Make Farewell More Beautiful")
        return {name, 400, 100, 80, 10.0f, 40.0f};
    if (name == "Before Dawn" || name == "Night on the Milky Way" ||
    name == "Incessant Rain" || name == "Earthly Escapade" ||
    name == "She Already Shut Her Eyes" || name == "Reforged Remembrance" ||
    name == "Echoes of the Coffin")
        return {name, 300, 80, 60, 5.0f, 30.0f};
    if (name == "Sleep Like the Dead" || name == "The Battle Isn't Over" || name == "Moment of Victory")
        return {name, 250, 65, 50, 3.0f, 20.0f};
    return {name, 100, 25, 20, 1.0f, 10.0f};
}

Lightcone* makeEquippedLightcone(const string& lcName) {
    Lightcone base = createLightcone(lcName);

    int s = lightconeSuperimposition[lcName];
    float multiplier = 1.0f + 0.01f * max(0, s - 1); // S1 = 1.0, S6 = 1.05

    base.bonusHP         = static_cast<int>(base.bonusHP * multiplier);
    base.bonusATK        = static_cast<int>(base.bonusATK * multiplier);
    base.bonusDEF        = static_cast<int>(base.bonusDEF * multiplier);
    base.bonusCritRate   = base.bonusCritRate * multiplier;
    base.bonusCritDamage = base.bonusCritDamage * multiplier;

    return new Lightcone(base);
}

void displayCharacterDetails(const Character& ch) {
    cout << "\n== Detail Karakter ==\n";
    cout << "Nama: " << ch.name << "\n";
    cout << "HP: " << ch.getTotalHP() << ", ATK: " << ch.getTotalATK() << ", DEF: " << ch.getTotalDEF() << "\n";
    cout << "Crit Rate: " << ch.getTotalCritRate() << "%, Crit Damage: " << ch.getTotalCritDamage() << "%\n";
    cout << "Lightcone: " << (ch.equippedLightcone ? ch.equippedLightcone->name : "Tidak ada") << "\n";
}

void merge(vector<string>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<string> L(arr.begin() + l, arr.begin() + m + 1);
    vector<string> R(arr.begin() + m + 1, arr.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] < R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<string>& arr, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int calculateDamage(int atk, int def, float critRate, float critDmg) {
    float chance = (rand() % 10000) / 100.0f;
    bool isCrit = chance < critRate;
    float dmg = atk - def;
    if (isCrit) dmg *= (1 + critDmg / 100.0f);
    return max(0, static_cast<int>(dmg));
}

int calculateBuff(int atk, float critRate, float critDmg) {
    float chance = (rand() % 10000) / 100.0f;
    bool isCrit = chance < critRate;
    float buff = atk * 0.5f;
    if (isCrit) buff *= (1 + critDmg / 100.0f);
    return static_cast<int>(buff);
}

int calculateDebuff(int atk, float critRate, float critDmg) {
    float chance = (rand() % 10000) / 100.0f;
    bool isCrit = chance < critRate;
    float debuff = atk * 0.4f;
    if (isCrit) debuff *= (1 + critDmg / 100.0f);
    return static_cast<int>(debuff);
}

int calculateShield(int def, float critRate, float critDmg) {
    float chance = (rand() % 10000) / 100.0f;
    bool isCrit = chance < critRate;
    float shield = def * 1.0f;
    if (isCrit) shield *= (1 + critDmg / 100.0f);
    return static_cast<int>(shield);
}

int calculateHeal(int hp, float critRate, float critDmg) {
    float chance = (rand() % 10000) / 100.0f;
    bool isCrit = chance < critRate;
    float heal = hp * 0.3f;
    if (isCrit) heal *= (1 + critDmg / 100.0f);
    return static_cast<int>(heal);
}


struct Boss {
    string name;
    int HP, ATK, DEF;
    float critRate, critDamage;
    int turnCounter = 0;
    int skillThreshold = 2;     // setiap 2 turn pakai skill
    int ultimateThreshold = 5;  // setiap 5 turn pakai ultimate
    int shield = 0;
};


map<string, bool> bossDefeated = {
    {"Cocolia", false},
    {"Feixiao", false},
    {"Aventurine", false}
};

Boss createBoss(const string& name) {
    if (name == "Cocolia") return {"Cocolia", 5000, 500, 200, 20.0f, 50.0f};
    if (name == "Feixiao") return {"Feixiao", 10000, 750, 500, 30.0f, 60.0f};
    if (name == "Aventurine") return {"Aventurine", 20000, 900, 300, 40.0f, 70.0f};
    return {"Unknown Boss", 1000, 100, 100, 1.0f, 10.0f};
}

void basicAttack(Character& ch, Boss& boss, int& points) {
    int dmg = calculateDamage(ch.getTotalATK(), boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
    boss.HP -= dmg;
    points = min(points + 1, 5);
    cout << ch.name << " melakukan basic attack! Boss kehilangan " << dmg << " HP. (Boss HP: " << boss.HP << ")\n";
}

void useSkill(Character& ch, Boss& boss, vector<Character>& team, int& points) {
    if (points <= 0) {
        cout << "Poin skill tidak cukup!\n";
        return;
    }
    points--;
    ch.skillCount++;

    switch (ch.type) {
        case DPS: {
            int dmg = calculateDamage(ch.getTotalATK(), boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan SKILL! Boss kehilangan " << dmg << " HP.\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case BUFFER: {
            int buff = calculateBuff(ch.getTotalATK(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            for (auto& ally : team)
                if (ally.currentHP > 0) ally.baseATK += buff;
            int dmg = calculateDamage(ch.getTotalATK() * 0.8, boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan SKILL! ATK tim bertambah " << buff << " dan Boss kehilangan " << dmg << " HP.\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case DEBUFFER: {
            int defDown = calculateDebuff(ch.getTotalATK(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.DEF = max(0, boss.DEF - defDown);
            int dmg = calculateDamage(ch.getTotalATK() * 0.8, boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan SKILL! Boss kehilangan " << dmg << " HP dan DEF berkurang " << defDown << "!\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case HEALER: {
            bool healed = false;
            for (auto& ally : team) {
                if (ally.currentHP > 0 && ally.currentHP < ally.getTotalHP()) {
                    int heal = calculateHeal(ch.getTotalHP(), ch.getTotalCritRate(), ch.getTotalCritDamage());
                    ally.currentHP = min(ally.getTotalHP(), ally.currentHP + heal);
                    cout << ch.name << " menggunakan SKILL! " << ally.name << " memulihkan " << heal << " HP!\n";
                    healed = true;
                    break;
                }
            }
            if (!healed) cout << ch.name << " menggunakan SKILL tapi semua anggota sudah full HP.\n";
            break;
        }
        case SHIELDER: {
            bool shielded = false;
            for (auto& ally : team) {
                if (ally.currentHP > 0) {
                    int shield = calculateShield(ch.getTotalDEF(), ch.getTotalCritRate(), ch.getTotalCritDamage());
                    ally.shield += shield;
                    cout << ch.name << " menggunakan SKILL! " << ally.name << " mendapatkan shield " << shield << " HP!\n";
                    shielded = true;
                    break;
                }
            }
            if (!shielded) cout << ch.name << " menggunakan SKILL tapi tidak ada yang bisa diberi shield.\n";
            break;
        }
    }
}

void useUltimate(Character& ch, Boss& boss, vector<Character>& team) {
    if (ch.skillCount < 3) {
        cout << "Ultimate belum siap! (Progress: " << ch.skillCount << "/3)\n";
        return;
    }

    ch.skillCount = 0;

    switch (ch.type) {
        case DPS: {
            int dmg = calculateDamage(ch.getTotalATK() * 2.5, boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan ULTIMATE! Boss kehilangan " << dmg << " HP.\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case BUFFER: {
            int buff = calculateBuff(ch.getTotalATK(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            for (auto& ally : team)
                if (ally.currentHP > 0) {
                    ally.baseATK += buff;
                    ally.baseCritRate += 15.0f;
                }
            int dmg = calculateDamage(ch.getTotalATK(), boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan ULTIMATE! Seluruh tim mendapat ATK+" << buff << ", Crit Rate+15%. Boss kehilangan " << dmg << " HP.\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case DEBUFFER: {
            int defDown = calculateDebuff(ch.getTotalATK(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.DEF = max(0, boss.DEF - defDown);
            boss.critRate = max(0.0f, boss.critRate - 15.0f);
            int dmg = calculateDamage(ch.getTotalATK(), boss.DEF, ch.getTotalCritRate(), ch.getTotalCritDamage());
            boss.HP -= dmg;
            cout << ch.name << " menggunakan ULTIMATE! Boss kehilangan " << dmg << " HP, DEF -" << defDown << ", dan Crit Rate -15%!\n";
            cout << "Boss HP: " << boss.HP << "\n";
            break;
        }
        case HEALER: {
            int heal = calculateHeal(ch.getTotalHP(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            for (auto& ally : team)
                if (ally.currentHP > 0)
                    ally.currentHP = min(ally.getTotalHP(), ally.currentHP + heal);
            cout << ch.name << " menggunakan ULTIMATE! Semua anggota tim memulihkan " << heal << " HP!\n";
            break;
        }
        case SHIELDER: {
            int shield = calculateShield(ch.getTotalDEF(), ch.getTotalCritRate(), ch.getTotalCritDamage());
            for (auto& ally : team)
                if (ally.currentHP > 0)
                    ally.shield += shield;
            cout << ch.name << " menggunakan ULTIMATE! Semua anggota tim mendapatkan shield " << shield << " HP!\n";
            break;
        }
    }
}

void bossSkill(Boss& boss, vector<Character>& team) {
    if (boss.name == "Cocolia") {
        // Skill = serangan ke seluruh tim
        cout << boss.name << " menggunakan SKILL: Serangan AOE!\n";
        for (auto& target : team) {
            if (target.currentHP <= 0) continue;
            int dmg = calculateDamage(boss.ATK * 1.2, target.getTotalDEF(), boss.critRate, boss.critDamage);
            if (target.shield > 0) {
                int absorbed = min(dmg, target.shield);
                target.shield -= absorbed;
                dmg -= absorbed;
                cout << target.name << " diserang, shield menyerap " << absorbed << "!\n";
            }
            target.currentHP -= dmg;
            cout << target.name << " menerima " << dmg << " damage dari skill.\n";
        }
    } else if (boss.name == "Feixiao") {
        // Skill = buff ATK
        boss.ATK += 100;
        cout << boss.name << " menggunakan SKILL: Meningkatkan ATK-nya sebesar 100!\n";
    } else if (boss.name == "Aventurine") {
        // Skill = shield untuk diri sendiri
        boss.shield += 300;
        cout << boss.name << " menggunakan SKILL: Memberikan shield 300 HP ke dirinya!\n";
    }
}

void bossUltimate(Boss& boss, vector<Character>& team) {
    if (boss.name == "Cocolia") {
        cout << boss.name << " menggunakan ULTIMATE: Serangan Super AOE!\n";
        for (auto& target : team) {
            if (target.currentHP <= 0) continue;
            int dmg = calculateDamage(boss.ATK * 1.8, target.getTotalDEF(), boss.critRate, boss.critDamage);
            if (target.shield > 0) {
                int absorbed = min(dmg, target.shield);
                target.shield -= absorbed;
                dmg -= absorbed;
                cout << target.name << " diserang, shield menyerap " << absorbed << "!\n";
            }
            target.currentHP -= dmg;
            cout << target.name << " menerima " << dmg << " damage dari ultimate!\n";
        }
    } else if (boss.name == "Feixiao") {
        // Ultimate = serangan tunggal sakit
        int idx = rand() % team.size();
        Character& target = team[idx];
        int dmg = calculateDamage(boss.ATK * 2.0, target.getTotalDEF(), boss.critRate, boss.critDamage);
        target.currentHP -= dmg;
        cout << boss.name << " menggunakan ULTIMATE! " << target.name << " terkena " << dmg << " damage berat!\n";
    } else if (boss.name == "Aventurine") {
        boss.HP += 500;
        if (boss.HP > 10000) boss.HP = 10000;
        cout << boss.name << " menggunakan ULTIMATE: Menyembuhkan diri 500 HP dan menyerang!\n";
        for (auto& target : team) {
            if (target.currentHP <= 0) continue;
            int dmg = calculateDamage(boss.ATK * 1.5, target.getTotalDEF(), boss.critRate, boss.critDamage);
            target.currentHP -= dmg;
            cout << target.name << " terkena " << dmg << " damage dari Aventurine.\n";
        }
    }
}

void battle(vector<string>& teamNames) {
    string currentBossName = "";
    battleHistory.clear(); // Reset history setiap mulai battle
    string bossName;
    
    while (true) {
        cout << "\n== Pilih Bos ==\n";
        int i = 1;
        vector<string> bosses = {"Cocolia", "Feixiao", "Aventurine"};
        for (const auto& name : bosses) {
            cout << i++ << ". " << name;
            if (bossDefeated[name]) cout << " [Defeated]";
            cout << "\n";
        }
        cout << "0. Kembali ke menu\n";

        int choice;
        cout << "Pilih bos (0-" << bosses.size() << "): ";
        choice = getChoice0to3();

        if (choice == 0) return;
        if (choice >= 1 && choice <= bosses.size()) {
            bossName = bosses[choice - 1];
            if (bossDefeated[bossName]) {
                cout << "Kamu sudah mengalahkan " << bossName << ". Ingin bertarung lagi? (y/n): ";
                char again;
                again = getYesOrNo();
                if (again == 'y' || again == 'Y') break;
                else continue;
            } else {
                break;
            }
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }

    // Setup karakter
    vector<Character> team;
    for (const string& name : teamNames) {
        string baseName = getBaseName(name);
        Character ch = createCharacter(baseName);

        int eidolonLevel = 0;
        if (name.find("E") != string::npos) {
            eidolonLevel = stoi(name.substr(name.find("E") + 1));
        }

        ch.type = getCharacterType(baseName);         
        applyEidolonStatBoosts(ch, eidolonLevel);     

        if (equippedLightcones.count(baseName)) {
            ch.equippedLightcone = makeEquippedLightcone(equippedLightcones[baseName]);
        }

        ch.initHP();
        ch.name = name;  
        team.push_back(ch);
    }


    Boss boss = createBoss(bossName);
    int actionPoints = 5;
    int turn = 0;

    while (boss.HP > 0 && any_of(team.begin(), team.end(), [](Character& c){ return c.currentHP > 0; })) {
        cout << "\n==== Giliran ====\n";
        for (int i = 0; i < team.size(); ++i)
            if (team[i].currentHP > 0)
                cout << i + 1 << ". " << team[i].name << "\n";
        cout << "Boss: " << boss.name << " (HP: " << boss.HP << ")\n";
        cout << "Poin Aksi: " << actionPoints << "\n";

        Character& ch = team[turn % team.size()];
        if (ch.currentHP <= 0) {
            turn++;
            continue;
        }

        cout << "\n" << ch.name << ":\n";
        cout << "HP: " << ch.currentHP << "/" << ch.getTotalHP() << "\n";
        cout << "1. Basic Attack\n2. Skill\n3. Ultimate (" << ch.skillCount << "/3)\n";
        int input;
        cout << "Pilih aksi: ";
        input = getChoice1to3();

        switch (input) {
            case 1: {
                int hpBefore = boss.HP;
                basicAttack(ch, boss, actionPoints);
                int dmg = max(0, hpBefore - boss.HP);
                string desc = ch.name + " melakukan Basic Attack ke " + boss.name + " (" + to_string(dmg) + " damage)";
                battleHistory.addAction(ch.name, "BasicAttack", boss.name, dmg, turn, desc);
                break;
            }
            case 2: {
                int hpBefore = boss.HP;
                useSkill(ch, boss, team, actionPoints);
                int dmg = max(0, hpBefore - boss.HP);
                string desc = ch.name + " menggunakan Skill ke " + boss.name + " (" + to_string(dmg) + " damage)";
                battleHistory.addAction(ch.name, "Skill", boss.name, dmg, turn, desc);
                break;
            }
            case 3: {
                int hpBefore = boss.HP;
                useUltimate(ch, boss, team);
                int dmg = max(0, hpBefore - boss.HP);
                string desc = ch.name + " mengeluarkan Ultimate ke " + boss.name + " (" + to_string(dmg) + " damage)";
                battleHistory.addAction(ch.name, "Ultimate", boss.name, dmg, turn, desc);
                break;
            }
            default: cout << "Pilihan tidak valid.\n"; continue;
        }

        // giliran boss
        if ((turn + 1) % team.size() == 0 && boss.HP > 0) {
            boss.turnCounter++;

        // Ultimate > Skill > Basic Attack
        if (boss.turnCounter % boss.ultimateThreshold == 0) {
            bossUltimate(boss, team);
        } else if (boss.turnCounter % boss.skillThreshold == 0) {
            bossSkill(boss, team);
        } else {
            // Basic attack seperti biasa
            vector<int> alive;
            for (int i = 0; i < team.size(); ++i)
                if (team[i].currentHP > 0)
                    alive.push_back(i);

            int targetIdx = alive[rand() % alive.size()];
            Character& target = team[targetIdx];
            int dmg = calculateDamage(boss.ATK, target.getTotalDEF(), boss.critRate, boss.critDamage);

            if (boss.shield > 0) {
                cout << boss.name << " punya shield " << boss.shield << ", tetapi tidak memengaruhi serangan!\n";
            }

            if (target.shield > 0) {
                int absorbed = min(dmg, target.shield);
                target.shield -= absorbed;
                dmg -= absorbed;
                cout << "\n" << boss.name << " menghantam " << target.name << " tapi shield menyerap " << absorbed << "!\n";
            }

            target.currentHP -= dmg;
            cout << boss.name << " menyerang " << target.name << " dengan " << dmg << " damage.\n";
            string desc = boss.name + " menyerang " + target.name + " (" + to_string(dmg) + " damage)";
            battleHistory.addAction(boss.name, "BossAttack", target.name, dmg, turn, desc);
        }
    }

        turn++;
    }

    if (boss.HP <= 0) {
        cout << "\nBoss dikalahkan!\n";
        bossDefeated[boss.name] = true;
        currentBossName = boss.name;
        allBattles.push_back(battleHistory);
        bossNames.push_back(boss.name);
    } else {
        cout << "\nTim kamu kalah...\n";
    }
}

// ===== SHOP AVL TREE SEDERHANA =====
struct ShopItem {
    string name;
    int price;
    int stock; // -1 for characters with no stock limit
    bool isCharacter; // true for character, false for lightcone
    ShopItem(string n, int p, int s, bool ic = false) : name(n), price(p), stock(s), isCharacter(ic) {}
};

struct AVLShopNode {
    ShopItem item;
    AVLShopNode* left;
    AVLShopNode* right;
    int height;
    AVLShopNode(const ShopItem& i) : item(i), left(nullptr), right(nullptr), height(1) {}
};

// ====== AVL Tree: Shop ======
class AVLShopTree {
    AVLShopNode* root = nullptr;
    int height(AVLShopNode* n) { return n ? n->height : 0; }
    int getBalance(AVLShopNode* n) { return n ? height(n->left) - height(n->right) : 0; }
    AVLShopNode* rightRotate(AVLShopNode* y) {
        AVLShopNode* x = y->left;
        AVLShopNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        return x;
    }
    AVLShopNode* leftRotate(AVLShopNode* x) {
        AVLShopNode* y = x->right;
        AVLShopNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        return y;
    }
    AVLShopNode* insert(AVLShopNode* node, const ShopItem& item) {
        if (!node) return new AVLShopNode(item);
        if (item.name < node->item.name)
            node->left = insert(node->left, item);
        else if (item.name > node->item.name)
            node->right = insert(node->right, item);
        else
            return node; // duplikat nama tidak diinsert
        node->height = 1 + std::max(height(node->left), height(node->right));
        int balance = getBalance(node);
        // Left Left
        if (balance > 1 && item.name < node->left->item.name)
            return rightRotate(node);
        // Right Right
        if (balance < -1 && item.name > node->right->item.name)
            return leftRotate(node);
        // Left Right
        if (balance > 1 && item.name > node->left->item.name) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // Right Left
        if (balance < -1 && item.name < node->right->item.name) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }
    void inorder(AVLShopNode* node) {
        if (!node) return;
        inorder(node->left);
        std::cout << "- " << node->item.name << " | Harga: " << node->item.price << " | Stok: " << node->item.stock << "\n";
        inorder(node->right);
    }
    
    // Collect all shop items into a vector for custom sorting
    void collectItems(AVLShopNode* node, std::vector<ShopItem>& items) {
        if (!node) return;
        collectItems(node->left, items);
        items.push_back(node->item);
        collectItems(node->right, items);
    }
    void clear(AVLShopNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
public:
    ~AVLShopTree() { clear(root); }
    // Find a shop item by name
    AVLShopNode* findItem(AVLShopNode* node, const string& name) {
        if (!node) return nullptr;
        if (name == node->item.name) return node;
        if (name < node->item.name) return findItem(node->left, name);
        return findItem(node->right, name);
    }
    
    // Update item stock
    void updateStock(AVLShopNode* node, const string& name, int newStock) {
        if (!node) return;
        if (node->item.name == name) {
            node->item.stock = newStock;
            return;
        }
        if (name < node->item.name) updateStock(node->left, name, newStock);
        else updateStock(node->right, name, newStock);
    }
    
public:
    void insert(const ShopItem& item) { root = insert(root, item); }
    
    // Purchase an item from the shop
    bool purchaseItem(const string& itemName, int& starglitterBalance) {
        AVLShopNode* itemNode = findItem(root, itemName);
        
        if (!itemNode) {
            std::cout << "\nItem tidak ditemukan di shop!\n";
            return false;
        }
        
        // Check if enough starglitter
        if (starglitterBalance < itemNode->item.price) {
            std::cout << "\nStarglitter tidak cukup! Dibutuhkan: " << itemNode->item.price 
                      << ", Dimiliki: " << starglitterBalance << "\n";
            return false;
        }
        
        // Check stock for lightcones (characters don't have stock limits)
        if (!itemNode->item.isCharacter && itemNode->item.stock <= 0) {
            std::cout << "\nStok lightcone habis!\n";
            return false;
        }
        
        // Process the purchase
        starglitterBalance -= itemNode->item.price;
        
        if (itemNode->item.isCharacter) {
            // Character purchase - add to ownedCharacters or increment eidolon
            if (++ownedCharacters[itemNode->item.name] > 6) {
                // Max eidolon already, return starglitter
                ownedCharacters[itemNode->item.name]--;
                starglitterBalance += 40; // Refund for duplicate over max
                std::cout << "\nKarakter sudah maksimal Eidolon! Mendapatkan 40 Starglitter sebagai kompensasi.\n";
            } else {
                int eidolon = ownedCharacters[itemNode->item.name] - 1;
                if (eidolon == 0) {
                    std::cout << "\nBerhasil membeli karakter: " << itemNode->item.name << "!\n";
                    // Item successfully added to inventory
                    // (Character tree is updated elsewhere when displaying)
                
                    // Decrease stock for characters since they can only be bought once
                    updateStock(root, itemNode->item.name, 0);
                } else {
                    std::cout << "\nBerhasil meningkatkan karakter " << itemNode->item.name 
                              << " ke Eidolon " << eidolon << "!\n";
                }
            }
        } else {
            // Lightcone purchase - add to superimposition or increment
            if (++lightconeSuperimposition[itemNode->item.name] > 5) {
                // Max superimposition already, return starglitter
                lightconeSuperimposition[itemNode->item.name]--;
                starglitterBalance += 15; // Refund for duplicate over max
                std::cout << "\nLightcone sudah maksimal Superimposition! Mendapatkan 15 Starglitter sebagai kompensasi.\n";
            } else {
                int superimposition = lightconeSuperimposition[itemNode->item.name];
                if (superimposition == 1) {
                    std::cout << "\nBerhasil membeli lightcone: " << itemNode->item.name << "!\n";
                    // Item successfully added to inventory
                    // (Lightcone tree is updated elsewhere when displaying)
                } else {
                    std::cout << "\nBerhasil meningkatkan lightcone " << itemNode->item.name 
                              << " ke Superimposition " << superimposition << "!\n";
                }
            }
            
            // Decrease stock for lightcones
            updateStock(root, itemNode->item.name, itemNode->item.stock - 1);
        }
        
        if ((itemNode->item.isCharacter && ownedCharacters[itemNode->item.name] == 1) ||
            (!itemNode->item.isCharacter && lightconeSuperimposition[itemNode->item.name] == 1)) {
        }
        
        return true;
    }
    // Display by name (alphabetical) using merge sort
    void displayByName() {
        std::vector<ShopItem> items;
        collectItems(root, items);
        
        // Sort items by name using merge sort
        if (!items.empty()) {
            mergeSort(items, 0, items.size()-1, false); // false = sort by name
        }
        
        std::cout << "\n=== SHOP ITEM LIST (BY NAME) ===\n";
        if (items.empty()) std::cout << "(Shop kosong)\n";
        else {
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            std::cout << "| " << std::left << std::setw(38) << "Item Name" << " | ";
            std::cout << std::setw(10) << "Type" << " | ";
            std::cout << std::setw(8) << "Price" << " | ";
            std::cout << std::setw(7) << "Stock" << " |\n";
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            
            for (const auto& item : items) {
                std::cout << "| " << std::left << std::setw(38) << item.name << " | ";
                
                // Determine if it's a character or lightcone
                bool isChar = false;
                for (const auto& c : shopCharacters) {
                    if (c == item.name) {
                        isChar = true;
                        break;
                    }
                }
                
                std::cout << std::setw(10) << (isChar ? "Character" : "Lightcone") << " | ";
                std::cout << std::right << std::setw(8) << item.price << " | ";
                
                // Always display the actual stock value
                std::cout << std::setw(7) << item.stock << " |\n";
            }
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
        }
    }
    
    // Custom merge function for ShopItems
    void merge(vector<ShopItem>& arr, int l, int m, int r, bool byPrice) {
        int n1 = m - l + 1, n2 = r - m;
        vector<ShopItem> L(arr.begin() + l, arr.begin() + m + 1);
        vector<ShopItem> R(arr.begin() + m + 1, arr.begin() + r + 1);

        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            if ((byPrice && L[i].price < R[j].price) || 
                (!byPrice && L[i].name < R[j].name))
                arr[k++] = L[i++];
            else
                arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    // MergeSort for ShopItems
    void mergeSort(vector<ShopItem>& arr, int l, int r, bool byPrice) {
        if (l < r) {
            int m = (l + r) / 2;
            mergeSort(arr, l, m, byPrice);
            mergeSort(arr, m + 1, r, byPrice);
            merge(arr, l, m, r, byPrice);
        }
    }
    
    // Custom merge function for Stock sorting (descending)
    void mergeByStock(vector<ShopItem>& arr, int l, int m, int r) {
        int n1 = m - l + 1, n2 = r - m;
        vector<ShopItem> L(arr.begin() + l, arr.begin() + m + 1);
        vector<ShopItem> R(arr.begin() + m + 1, arr.begin() + r + 1);

        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            if (L[i].stock > R[j].stock) // Note: descending
                arr[k++] = L[i++];
            else
                arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    // MergeSort for Stock (descending)
    void mergeSortByStock(vector<ShopItem>& arr, int l, int r) {
        if (l < r) {
            int m = (l + r) / 2;
            mergeSortByStock(arr, l, m);
            mergeSortByStock(arr, m + 1, r);
            mergeByStock(arr, l, m, r);
        }
    }
    
    // Display by price (low to high)
    void displayByPrice() {
        std::vector<ShopItem> items;
        collectItems(root, items);
        
        // Sort items by price using merge sort
        if (!items.empty()) {
            mergeSort(items, 0, items.size()-1, true); // true = sort by price
        }
        
        std::cout << "\n=== SHOP ITEM LIST (BY PRICE: LOW TO HIGH) ===\n";
        if (items.empty()) std::cout << "(Shop kosong)\n";
        else {
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            std::cout << "| " << std::left << std::setw(38) << "Item Name" << " | ";
            std::cout << std::setw(10) << "Type" << " | ";
            std::cout << std::setw(8) << "Price" << " | ";
            std::cout << std::setw(7) << "Stock" << " |\n";
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            
            for (const auto& item : items) {
                std::cout << "| " << std::left << std::setw(38) << item.name << " | ";
                
                // Determine if it's a character or lightcone
                bool isChar = false;
                for (const auto& c : shopCharacters) {
                    if (c == item.name) {
                        isChar = true;
                        break;
                    }
                }
                
                std::cout << std::setw(10) << (isChar ? "Character" : "Lightcone") << " | ";
                std::cout << std::right << std::setw(8) << item.price << " | ";
                
                // Always display the actual stock value
                std::cout << std::setw(7) << item.stock << " |\n";
            }
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
        }
    }
    
    // Display by stock availability (high to low)
    void displayByStock() {
        std::vector<ShopItem> items;
        collectItems(root, items);
        
        // Filter out character items with -1 stock since they're not applicable for stock display
        std::vector<ShopItem> stockItems;
        for (const auto& item : items) {
            if (item.stock != -1) {
                stockItems.push_back(item);
            }
        }
        
        // Sort items by stock (descending) using merge sort
        if (!stockItems.empty()) {
            mergeSortByStock(stockItems, 0, stockItems.size()-1);
        }
        
        std::cout << "\n=== SHOP ITEM LIST (BY STOCK: HIGH TO LOW) ===\n";
        std::cout << "(Only showing items with applicable stock)\n";
        
        if (stockItems.empty()) std::cout << "(No items with stock available)\n";
        else {
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            std::cout << "| " << std::left << std::setw(38) << "Item Name" << " | ";
            std::cout << std::setw(10) << "Type" << " | ";
            std::cout << std::setw(8) << "Price" << " | ";
            std::cout << std::setw(7) << "Stock" << " |\n";
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
            
            for (const auto& item : stockItems) {
                std::cout << "| " << std::left << std::setw(38) << item.name << " | ";
                std::cout << std::setw(10) << "Lightcone" << " | "; // Only lightcones have stock
                std::cout << std::right << std::setw(8) << item.price << " | ";
                std::cout << std::setw(7) << item.stock << " |\n";
            }
            std::cout << "+" << std::string(40, '-') << "+" << std::string(12, '-') << "+" << std::string(10, '-') << "+" << std::string(9, '-') << "+\n";
        }
    }
    
    // Display the AVL tree structure with balance factors
    void displayAVLStructure() {
        std::cout << "\n=== AVL SHOP TREE STRUCTURE ===\n";
        std::cout << "(Root at the top, shows balance factors in parentheses)\n\n";
        if (!root) {
            std::cout << "(Empty tree)\n";
            return;
        }
        displayAVLStructureRecursive(root, "", true);
    }
    
    void displayAVLStructureRecursive(AVLShopNode* node, std::string prefix, bool isLast) {
        if (!node) return;
        
        std::cout << prefix;
        std::cout << (isLast ? "+-- " : "+-- ");
        
        // Display node with balance factor
        int balance = getBalance(node);
        std::cout << node->item.name << " (" << balance << ") [Stock: " << node->item.stock << "]\n";
        
        // Prepare prefix for children
        std::string newPrefix = prefix + (isLast ? "    " : "|   ");
        
        // Process children
        if (node->right)
            displayAVLStructureRecursive(node->right, newPrefix, node->left == nullptr);
        if (node->left)
            displayAVLStructureRecursive(node->left, newPrefix, true);
    }
    
    // Display AVL tree height and node count statistics
    void displayAVLStats() {
        int height = calculateHeight(root);
        int nodeCount = countNodes(root);
        int leafCount = countLeafNodes(root);
        
        std::cout << "\n=== AVL TREE STATISTICS ===\n";
        std::cout << "Tree Height: " << height << "\n";
        std::cout << "Total Nodes: " << nodeCount << "\n";
        std::cout << "Leaf Nodes: " << leafCount << "\n";
        std::cout << "Internal Nodes: " << (nodeCount - leafCount) << "\n";
        
        // Calculate theoretical min/max height for comparison
        int minHeight = static_cast<int>(log2(nodeCount + 1));
        int maxHeight = static_cast<int>(1.44 * log2(nodeCount + 2) - 0.328);
        
        std::cout << "\nBalance Analysis:\n";
        std::cout << "Theoretical Min Height: " << minHeight << "\n";
        std::cout << "Theoretical Max Height for AVL: " << maxHeight << "\n";
        std::cout << "Actual/Theoretical Height Ratio: " << std::fixed << std::setprecision(2) 
                  << (static_cast<float>(height) / minHeight) << "\n";
    }
    
    int calculateHeight(AVLShopNode* node) {
        if (!node) return 0;
        return 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));
    }
    
    int countNodes(AVLShopNode* node) {
        if (!node) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }
    
    int countLeafNodes(AVLShopNode* node) {
        if (!node) return 0;
        if (!node->left && !node->right) return 1;
        return countLeafNodes(node->left) + countLeafNodes(node->right);
    }
    
    // Legacy display method - redirects to displayByName
    void display() {
        displayByName();
    }
};

AVLShopTree shopTree;

void initShopItems() {
    // Make sure random is initialized
    static bool initialized = false;
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }
    
    // Clear existing items first to avoid duplicates when re-initializing
    shopTree = AVLShopTree();
    
    // Insert characters with stock of 1 (can only be purchased once)
    for (const auto& character : shopCharacters) {
        // Fixed price of 5000 for all characters as requested
        int price = 5000;
        shopTree.insert(ShopItem(character, price, 1, true)); // Stock of 1 means can only be purchased once, true = is a character
    }
    
    // Insert lightcones with stock between 1-3 and isCharacter set to false
    // with variable pricing between 3400-5000
    for (const auto& lightcone : shopLightcones) {
        // Varied price for lightcones between 3400-5000
        int price = 3400 + (rand() % 1601); // Random price between 3400-5000
        int stock = (rand() % 3) + 1; // Random stock between 1-3
        shopTree.insert(ShopItem(lightcone, price, stock, false)); // false = is a lightcone
    }
}

// Forward declarations for shop menus
void showCharacterShop();
void showLightconeShop();

// Main shop menu function
void showShopMenu() {
    initShopItems();
    
    bool exitShop = false;
    
    while (!exitShop) {
        system("cls");
        std::cout << "\n===== SHOP MENU =====\n";
        std::cout << "Starglitter Anda: " << starglitter << "\n";
        std::cout << "1. Character Shop\n";
        std::cout << "2. Lightcone Shop\n";
        std::cout << "3. Kembali ke Menu Utama\n";
        std::cout << "Pilihan: ";
        
        int menuChoice;
        menuChoice = getChoice1to3();
        
        switch(menuChoice) {
            case 1:
                showCharacterShop();
                break;
            case 2:
                showLightconeShop();
                break;
            case 3:
                exitShop = true;
                break;
            default:
                std::cout << "Pilihan tidak valid!\n";
                system("pause");
        }
    }
}

// Character shop menu with specialized views for characters
void showCharacterShop() {
    bool exitCharacterShop = false;
    int viewMode = 0; // 0: by name, 1: by price, 2: AVL structure, 3: AVL stats
    
    while (!exitCharacterShop) {
        system("cls");
        std::cout << "\n===== CHARACTER SHOP =====\n";
        std::cout << "Starglitter Anda: " << starglitter << "\n";
        
        // Create character-specific AVL tree
        AVLShopTree characterShopTree;
        for (const auto& character : shopCharacters) {
            int price = 5000;
            characterShopTree.insert(ShopItem(character, price, 1, true)); // Stock 1 for characters
        }
        
        // Display characters based on current view mode
        switch (viewMode) {
            case 0: characterShopTree.displayByName(); break;
            case 1: characterShopTree.displayByPrice(); break;
            case 2: characterShopTree.displayAVLStructure(); break;
            case 3: characterShopTree.displayAVLStats(); break;
        }
        
        std::cout << "[t] Toggle View Mode (" << 
            (viewMode == 0 ? "Name -> Price" : 
             viewMode == 1 ? "Price -> AVL Structure" : 
             viewMode == 2 ? "AVL Structure -> AVL Stats" : "AVL Stats -> Name") << ")\n";
        
        // Only show purchase option if not in AVL structure or stats view mode
        if (viewMode < 2) {
            std::cout << "[p] Beli Character\n";
        }
        
        // AVL-specific explanations
        if (viewMode == 2) {
            std::cout << "\n--- AVL Tree Information ---\n";
            std::cout << "AVL trees maintain balance through rotations.\n";
            std::cout << "Balance factors shown in parentheses (height of right - height of left).\n";
            std::cout << "For a balanced AVL tree, all nodes must have balance factors of -1, 0, or 1.\n";
        } 
        else if (viewMode == 3) {
            std::cout << "\n--- Why Use AVL Trees? ---\n";
            std::cout << "- Search/insert/delete operations all work in O(log n) guaranteed time\n";
            std::cout << "- Self-balancing ensures tree doesn't degrade to linked list\n";
            std::cout << "- Height is always ~1.44*log2(n), making searches efficient\n";
        }

        std::cout << "[b] Kembali ke Shop Menu\n";
        std::cout << "Pilihan: ";
        char option;
        std::cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        
        switch(option) {
            case 't': case 'T':
                // Toggle view mode in a cycle (4 modes for characters)
                viewMode = (viewMode + 1) % 4;
                break;
            case 'p': case 'P':
                if (viewMode < 2) { // Only allow purchases from item views
                    // Purchase functionality
                    std::cin.ignore(); // Clear input buffer
                    std::cout << "Masukkan nama character yang ingin dibeli: ";
                    std::string itemName;
                    getline(std::cin, itemName);
                    
                    // Process purchase using the global shop tree
                    if (shopTree.purchaseItem(itemName, starglitter)) {
                        // Success message - saving is handled in the purchaseItem method
                        std::cout << "Pembelian character berhasil! Starglitter tersisa: " << starglitter << "\n";
                    } else {
                        std::cout << "Gagal membeli character. Pastikan nama karakter benar.\n";
                    }
                    system("pause");
                } else {
                    std::cout << "Untuk membeli character, silahkan gunakan tampilan nama atau harga.\n";
                    system("pause");
                }
                break;
            case 'b': case 'B':
                exitCharacterShop = true;
                break;
            default:
                std::cout << "Pilihan tidak valid!\n";
                system("pause");
        }
    }
}

// Lightcone shop menu with all view modes including stock
void showLightconeShop() {
    bool exitLightconeShop = false;
    int viewMode = 0; // 0: by name, 1: by price, 2: by stock, 3: AVL structure, 4: AVL stats
    
    while (!exitLightconeShop) {
        system("cls");
        std::cout << "\n===== LIGHTCONE SHOP =====\n";
        std::cout << "Starglitter Anda: " << starglitter << "\n";
        
        // Create lightcone-specific AVL tree
        AVLShopTree lightconeShopTree;
        for (const auto& lightcone : shopLightcones) {
            int price = 3400 + (rand() % 1601); // Random price between 3400-5000
            int stock = (rand() % 3) + 1; // Random stock between 1-3
            lightconeShopTree.insert(ShopItem(lightcone, price, stock, false));
        }
        
        // Display lightcones based on current view mode
        switch (viewMode) {
            case 0: lightconeShopTree.displayByName(); break;
            case 1: lightconeShopTree.displayByPrice(); break;
            case 2: lightconeShopTree.displayByStock(); break;
            case 3: lightconeShopTree.displayAVLStructure(); break;
            case 4: lightconeShopTree.displayAVLStats(); break;
        }
        
        std::cout << "[t] Toggle View Mode (" << 
            (viewMode == 0 ? "Name -> Price" : 
             viewMode == 1 ? "Price -> Stock" : 
             viewMode == 2 ? "Stock -> AVL Structure" :
             viewMode == 3 ? "AVL Structure -> AVL Stats" : "AVL Stats -> Name") << ")\n";
        
        // Only show purchase option if not in AVL structure or stats view mode
        if (viewMode < 3) {
            std::cout << "[p] Beli Lightcone\n";
        }
        
        // AVL-specific explanations
        if (viewMode == 3) {
            std::cout << "\n--- AVL Tree Information ---\n";
            std::cout << "AVL trees maintain balance through rotations.\n";
            std::cout << "Balance factors shown in parentheses (height of right - height of left).\n";
            std::cout << "For a balanced AVL tree, all nodes must have balance factors of -1, 0, or 1.\n";
        } 
        else if (viewMode == 4) {
            std::cout << "\n--- Why Use AVL Trees? ---\n";
            std::cout << "- Search/insert/delete operations all work in O(log n) guaranteed time\n";
            std::cout << "- Self-balancing ensures tree doesn't degrade to linked list\n";
            std::cout << "- Height is always ~1.44*log2(n), making searches efficient\n";
        }

        std::cout << "[b] Kembali ke Shop Menu\n";
        std::cout << "Pilihan: ";
        char option;
        std::cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        
        switch(option) {
            case 't': case 'T':
                // Toggle view mode in a cycle (5 modes for lightcones)
                viewMode = (viewMode + 1) % 5;
                break;
            case 'p': case 'P':
                if (viewMode < 3) { // Only allow purchases from item views
                    // Purchase functionality
                    std::cin.ignore(); // Clear input buffer
                    std::cout << "Masukkan nama lightcone yang ingin dibeli: ";
                    std::string itemName;
                    getline(std::cin, itemName);
                    
                    // Process purchase using the global shop tree
                    if (shopTree.purchaseItem(itemName, starglitter)) {
                        // Success message - saving is handled in the purchaseItem method
                        std::cout << "Pembelian lightcone berhasil! Starglitter tersisa: " << starglitter << "\n";
                    } else {
                        std::cout << "Gagal membeli lightcone. Pastikan nama lightcone benar dan stok tersedia.\n";
                    }
                    system("pause");
                } else {
                    std::cout << "Untuk membeli lightcone, silahkan gunakan tampilan nama, harga, atau stok.\n";
                    system("pause");
                }
                break;
            case 'b': case 'B':
                exitLightconeShop = true;
                break;
            default:
                std::cout << "Pilihan tidak valid!\n";
                system("pause");
        }
    }
}

// ====== Circular Linked List: Cycling Karakter ======
struct CircleNode {
    string character;
    CircleNode* next;
    CircleNode(string character) : character(character), next(nullptr) {}
};

class CharacterCycle {
    CircleNode* head;
public:
    CharacterCycle(vector<string>& characters) {
        head = new CircleNode(characters[0]);
        CircleNode* temp = head;
        for (int i = 1; i < characters.size(); i++) {
            temp->next = new CircleNode(characters[i]);
            temp = temp->next;
        }
        temp->next = head;
    }

};

string cleanName(const string& rawName) {
    string name = rawName;

    // 1) Potong tag " S<digit>" hanya jika ada di akhir string
    regex superimposeRegex(R"(^(.*) S[1-5]$)");
    smatch match;

    if (regex_match(name, match, superimposeRegex)) {
        name = match[1];
    }

    // 2) Potong "(Starglitter ...)"
    size_t pos = name.find(" (Starglitter");
    if (pos != string::npos) {
        name = name.substr(0, pos);
    }

    // 3) Trim whitespace
    name.erase(name.find_last_not_of(" \n\r\t") + 1);
    return name;
}

bool isLightcone(const string& name) {
    string cleaned = cleanName(name);

    return find(lightcones.begin(), lightcones.end(), cleaned) != lightcones.end() ||
           find(standardLightcones.begin(), standardLightcones.end(), cleaned) != standardLightcones.end() ||
           find(threeStarLightcones.begin(), threeStarLightcones.end(), cleaned) != threeStarLightcones.end() ||
           cleaned == rateUpLightcones; // Tambahan: Cek rate-up lightcone
}
// Gacha History Linked List
struct GachaNode {
    string character;
    bool isLightcone;
    GachaNode* next;

    GachaNode(const string& rawName)
        : character(rawName), isLightcone(::isLightcone(rawName)), next(nullptr) {}
};


class GachaHistory {
    GachaNode* head;

    void paginateDisplay(
    const vector<string>& characterList,
    const vector<string>& lightconeList,
    bool showCharacter,
    const string& title,
    bool& backToMenu,
    bool& toggleType
    ) {
    const vector<string>& items = showCharacter ? characterList : lightconeList;

    if (items.empty()) {
        cout << "\n(Tidak ada " << title << " yang ditarik.)\n";
        backToMenu = true;
        return;
    }

    int page = 0;
    int pageSize = 10;
    int totalPages = (items.size() + pageSize - 1) / pageSize;

    while (true) {
        system("cls"); // atau system("clear") jika Linux/Mac
        cout << "\n--- " << title << " Pulls (Page " << page + 1 << " of " << totalPages << ") ---\n";
        int start = page * pageSize;
        int end = min(start + pageSize, (int)items.size());
        for (int i = start; i < end; ++i) {
            cout << i + 1 << ". " << items[i] << "\n";
        }

        cout << "\nNavigasi:\n";
        cout << "[n] Next  [p] Previous  [f] First  [l] Last  [s] Search  [c] Change to " 
             << (title == "Character" ? "Lightcone" : "Character") << "  [x] Exit to Menu\n";
        cout << "Pilihan: ";
        char cmd = getCommand();
        cin.ignore();

        if (cmd == 'n' && page < totalPages - 1) page++;
        else if (cmd == 'p' && page > 0) page--;
        else if (cmd == 'f') page = 0;
        else if (cmd == 'l') page = totalPages - 1;
        else if (cmd == 's') {
            string target;
            cout << "Masukkan nama karakter (case sensitive): ";
            getline(cin, target);

            const vector<string>& currentList = showCharacter ? characterList : lightconeList;
            int jumlah = count(currentList.begin(), currentList.end(), target);

            if (jumlah > 0)
                cout << "\n" << target << " ditemukan sebanyak " << jumlah << " kali.\n";
            else
                cout << "\n" << target << " tidak ditemukan dalam histori.\n";

            system("pause");
        }
        else if (cmd == 'x') { backToMenu = true; break; }
        else if (cmd == 'c') { toggleType = true; break; }
        else cout << "Pilihan tidak valid.\n";
    }
}


public:
    GachaHistory() : head(nullptr) {}

    void addHistory(string name) {
        GachaNode* newNode = new GachaNode(name);
        if (!head) head = newNode;
        else {
            GachaNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    void viewHistory() {
    if (!head) {
        cout << "\n=== Gacha History ===\n";
        cout << "Belum ada histori gacha.\n";
        return;
    }

    vector<string> characterList, lightconeList;
    GachaNode* temp = head;
    while (temp) {
        string name = temp->character;
        if (temp->isLightcone) {
            lightconeList.push_back(name);
        } else {
            characterList.push_back(name);
        }
        temp = temp->next;
    }

    bool showCharacter = true;
        while (true) {
            bool backToMenu = false;
            bool toggleType = false;

            if (showCharacter)
                paginateDisplay(characterList, lightconeList, showCharacter, "Character", backToMenu, toggleType);
            else
                paginateDisplay(characterList, lightconeList, showCharacter, "Lightcone", backToMenu, toggleType);

            if (backToMenu) break;
            if (toggleType) showCharacter = !showCharacter;
        }
    }


    ~GachaHistory() {
        while (head) {
            GachaNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
} history;



// Stack
struct StackNode {
    string name;
    StackNode* next;
    StackNode(string n) : name(n), next(nullptr) {}
};

class RecentCharacterStack {
    StackNode* top;
    int size;
    const int LIMIT = 10;

public:
    RecentCharacterStack() : top(nullptr), size(0) {}

    void push(const string& name) {
        if (size == LIMIT) {
            StackNode* curr = top;
            StackNode* prev = nullptr;
            while (curr->next) {
                prev = curr;
                curr = curr->next;
            }
            delete curr;
            if (prev) prev->next = nullptr;
            else top = nullptr;
            size--;
        }
        StackNode* node = new StackNode(name);
        node->next = top;
        top = node;
        size++;
    }

    void display() const {
        StackNode* curr = top;
        vector<string> names;

        cout << "\n== Recent Characters ===\n";
        while (curr) {
            cout << curr->name << "\n";
            names.push_back(curr->name);
            curr = curr->next;
        }

        // Lambda: cek apakah nama mengandung Eidolon (E<number>) di akhir
        auto hasEidolon = [](const string& name) {
            regex eidolonPattern(R"(.*\sE[1-6]$)");
            return regex_match(name, eidolonPattern);
        };

        int count = count_if(names.begin(), names.end(), hasEidolon);
        cout << "(Total karakter dengan Eidolon: " << count << ")\n";
    }

    ~RecentCharacterStack() {
        while (top) {
            StackNode* temp = top;
            top = top->next;
            delete temp;
        }
    }
} recentCharacters;

// Queue
struct QueueNode {
    string name;
    QueueNode* next;
    QueueNode(string n) : name(n), next(nullptr) {}
};

class CharacterQueue {
    QueueNode* front;
    QueueNode* rear;

public:
    CharacterQueue() : front(nullptr), rear(nullptr) {}

    void enqueue(const string& name) {
        QueueNode* node = new QueueNode(name);
        if (!rear) {
            front = rear = node;
        } else {
            rear->next = node;
            rear = node;
        }
    }

    void display() const {
        QueueNode* curr = front;
        cout << "Character Queue:\n";
        while (curr) {
            cout << curr->name << "\n";
            curr = curr->next;
        }
    }

    ~CharacterQueue() {
        while (front) {
            QueueNode* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
    }
} characterQueue;

// Tree
struct TreeNode {
    string character;
    TreeNode* left;
    TreeNode* right;
    TreeNode(string name) : character(name), left(nullptr), right(nullptr) {}
};

// Utility: Levenshtein Distance
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    int m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (tolower(s1[i - 1]) == tolower(s2[j - 1]))
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

// ====== BST: Search Everything ======
class CharacterTree {
    TreeNode* root;

    TreeNode* insert(TreeNode* node, string name) {
        if (!node) return new TreeNode(name);
        if (name < node->character) node->left = insert(node->left, name);
        else node->right = insert(node->right, name);
        return node;
    }

    void inorder(TreeNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << node->character << " ";
        inorder(node->right);
    }

    void inorderLabeled(TreeNode* node) {
        if (!node) return;
        inorderLabeled(node->left);
        cout << "[Entry: " << node->character << "]\n";
        inorderLabeled(node->right);
    }

    void collectByAlphabet(TreeNode* node, map<char, vector<string>>& groups) {
        if (!node) return;
        collectByAlphabet(node->left, groups);
        char initial = toupper(node->character[0]);
        groups[initial].push_back(node->character);
        collectByAlphabet(node->right, groups);
    }

    void searchInTree(TreeNode* node, const string& keyword, vector<string>& result, bool fuzzy = false, int maxDistance = 2) {
        if (!node) return;
        searchInTree(node->left, keyword, result, fuzzy, maxDistance);
        string lowerChar = toLower(node->character);
        if (!fuzzy) {
            if (lowerChar.find(keyword) != string::npos)
                result.push_back(node->character);
        } else {
            // Fuzzy: gunakan Levenshtein Distance
            if (levenshteinDistance(lowerChar, keyword) <= maxDistance)
                result.push_back(node->character);
        }
        searchInTree(node->right, keyword, result, fuzzy, maxDistance);
    }

    string toLower(string str) {
        for (char& c : str) c = tolower(c);
        return str;
    }

public:
    CharacterTree() : root(nullptr) {}
    void addCharacter(string name) {
        root = insert(root, name);
    }
    void viewInorder() {
        cout << "\n=== Character Tree ===\n";
        cout << "Diurutkan Berdasarkan Abjad:\n";
        if (!root) {
            cout << "Tree kosong.\n";
            return;
        }
        inorder(root);
        cout << endl;
    }
    void viewWithLabel() {
        cout << "\n=== Character Tree (Dengan Label) ===\n";
        if (!root) {
            cout << "Tree kosong.\n";
            return;
        }
        inorderLabeled(root);
        cout << endl;
    }
    void viewGroupedByAlphabet() {
        cout << "\n=== Tree Diurut Berdasarkan Awalan Huruf ===\n";
        map<char, vector<string>> groups;
        collectByAlphabet(root, groups);
        
        // Menggunakan iterator eksplisit untuk traversal map
        for (map<char, vector<string>>::iterator it = groups.begin(); it != groups.end(); ++it) {
            char c = it->first;
            vector<string>& vec = it->second;
            
            // Menghilangkan duplikat dan entri dengan format gacha
            set<string> uniqueBaseNames;
            vector<string> cleanedEntries;
            
            for (const auto& entry : vec) {
                // Extract base name (remove E1, E2, Starglitter, etc.)
                string baseName = entry;
                size_t spacePos = entry.find(' ');
                if (spacePos != string::npos) {
                    baseName = entry.substr(0, spacePos);
                }
                
                // Only add if this base name hasn't been seen before
                if (uniqueBaseNames.find(baseName) == uniqueBaseNames.end()) {
                    uniqueBaseNames.insert(baseName);
                    cleanedEntries.push_back(baseName);
                }
            }
            
            // Sort the cleaned entries
            if (!cleanedEntries.empty()) {
                sort(cleanedEntries.begin(), cleanedEntries.end());
            }
            
            cout << "[" << c << "]\n";
            // Display only unique base names
            for (const auto& name : cleanedEntries) {
                cout << " - " << name << "\n";
            }
        }
    }
    void searchSubstring(string keyword, bool fuzzy = false, int maxDistance = 2) {
        keyword = toLower(keyword);
        vector<string> found;
        clock_t start = clock();
        searchInTree(root, keyword, found, fuzzy, maxDistance);
        clock_t end = clock();
        double elapsed = double(end - start) / CLOCKS_PER_SEC;
        if (found.empty()) {
            cout << "Tidak ditemukan hasil pencarian untuk \"" << keyword << "\"\n";
        } else {
            cout << "Hasil pencarian untuk \"" << keyword << "\" (" << found.size() << " hasil, " << elapsed << " detik):\n";
            for (const auto& item : found) {
                cout << "- " << item << "\n";
            }
        }
    }
};

CharacterTree characterTree;
AVLCharacterTree avlCharacterTree;

// Deklarasi fungsi-fungsi
void saveGame();
void loadGame();
void manageTeam();
void displayTeam();
void battle(vector<string>& selectedTeam);
void shop();
void displayStarglitter();
void showBattleHistoryAndPerformance();
void showAVLMenu();

// Library (fixed)
struct CharacterLibrary {
    vector<string> characters;
    vector<string> lightcones;

    CharacterLibrary() {
        vector<string> fiveStarCharacters = {
            "Bronya", "Clara", "Gepard", "Himeko", "Seele", "Yanqing",
            "Bailu", "Welt", "Fu Xuan", "Sparkle", "Silver Wolf",
            "Black Swan", "Luocha"
        };

        vector<string> fourStarCharacters = {
            "Arlan", "Asta", "Dan Heng", "Hook", "March 7th", "Natasha",
            "Pela", "Sampo", "Serval", "Tingyun"
        };

        characters.insert(characters.end(), fiveStarCharacters.begin(), fiveStarCharacters.end());
        characters.insert(characters.end(), fourStarCharacters.begin(), fourStarCharacters.end());
        characters.push_back("Castorice");

        vector<string> lc5 = {
            "Before Dawn", "Night on the Milky Way",
            "Make Farewell More Beautiful", "Incessant Rain", "Earthly Escapade",
            "She Already Shut Her Eyes", "Reforged Remembrance", "Echoes of the Coffin"
        };

        vector<string> lcStd = {
            "Sleep Like the Dead", "The Battle Isn't Over", "Moment of Victory"
        };

        vector<string> lc3 = {
            "Darting Arrow", "Meshing Cogs", "Chorus", "So Adversarial", "Passkey",
            "Loop", "Multiplication", "Collapsing Sky", "Shattered Home", "Void"
        };

        lightcones.insert(lightcones.end(), lc5.begin(), lc5.end());
        lightcones.insert(lightcones.end(), lcStd.begin(), lcStd.end());
        lightcones.insert(lightcones.end(), lc3.begin(), lc3.end());
    }

    void loadToTree(CharacterTree& tree) {
        // Clear any existing content in the tree first
        tree = CharacterTree(); // Reset to a new empty tree
        
        // Add all characters and lightcones from the clean library
        for (const auto& ch : characters) tree.addCharacter(ch);
        for (const auto& lc : lightcones) tree.addCharacter(lc);
    }
    
    void loadToAVLTree(AVLCharacterTree& tree) {
        for (const auto& ch : characters) tree.addCharacter(ch);
        for (const auto& lc : lightcones) tree.addCharacter(lc);
    }
};

// Gacha
// Fungsi untuk mengecek pity counter dan menangani exception
void exampleCheckPity(int pity) {
    try {
        if (pity > 1000) throw overflow_error("Pity counter terlalu tinggi! Mungkin terjadi bug atau manipulasi data.");
    } catch (const overflow_error& e) {
        cerr << "Warning: " << e.what() << endl;
    }
}

pair<string, string> performBannerGacha(int& pity5, int& pity4, BannerType banner) {
    int roll = rand() % 100 + 1;
    pity5++;
    pity4++;
    
    // Cek pity counter dengan exception handling
    exampleCheckPity(pity5);

    // 5★ pity / roll
    if (pity5 >= 90 || roll <= 1) {
        pity5 = 0;
        string result;

        if (banner == RATE_UP) {
            bool win5050 = rand() % 2 == 0;
            result = win5050 ? rateUpCharacter
                             : fiveStarCharacters[rand() % fiveStarCharacters.size()];
        }
        else if (banner == STANDARD) {
            result = fiveStarCharacters[rand() % fiveStarCharacters.size()];
        }
        else { // LIGHTCONE banner
            bool winRateUp;
            if (guaranteedLightconeNext) {
                winRateUp = true;
            } else {
                winRateUp = (rand() % 2 == 0);
            }

            if (winRateUp) {
                result = rateUpLightcones;
                guaranteedLightconeNext = false;
            } else {
                result = standardLightcones[rand() % standardLightcones.size()];
                guaranteedLightconeNext = true;
            }
        }

        // now apply superimposition / starglitter logic...
        if (isLightcone(result)) {
            if (++lightconeSuperimposition[result] > 6) {
                lightconeSuperimposition[result]--;
                starglitter += 15;
                result += " (Starglitter +15)";
            } else if (lightconeSuperimposition[result] > 1) {
                result += " S" + to_string(lightconeSuperimposition[result]);
            }
        } else {
            // character logic unchanged
            if (++ownedCharacters[result] > 6) {
                ownedCharacters[result]--;
                starglitter += 40;
                result += " (Starglitter +40)";
            } else if (ownedCharacters[result] > 1) {
                result += " E" + to_string(ownedCharacters[result] - 1);
            }
        }

        return {result, "5-star"};
    }
    
    if (pity4 >= 10 || roll <= 10) {
        pity4 = 0;
        string result = fourStarCharacters[rand() % fourStarCharacters.size()];
        if (isLightcone(result)) {
            if (++lightconeSuperimposition[result] > 5) {
                lightconeSuperimposition[result]--;
                starglitter += 15;
                result += " (Starglitter +15)";
            } else if (lightconeSuperimposition[result] > 1) {
                result += " S" + to_string(lightconeSuperimposition[result]);
            }
        } else {
            if (++ownedCharacters[result] > 6) {
                ownedCharacters[result]--;
                starglitter += 40;
                result += " (Starglitter +40)";
            } else if (ownedCharacters[result] > 1) {
                result += " E" + to_string(ownedCharacters[result] - 1);
            }
        }
        
        return {result, "4-star"};
    }

    string result = threeStarLightcones[rand() % threeStarLightcones.size()];
    if (++lightconeSuperimposition[result] > 5) {
        lightconeSuperimposition[result]--;
        starglitter += 15;
        result += " (Starglitter +15)";
    } else if (lightconeSuperimposition[result] > 1) {
        result += " S" + to_string(lightconeSuperimposition[result]);
    }
    return {result, "3-star"};
    
}

void selectBanner() {
    cout << "\n== Pilih Banner ==\n";
    cout << "1. Rate-Up Banner (Castorice)\n";
    cout << "2. Standard Banner\n";
    cout << "3. Rate-Up Lightcone Banner (Castorice)\n";
    int choice;
    choice = getChoice1to3();       
    switch (choice) {
        case 1: currentBanner = RATE_UP; break;
        case 2: currentBanner = STANDARD; break;
        case 3: currentBanner = LIGHTCONE; break;
        default: cout << "Pilihan tidak valid.\n";
    }
}

void manageTeam() {
    cout << "\n== Team Builder ==\n";
    if (ownedCharacters.empty()) {
        cout << "Kamu belum punya karakter.\n";
        return;
    }

    unordered_map<string, int> uniqueCharacters;
    for (auto& [name, copies] : ownedCharacters) {
        if (!uniqueCharacters.count(name) || copies > uniqueCharacters[name])
            uniqueCharacters[name] = copies;
    }

    vector<pair<string, int>> sortedCharacters(uniqueCharacters.begin(), uniqueCharacters.end());

    sort(sortedCharacters.begin(), sortedCharacters.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.first < b.first;
        });

    vector<string> list;
    int i = 1;
    for (auto& [name, copies] : sortedCharacters) {
        int eidLevel = max(0, copies - 1);
        string fullName = eidLevel > 0
            ? name + " E" + (eidLevel > 6 ? "6" : to_string(eidLevel))
            : name;
        list.push_back(fullName);
        cout << i++ << ". " << fullName << endl;
    }

    cout << "\nMasukkan nomor karakter untuk tim (maksimal 4, pisah dengan spasi): ";
    team.clear();
    vector<int> picks = getFourValidIndexes(list.size());
    for (int i : picks) {
        team.push_back(list[i - 1]);  
    }
}


void displayTeam() {
    if (team.empty()) {
        cout << "\n== Tim Saat Ini ==\nBelum ada tim.\n";
        return;
    }

    cout << "\n== Tim Saat Ini ==\n";
    for (int i = 0; i < team.size(); ++i) {
        cout << i + 1 << ". " << team[i];
        if (equippedLightcones.count(team[i])) {
            cout << " (Lightcone: " << equippedLightcones[team[i]] << ")";
        }
        cout << "\n";
    }

    while (true) {
        cout << "\n1. Lihat detail karakter\n2. Kembali ke menu\n";
        int opsi;
        opsi = getChoice1to2();
        if (opsi == 2) break;
        if (opsi == 1) {
            cout << "\nLihat detail siapa? 1-" << team.size() << "\n";
            int idx;
            idx = getChoice1to4();  
            if (idx < 1 || idx > team.size()) {
                cout << "Pilihan tidak valid.\n";
                continue;
            }

            // Ambil nama penuh dan baseName-nya
            string nama = team[idx - 1];
            string baseName = getBaseName(nama);

            // Buat karakter
            Character ch = createCharacter(baseName);
            ch.name = nama;

            int eidolonLevel = getEidolonLevel(nama);
            ch.type = getCharacterType(baseName);
            applyEidolonStatBoosts(ch, eidolonLevel);

            // Pasang Lightcone jika ada
            if (equippedLightcones.count(nama)) {
                Lightcone* eq = makeEquippedLightcone(equippedLightcones[nama]);
                ch.equippedLightcone = eq;
            }

            // Tampilkan detail dengan semua boost
            displayCharacterDetails(ch);

            // Opsi ganti / hapus lightcone
            cout << "\n1. Ganti/tambahkan lightcone\n2. Unequip lightcone\n3. Kembali\n";
            int pilihan;
            pilihan = getChoice1to3();  
            if (pilihan == 1) {
                cout << "\n== Daftar Lightcone yang Dimiliki ==\n";
                int num = 1;
                vector<string> allLightcones;

                // Hanya lightcone yang dimiliki (superimposition >= 1)
                for (const auto& pair : lightconeSuperimposition) {
                    if (pair.second > 0) {
                        allLightcones.push_back(pair.first);
                    }
                }

                if (allLightcones.empty()) {
                    cout << "Kamu tidak memiliki lightcone.\n";
                } else {
                    // Tampilkan daftar dengan superimposition level
                    for (auto& lc : allLightcones) {
                        string lcDisplay = lc;
                        int sup = lightconeSuperimposition[lc];
                        if (sup > 1) lcDisplay += " S" + to_string(sup);
                        cout << num++ << ". " << lcDisplay << "\n";
                    }

                    cout << "\nPilih lightcone untuk ditambahkan: ";
                    int lcChoice;
                    cin >> lcChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (lcChoice >= 1 && lcChoice <= allLightcones.size()) {
                        equippedLightcones[nama] = allLightcones[lcChoice - 1];
                        cout << "Lightcone berhasil dipasang!\n";
                    } else {
                        cout << "Pilihan tidak valid.\n";
                    }
                }
            } else if (pilihan == 2) {
                equippedLightcones.erase(nama);
                cout << "Lightcone telah dihapus.\n";
            }

            delete ch.equippedLightcone;
        }
    }
}


void displayStarglitter() {
    cout << "\nStarglitter: " << starglitter << "\n";
}

void saveGame() {
    try {
        ofstream out("save_data.txt");
        if (!out) throw runtime_error("Gagal membuka file untuk menyimpan.");

        // Currency
        out << starglitter << "\n";

        // Pity counters
        out << pityCounter5RateUp << " " << pityCounter4RateUp << "\n";
        out << pityCounter5Standard << " " << pityCounter4Standard << "\n";
        out << pityCounter5Lightcone << " " << pityCounter4Lightcone << "\n";

        // Team
        out << team.size() << "\n";
        for (const auto& ch : team) {
            out << ch << "\n";
        }

        // Owned characters
        out << ownedCharacters.size() << "\n";
        for (const auto& [name, eid] : ownedCharacters) {
            out << name << "|" << eid << "\n";
        }

        // Equipped lightcones
        out << equippedLightcones.size() << "\n";
        for (const auto& [nama, lc] : equippedLightcones) {
            out << nama << "|" << lc << "\n";
        }

        // Superimposition levels
        out << lightconeSuperimposition.size() << "\n";
        for (const auto& [lcName, Slevel] : lightconeSuperimposition) {
            out << lcName << "|" << Slevel << "\n";  // Changed to use | delimiter for consistency
        }

        // Battle history
        out << allBattles.size() << "\n";
        for (size_t i = 0; i < allBattles.size(); ++i) {
            out << bossNames[i] << "\n";
            int nAksi = allBattles[i].totalActions;
            out << nAksi << "\n";
            BattleAction* act = allBattles[i].head;
            while (act) {
                out << act->actor << "\n" << act->target << "\n"
                    << act->action << "\n" << act->damage << "\n";
                act = act->next;
            }
        }

        out.close();
        cout << "Game berhasil disimpan.\n";
    } catch (const exception& e) {
        cerr << "Terjadi kesalahan saat menyimpan: " << e.what() << endl;
    }
}

void loadGame() {
    try {
        ifstream in("save_data.txt");
        if (!in) throw runtime_error("File save tidak ditemukan.");

        // Clear all data
        team.clear();
        equippedLightcones.clear();
        allBattles.clear();
        bossNames.clear();
        ownedCharacters.clear();
        lightconeSuperimposition.clear();

        // Read currency
        if (!(in >> starglitter)) {
            throw runtime_error("Format file rusak (starglitter).");
        }

        // Read pity counters
        if (!(in >> pityCounter5RateUp >> pityCounter4RateUp)) {
            throw runtime_error("Format file rusak (pity rate-up).");
        }
        if (!(in >> pityCounter5Standard >> pityCounter4Standard)) {
            throw runtime_error("Format file rusak (pity standard).");
        }
        if (!(in >> pityCounter5Lightcone >> pityCounter4Lightcone)) {
            throw runtime_error("Format file rusak (pity lightcone).");
        }

        // Read team
        int teamSize;
        if (!(in >> teamSize) || teamSize < 0) {
            throw runtime_error("Format file rusak (team size).");
        }
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < teamSize; ++i) {
            string member;
            if (!getline(in, member)) {
                throw runtime_error("Format file rusak (team member " + to_string(i) + ").");
            }
            team.push_back(member);
        }

        // Read owned characters
        int ownedSize;
        if (!(in >> ownedSize) || ownedSize < 0) {
            throw runtime_error("Format file rusak (owned characters size).");
        }
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < ownedSize; ++i) {
            string line;
            if (!getline(in, line)) {
                throw runtime_error("Format file rusak (tidak bisa membaca owned character " + to_string(i) + ").");
            }
            
            if (line.empty()) {
                throw runtime_error("Format file rusak (owned character entry " + to_string(i) + " kosong).");
            }
            
            size_t delim = line.find('|');
            if (delim == string::npos) {
                throw runtime_error("Format file rusak (delimiter '|' tidak ditemukan pada owned character " + to_string(i) + ": '" + line + "').");
            }
            if (delim == 0 || delim == line.length() - 1) {
                throw runtime_error("Format file rusak (delimiter '|' di posisi tidak valid pada owned character " + to_string(i) + ": '" + line + "').");
            }
            
            string name = line.substr(0, delim);
            string eidStr = line.substr(delim + 1);
            
            try {
                int eid = stoi(eidStr);
                ownedCharacters[name] = eid;
            } catch (const invalid_argument&) {
                throw runtime_error("Format file rusak (eid tidak valid untuk character '" + name + "': '" + eidStr + "').");
            } catch (const out_of_range&) {
                throw runtime_error("Format file rusak (eid terlalu besar untuk character '" + name + "': '" + eidStr + "').");
            }
        }

        // Read equipped lightcones
        int eqSize;
        if (!(in >> eqSize) || eqSize < 0) {
            throw runtime_error("Format file rusak (equipped lightcones size).");
        }
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < eqSize; ++i) {
            string line;
            if (!getline(in, line)) {
                throw runtime_error("Format file rusak (tidak bisa membaca lightcone entry " + to_string(i) + ").");
            }
            
            if (line.empty()) {
                throw runtime_error("Format file rusak (lightcone entry " + to_string(i) + " kosong).");
            }
            
            size_t delim = line.find('|');
            if (delim == string::npos) {
                throw runtime_error("Format file rusak (delimiter '|' tidak ditemukan pada entry " + to_string(i) + ": '" + line + "').");
            }
            if (delim == 0 || delim == line.length() - 1) {
                throw runtime_error("Format file rusak (delimiter '|' di posisi tidak valid pada entry " + to_string(i) + ": '" + line + "').");
            }
            
            string nama = line.substr(0, delim);
            string lc = line.substr(delim + 1);
            equippedLightcones[nama] = lc;
        }

        // Read lightcone superimposition
        int supSize;
        if (!(in >> supSize) || supSize < 0) {
            throw runtime_error("Format file rusak (superimposition size).");
        }
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < supSize; ++i) {
            string line;
            if (!getline(in, line)) {
                throw runtime_error("Format file rusak (superimposition entry " + to_string(i) + ").");
            }
            
            if (line.empty()) {
                throw runtime_error("Format file rusak (superimposition entry " + to_string(i) + " kosong).");
            }
            
            size_t delim = line.find('|');
            if (delim == string::npos) {
                throw runtime_error("Format file rusak (delimiter '|' tidak ditemukan pada superimposition " + to_string(i) + ").");
            }
            
            string lcName = line.substr(0, delim);
            string slevelStr = line.substr(delim + 1);
            
            try {
                int slevel = stoi(slevelStr);
                lightconeSuperimposition[lcName] = slevel;
            } catch (const invalid_argument&) {
                throw runtime_error("Format file rusak (superimposition level tidak valid: '" + slevelStr + "').");
            }
        }

        // Read battle history
        int nBattles;
        if (!(in >> nBattles) || nBattles < 0) {
            throw runtime_error("Format file rusak (battle count).");
        }
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < nBattles; ++i) {
            string bossName;
            if (!getline(in, bossName)) {
                throw runtime_error("Format file rusak (boss name " + to_string(i) + ").");
            }
            bossNames.push_back(bossName);

            BattleHistory history;
            history.bossName = bossName;

            int nActions;
            if (!(in >> nActions) || nActions < 0) {
                throw runtime_error("Format file rusak (action count for battle " + to_string(i) + ").");
            }
            in.ignore(numeric_limits<streamsize>::max(), '\n');

            for (int j = 0; j < nActions; ++j) {
                string actor, target, action;
                int damage;
                
                if (!getline(in, actor) || !getline(in, target) || !getline(in, action)) {
                    throw runtime_error("Format file rusak (action data " + to_string(j) + " in battle " + to_string(i) + ").");
                }
                
                if (!(in >> damage)) {
                    throw runtime_error("Format file rusak (damage value " + to_string(j) + " in battle " + to_string(i) + ").");
                }
                in.ignore(numeric_limits<streamsize>::max(), '\n');

                history.addAction(actor, target, action, damage);
            }

            allBattles.push_back(history);
        }

        cout << "Game berhasil dimuat.\n";
        cout << "Data yang dimuat:\n";
        cout << "- Starglitter: " << starglitter << "\n";
        cout << "- Team members: " << team.size() << "\n";
        cout << "- Owned characters: " << ownedCharacters.size() << "\n";
        cout << "- Equipped lightcones: " << equippedLightcones.size() << "\n";
        cout << "- Lightcone superimpositions: " << lightconeSuperimposition.size() << "\n";
        cout << "- Battle histories: " << allBattles.size() << "\n";
        
    } catch (const exception& e) {
        cerr << "Error saat memuat game: " << e.what() << endl;
        // Optionally reset data on error
        team.clear();
        equippedLightcones.clear();
        allBattles.clear();
        bossNames.clear();
        ownedCharacters.clear();
        lightconeSuperimposition.clear();
    }
}

void shop() {
    int shopMenuChoice;

    do {
        system("cls"); // atau clear
        cout << "====== SHOP MENU ======\n";
        cout << "Starglitter Anda: " << starglitter << "\n";
        cout << "1. Beli Karakter (5000 Starglitter)\n";
        cout << "2. Beli Lightcone (5000 Starglitter)\n";
        cout << "3. Kembali ke Main Menu\n";
        cout << "Pilihan: ";
        cin >> shopMenuChoice;

        if (shopMenuChoice == 1 || shopMenuChoice == 2) {
            vector<string>& shopList = (shopMenuChoice == 1) ? shopCharacters : shopLightcones;

            while (true) {
                system("cls");
                cout << (shopMenuChoice == 1 ? "== Karakter Tersedia ==" : "== Lightcone Tersedia ==") << "\n";
                for (int i = 0; i < shopList.size(); ++i) {
                    cout << i + 1 << ". " << shopList[i];
                    if ((shopMenuChoice == 1 && ownedCharacters.count(shopList[i])) ||
                        (shopMenuChoice == 2 && lightconeSuperimposition.count(shopList[i])))
                        cout << " [Sudah dimiliki]";
                    cout << "\n";
                }
                cout << "0. Kembali ke Shop Menu\n";
                cout << "Pilihan: ";
                int beli;
                cin >> beli;

                if (beli == 0) break;
                if (beli < 1 || beli > shopList.size()) {
                    cout << "Pilihan tidak valid.\n";
                    system("pause");
                    continue;
                }

                string selected = shopList[beli - 1];

                if ((shopMenuChoice == 1 && ownedCharacters.count(selected)) ||
                    (shopMenuChoice == 2 && lightconeSuperimposition.count(selected))) {
                    cout << "Item sudah dimiliki!\n";
                    system("pause");
                    continue;
                }

                if (starglitter < 5000) {
                    cout << "Starglitter tidak cukup!\n";
                    system("pause");
                    continue;
                }

                starglitter -= 5000;
                if (shopMenuChoice == 1) {
                    ownedCharacters[selected] = 1;
                    characterTree.addCharacter(selected);
                    cout << "Berhasil membeli karakter: " << selected << "!\n";
                } else {
                    lightconeSuperimposition[selected] = 1;
                    cout << "Berhasil membeli lightcone: " << selected << "!\n";
                }

                system("pause");
            }
        }
    } while (shopMenuChoice != 3);
}


// Overloaded functions for displaying character info
void displayCharacterInfo(const Character& ch) {
    cout << "Nama: " << ch.name << "\n";
    cout << "HP: " << ch.getTotalHP() << ", ATK: " << ch.getTotalATK() << ", DEF: " << ch.getTotalDEF() << "\n";
    cout << "Crit Rate: " << ch.getTotalCritRate() << "%, Crit Damage: " << ch.getTotalCritDamage() << "%\n";
    cout << "Lightcone: tidak ada\n";
}

void displayCharacterInfo(const Character& ch, const Lightcone* lc) {
    cout << "Nama: " << ch.name << "\n";
    cout << "HP: " << ch.getTotalHP() << ", ATK: " << ch.getTotalATK() << ", DEF: " << ch.getTotalDEF() << "\n";
    cout << "Crit Rate: " << ch.getTotalCritRate() << "%, Crit Damage: " << ch.getTotalCritDamage() << "%\n";
    if (lc) {
        cout << "Lightcone: " << lc->name;
        int s = lightconeSuperimposition[lc->name];
        if (s > 1) cout << " S" << s;
        cout << "\n";
    } else {
        cout << "Lightcone: tidak ada\n";
    }
}

template<typename T>
void displayList(const vector<T>& list, const string& title) {
    cout << "\n== " << title << " ==\n";
    for (size_t i = 0; i < list.size(); ++i) {
        cout << i + 1 << ". " << list[i] << "\n";
    }

    // Tambahan lambda expression sederhana
    auto panjangLebihDari10 = [](const T& item) {
        return item.length() > 10;
    };
    int count = count_if(list.begin(), list.end(), panjangLebihDari10);
    cout << "(Total item dengan nama > 10 karakter: " << count << ")\n";
}


void playGachaSound() {
    PlaySound(TEXT("../Assets/Audio/Gacha.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void playCastoriceVideo() {
    system("start \"\" \"../Assets/Animation/Castorice.mp4\"");
}

// Fungsi untuk menampilkan menu AVL Tree dan menggunakan fiturnya
void showAVLMenu() {
    int choice;
    do {
        system("cls");
        cout << "\n=== AVL Tree Menu ===\n";
        cout << "1. Tampilkan karakter dikelompokkan berdasarkan huruf awal\n";
        cout << "2. Tampilkan properti AVL Tree (tinggi dan balance factor)\n";
        cout << "3. Cari karakter dalam AVL Tree\n";
        cout << "4. Kembali ke Menu Utama\n";
        cout << "Pilihan Anda: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                system("cls");
                avlCharacterTree.viewGroupedByAlphabet();
                system("pause");
                break;
                
            case 2:
                system("cls");
                avlCharacterTree.displayTreeProperties();
                system("pause");
                break;
                
            case 3: {
                system("cls");
                cin.ignore();
                string keyword;
                cout << "\nMasukkan nama karakter yang dicari: ";
                getline(cin, keyword);
                
                bool found = avlCharacterTree.findCharacter(keyword);
                if (found) {
                    cout << "\nKarakter '" << keyword << "' ditemukan dalam AVL Tree!\n";
                } else {
                    cout << "\nKarakter '" << keyword << "' tidak ditemukan.\n";
                }
                system("pause");
                break;
            }
                
            case 4:
                return;
                
            default:
                cout << "\nPilihan tidak valid.\n";
                system("pause");
        }
    } while (true);
}

void showBattleHistoryAndPerformance() {
    cout << "\n==============================\n";
    cout << "     Battle History & Performance\n";
    cout << "==============================\n";
    if (allBattles.empty()) {
        cout << "\nBelum ada riwayat pertarungan yang tersimpan.\n";
        system("pause");
        return;
    }
    cout << "\nList Riwayat Pertarungan:\n";
    for (size_t i = 0; i < allBattles.size(); ++i) {
        cout << i+1 << ". [Boss: " << bossNames[i] << "]\n";
    }
    cout << "\nMasukkan ID riwayat battle yang ingin dilihat (1-" << allBattles.size() << ", 0 untuk batal): ";
    int id;
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    if (id < 1 || id > (int)allBattles.size()) return;
    
    bool viewMode = true;    // true = dari awal (BFS), false = dari akhir (DFS)
    bool showGraph = true;   // true = show performance graph, false = hide graph
    bool exitView = false;
    
    while (!exitView) {
        system("cls"); // Clear screen for better UI
        cout << "\n=== Detail Pertarungan vs " << bossNames[id-1] << " ===\n";
        
        // Show battle history based on current view mode
        if (viewMode) {
            allBattles[id-1].showBFS(); // Show chronological view (dari awal)
        } else {
            allBattles[id-1].showDFS(); // Show reverse chronological view (dari akhir)
        }
        
        // Show performance graph if enabled
        if (showGraph) {
            allBattles[id-1].showPerformanceGraph();
        }
        
        // Menu options for toggling view, graph, or exiting
        cout << "\n============= Menu Riwayat =============\n";
        cout << "[t] Toggle Urutan " << (viewMode ? "(Dari awal -> Dari akhir)" : "(Dari akhir -> Dari awal)") << "\n";
        cout << "[g] " << (showGraph ? "Sembunyikan" : "Tampilkan") << " Performance Graph\n";
        cout << "[x] Kembali ke Menu Utama\n";
        cout << "Pilihan: ";
        
        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        
        switch (choice) {
            case 't':
            case 'T':
                viewMode = !viewMode; // Toggle between dari awal and dari akhir
                break;
            case 'g':
            case 'G':
                showGraph = !showGraph; // Toggle performance graph visibility
                break;
            case 'x':
            case 'X':
                exitView = true; // Exit the viewing loop
                break;
            default:
                cout << "Pilihan tidak valid!\n";
                system("pause");
        }
    }
}

int getValidatedChoice(int min, int max) {
    string input;
    int choice;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek jika input berisi angka saja
        bool valid = all_of(input.begin(), input.end(), ::isdigit);
        if (!valid) {
            cout << "Input hanya boleh angka antara " << min << " sampai " << max << ".\n";
            continue;
        }

        choice = stoi(input);

        if (choice < min || choice > max) {
            cout << "Pilihan hanya antara " << min << " sampai " << max << ".\n";
        } else {
            break;
        }
    }

    return choice;
}

int getChoice1to2() {
    string input;
    int choice;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek apakah seluruh karakter adalah digit
        bool isValid = all_of(input.begin(), input.end(), ::isdigit);
        if (!isValid) {
            cout << "Input hanya boleh angka 1 sampai 2.\n";
            continue;
        }

        // Ubah ke integer dan cek rentangnya
        choice = stoi(input);
        if (choice < 1 || choice > 2) {
            cout << "Hanya angka 1 atau 2 yang diperbolehkan.\n";
            continue;
        }

        break;
    }

    return choice;
}

int getChoice0to3() {
    string input;
    int choice;

    while (true) {
        getline(cin, input);

        if (input.empty()) {
            cout << "Input tidak boleh kosong.\n";
            continue;
        }

        // Cek apakah semua karakter digit
        bool isDigit = all_of(input.begin(), input.end(), ::isdigit);
        if (!isDigit) {
            cout << "Input hanya boleh angka 0 sampai 3.\n";
            continue;
        }

        choice = stoi(input);
        if (choice < 0 || choice > 3) {
            cout << "Pilihan hanya 0, 1, 2, atau 3.\n";
            continue;
        }

        return choice;
    }
}

int getChoice1to3() {
    string input;
    int choice;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek apakah seluruh karakter adalah digit
        bool isValid = all_of(input.begin(), input.end(), ::isdigit);
        if (!isValid) {
            cout << "Input hanya boleh angka 1 sampai 3.\n";
            continue;
        }

        // Ubah ke integer dan cek rentangnya
        choice = stoi(input);
        if (choice < 1 || choice > 3) {
            cout << "Hanya angka 1, 2, atau 3 yang diperbolehkan.\n";
            continue;
        }

        break;
    }

    return choice;
}

int getChoice1to4() {
    string input;
    int choice;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek apakah seluruh karakter adalah digit
        bool isValid = all_of(input.begin(), input.end(), ::isdigit);
        if (!isValid) {
            cout << "Input hanya boleh angka 1 sampai 4.\n";
            continue;
        }

        // Ubah ke integer dan cek rentangnya
        choice = stoi(input);
        if (choice < 1 || choice > 4) {
            cout << "Hanya angka 1, 2, 3 atau 4 yang diperbolehkan.\n";
            continue;
        }

        break;
    }

    return choice;
}

char getCharNS() {
    string input;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek hanya satu karakter dan valid
        if (input.size() == 1) {
            char c = tolower(input[0]);
            if (c == 'n' || c == 's') {
                return c;
            }
        }

        cout << "Input hanya boleh 'n' atau 's'. Coba lagi.\n";
    }
}

char getYesOrNo() {
    string input;

    while (true) {
        getline(cin, input);

        if (input.empty()) {
            cout << "Input tidak boleh kosong.\n";
            continue;
        }

        if (input.size() == 1) {
            char c = tolower(input[0]);
            if (c == 'y' || c == 'n') {
                return c;
            }
        }

        cout << "Input hanya boleh 'y' atau 'n'. Coba lagi.\n";
    }
}


char getCommand() {
    string input;

    while (true) {
        getline(cin, input);

        // Cek jika input kosong
        if (input.empty()) {
            cout << "Input tidak boleh kosong. Coba lagi.\n";
            continue;
        }

        // Cek panjang input = 1 dan termasuk huruf valid
        if (input.size() == 1) {
            char c = tolower(input[0]);
            if (c == 'n' || c == 'p' || c == 'f' || c == 'l' || c == 's' || c == 'c' || c == 'x') {
                return c;
            }
        }

        cout << "Input hanya boleh huruf n, p, f, l, s, c, atau x. Coba lagi.\n";
    }
}

vector<int> getFourValidIndexes(int maxIndex) {
    vector<int> result;
    string line;
    
    while (true) {
        getline(cin, line);
        stringstream ss(line);
        int num;
        result.clear();
        bool invalid = false;

        while (ss >> num) {
            if (num < 1 || num > maxIndex) {
                cout << "Nomor " << num << " tidak valid (harus antara 1 dan " << maxIndex << ").\n";
                invalid = true;
                break;
            }
            result.push_back(num);
        }

        if (invalid) continue;

        if (result.size() != 4) {
            cout << "Harus memasukkan tepat 4 angka yang dipisahkan oleh spasi.\n";
            continue;
        }

        break;
    }

    return result;
}


int main() {
    srand(static_cast<unsigned int>(time(0)));
    CharacterCycle cycle(fiveStarCharacters);
    cout << "\nDaftar Karakter Bintang 5 Standar Yang Tersedia:\n";
    cycle.display(10);
    cout << "\nKarakter Bintang 5 Yang Sedang Rate On: Castorice\n";

    int choice;
    do {
        cout << "\n=== Honkai: Star Rail Gacha Simulator ===\n";
        cout << "1. Perform Gacha\n2. View Gacha History\n3. Library\n4. Save Game\n5. Load Game\n6. Build Team\n7. View Team\n8. Battle Mode\n9. Battle History & Performance\n10. Shop\n0. Exit\n";
        cout << "Enter your choice: ";
        choice = getValidatedChoice(0, 11); 

        switch (choice) {
            case 1: {
                selectBanner();
                int subChoice;
                do {
                    cout << "\n== Gacha Menu ==\n";
                    cout << "1. Pull 1x\n2. Pull 10x\n3. Kembali ke Menu Utama\n";
                    cout << "Pilihan Anda: ";
                    subChoice = getChoice1to3();                  
                    cout << "\n";
            
                    switch (subChoice) {
                        case 1: {
                            pair<string, string> result;
                            if (currentBanner == RATE_UP)
                                result = performBannerGacha(pityCounter5RateUp, pityCounter4RateUp, RATE_UP);
                            else if (currentBanner == STANDARD)
                                result = performBannerGacha(pityCounter5Standard, pityCounter4Standard, STANDARD);
                            else
                                result = performBannerGacha(pityCounter5Lightcone, pityCounter4Lightcone, LIGHTCONE);
                        
                            playGachaSound();
                        
                            string baseName = cleanName(result.first);
                            bool lightcone = isLightcone(baseName);
                            int S = lightcone ? lightconeSuperimposition[baseName] : 0;
                            string historyName = lightcone
                                ? (S > 1 ? baseName + " S" + to_string(S) : baseName)
                                : result.first;
                        
                            cout << "You got: " << result.first << " (" << result.second << ")\n";
                            history.addHistory(historyName);
                        
                            if (!lightcone) {
                                recentCharacters.push(historyName);
                                characterQueue.enqueue(historyName);
                                characterTree.addCharacter(historyName);
                        
                                if (baseName == "Castorice") {
                                    playCastoriceVideo();
                                }
                            }
                        
                            displayStarglitter();
                            break;
                        }
                        
                        case 2: {
                            vector<string> hasilGacha;
                            vector<string> baseNames;
                            bool castoriceFoundAfterFirst = false;
                        
                            for (int i = 0; i < 10; ++i) {
                                pair<string, string> result;
                                if (currentBanner == RATE_UP)
                                    result = performBannerGacha(pityCounter5RateUp, pityCounter4RateUp, RATE_UP);
                                else if (currentBanner == STANDARD)
                                    result = performBannerGacha(pityCounter5Standard, pityCounter4Standard, STANDARD);
                                else
                                    result = performBannerGacha(pityCounter5Lightcone, pityCounter4Lightcone, LIGHTCONE);
                        
                                string baseName = cleanName(result.first);
                                bool lightcone = isLightcone(baseName);
                                int S = lightcone ? lightconeSuperimposition[baseName] : 0;
                                string historyName = lightcone
                                    ? (S > 1 ? baseName + " S" + to_string(S) : baseName)
                                    : result.first;
                        
                                hasilGacha.push_back("Pull " + to_string(i + 1) + ": " + result.first + " (" + result.second + ")");
                                baseNames.push_back(baseName);
                                history.addHistory(historyName);
                        
                                if (!lightcone) {
                                    recentCharacters.push(historyName);
                                    characterQueue.enqueue(historyName);
                                    characterTree.addCharacter(historyName);
                                }
                            }
                        
                            cout << "\nMenampilkan hasil gacha satu per satu...\n";
                        
                            for (int i = 0; i < hasilGacha.size(); ++i) {
                                playGachaSound();
                                cout << hasilGacha[i] << "\n";
                        
                                if (baseNames[i] == "Castorice") {
                                    playCastoriceVideo();
                                }
                        
                                if (i < hasilGacha.size() - 1) {
                                    cout << "Tekan [n] untuk next, [s] untuk skip semua: ";
                                    char cmd = getCharNS();
                                    if (cmd == 's' || cmd == 'S') {
                                        playGachaSound();
                        
                                        // Cek apakah masih ada Castorice di sisa pull
                                        for (int j = i + 1; j < baseNames.size(); ++j) {
                                            if (baseNames[j] == "Castorice") {
                                                castoriceFoundAfterFirst = true;
                                                break;
                                            }
                                        }
                        
                                        if (castoriceFoundAfterFirst) {
                                            playCastoriceVideo(); // hanya sekali saat skip
                                        }
                        
                                        for (int j = i + 1; j < hasilGacha.size(); ++j) {
                                            cout << hasilGacha[j] << "\n";
                                        }
                                        break;
                                    }
                                }
                            }
                        
                            displayStarglitter();
                            break;
                        } 
                            
                        case 3:
                            cout << "Kembali ke menu utama...\n";
                            break;
                        default:
                            cout << "Pilihan tidak valid.\n";
                    }
                
                } while (subChoice != 3);
                break;
            }
            
            case 2: {
                int hChoice;
                do {
                    cout << "\n== History Gacha ==\n";
                    cout << "1. View Gacha History\n";
                    cout << "2. View Recent Characters\n";
                    cout << "3. View Character Queue\n";
                    cout << "4. Kembali ke Menu Utama\n";
                    cout << "Pilih opsi: ";
                    hChoice = getChoice1to4();  

                    switch (hChoice) {
                        case 1: history.viewHistory(); break;
                        case 2: recentCharacters.display(); break;
                        case 3: characterQueue.display(); break;
                        case 4: cout << "Kembali ke Menu Utama...\n"; break;
                        default: cout << "Pilihan tidak valid.\n";
                    }

                } while (hChoice != 4);
                break;
            }
            case 3: {
                static bool isLoaded = false;
                if (!isLoaded) {
                    CharacterLibrary lib;
                    lib.loadToTree(characterTree);
                    isLoaded = true;
                }
            
                int sub;
                do {
                    cout << "\n=== Library Menu ===\n";
                    cout << "1. Lihat Isi Tree (Label A-Z)\n";
                    cout << "2. Cari Karakter / Lightcone\n";
                    cout << "3. Kembali ke Menu Utama\n";
                    cout << "Pilihan: ";
                    sub = getChoice1to3();  
                    cin.ignore();
            
                    switch (sub) {
                        case 1: characterTree.viewGroupedByAlphabet(); break;
                        case 2: {
                            string keyword;
                            cout << "Masukkan keyword pencarian: ";
                            getline(cin, keyword);
                            characterTree.searchSubstring(keyword);
                            break;
                        }
                        case 3: cout << "Kembali ke menu utama...\n"; break;
                        default: cout << "Pilihan tidak valid.\n";
                    }
            
                } while (sub != 3);
                break;
            }
                    
            case 4: saveGame(); break;
            case 5: loadGame(); break;
            case 6: manageTeam(); break;
            case 7: displayTeam(); break;
            case 8: {
            if (team.empty()) cout << "Bentuk tim dulu melalui opsi 6.\n";
            else battle(team);
            break;
            }
            case 9:
            showBattleHistoryAndPerformance();
            break;
            case 10:
            showShopMenu();
            break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}