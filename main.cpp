/* 
   EcoSim: A simple ecosystem simulation with plants, herbivores, and predators.
*/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <array>
#include <list>
#include <iomanip>
#include <algorithm>

using namespace std;

typedef map<string, array<list<string>, 3>> EcoMap; // using typedef for readability, may change to class method if I want to go oop later

// Global Config (should be from file, but hardcoded for now)

int GRID_ROWS = 0, GRID_COLS = 0;

int STEPS = 25; // >= 25

double plantGrowth = 0.25; // plants growth fraction

double herbBirth = 0.10; // chance per fed herbivore to reproduce

double predBirth = 0.05; // chance per successful predator to reproduce

int starvationSteps = 3; // predator starves after these steps

double migrateRate = 0.02; // chance an H/R moves to neighbor

map<string, int> predatorHunger; // tracks steps since last meal (keyed by "cell|index") 


// ==========================================================
// HELPER FUNCTIONS
// ==========================================================

// Generate a key for the map, e.g., "r3c4"
string keyForCell(int r, int c) {
    return "r" + to_string(r) + "c" + to_string(c);
}

// Push 'n' tokens (used to populate the lists)
void popList(list<string>& lst, const string& token, int n) {
    for (int i = 0; i < n; ++i) lst.push_back(token);
}

// ==========================================================
// MAIN FUNCTIONS
// ==========================================================

// File Loading function:

bool loadWorld(const string& path, EcoMap& eco) {  
//   - open file
    ifstream fin;
    fin.open(path.c_str());
    if (!fin.good()) {
        cout << "File not found.\n";
        return false;
    }

    bool sawCell = false;
    string tag;
    while (true) {
            if (!(fin >> tag)) break;                 // checks for end-of-file
            if (!tag.empty() && tag[0] == '#') {      // check for comment line
                string discard;                         
                getline(fin, discard);                // skip remainder
                continue;
            }

            if (tag == "GRID") { // checks for GRID tag
                fin >> GRID_ROWS >> GRID_COLS;
                string discard; getline(fin, discard); // discard rest of line
            }
            else if (tag == "PARAM") { // checks for PARAM tag
                string name; double val; // read name/value pair
                fin >> name >> val;
                if (name == "plantGrowth")          plantGrowth     = val;
                else if (name == "herbBirth")       herbBirth       = val;
                else if (name == "predBirth")       predBirth       = val;
                else if (name == "starvationSteps") starvationSteps = static_cast<int>(val); // needs to be int
                else if (name == "migrateRate")     migrateRate     = val;
                else if (name == "steps")           STEPS           = static_cast<int>(val);
                string discard; getline(fin, discard);
            }
            else if (tag == "CELL") { // checks for CELL tag
                int r, c, P, H, R;
                fin >> r >> c >> P >> H >> R;
                string discard; getline(fin, discard);

                string key = keyForCell(r, c);
                auto& cell = eco[key];
                popList(cell[0], "P", P); 
                popList(cell[1], "H", H);
                popList(cell[2], "R", R);
                sawCell = true;
            }
            else {
                // Unknown tag—skip rest of line to keep in sync
                string discard; getline(fin, discard);
            }
        }

    fin.close();
    return sawCell;
}
// Prints global parameters function:
void printGridHeader() {
    cout << "\n=== Predator Prey Grid Simulation (Beta) ===\n";
    cout << "Grid: " << GRID_ROWS << " x " << GRID_COLS
         << " | Steps=" << STEPS
         << " | plantGrowth=" << plantGrowth
         << " | herbBirth="   << herbBirth
         << " | predBirth="   << predBirth
         << " | starvationSteps=" << starvationSteps
         << "\n";
}

// Prints a formatted grid of (P,H,R) counts for each cell (full ecosystem)
void printGridCount(const EcoMap& eco, const string& title) {
    cout << "\n" << title << "\n";
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            string key = keyForCell(r, c);
            auto it = eco.find(key);
            size_t P=0, H=0, R=0;
            if (it != eco.end()) {
                P = it->second[0].size();
                H = it->second[1].size();
                R = it->second[2].size();
            }
            cout << "(" << setw(2) << P << "," << setw(2) << H << "," << setw(2) << R << ") ";
        }
        cout << "\n";
    }
}
// creates a viewport printout of a section of the grid instead of full grid
void printViewport(const EcoMap& eco, const string& title, int r0, int c0) {
    int h = 5, w = 5;

    cout << "\n" << title << "  [rows " << r0 << "-" << (r0+h-1)
         << ", cols " << c0 << "-" << (c0+w-1) << "]\n";

    if (GRID_ROWS <= 0 || GRID_COLS <= 0) {
        cout << "(grid size not set)\n";
        return;
    }

    // bounds check 
    if (r0 < 0) r0 = 0;
    if (c0 < 0) c0 = 0;
    if (r0 + h > GRID_ROWS) r0 = GRID_ROWS - h;
    if (c0 + w > GRID_COLS) c0 = GRID_COLS - w;
    // print the viewport with nested loops and p, h, r counts
    for (int r = r0; r < r0 + h; ++r) {
        for (int c = c0; c < c0 + w; ++c) {
            string key = keyForCell(r, c);
            auto it = eco.find(key);
            size_t P=0, H=0, R=0;
            if (it != eco.end()) {
                P = it->second[0].size();
                H = it->second[1].size();
                R = it->second[2].size();
            }
            cout << "(" << setw(2) << P << "," << setw(2) << H << "," << setw(2) << R << ") ";
        }
        cout << "\n";
    }
}
// Prints a specific quadrant of the grid using printViewport
void printQuadrant(const EcoMap& eco, int q, const string& title) {
    int r0 = 0, c0 = 0;
    int midRow = GRID_ROWS / 2;
    int midCol = GRID_COLS / 2;

    switch (q) {
        case 1: r0 = 0;        c0 = midCol; break; // Top-Right
        case 2: r0 = midRow;   c0 = 0;      break; // Bottom-Left
        case 3: r0 = midRow;   c0 = midCol; break; // Bottom-Right
        case 0:                 // Top-Left
        default: r0 = 0;       c0 = 0;      break;
    }

    printViewport(eco, title, r0, c0);
}

// Simulation Step function:
void simStep(EcoMap& eco, int /*t*/) { 
    for (auto& kv : eco) {
        auto& cell = kv.second;

        // 1. Plants grow by plantGrowth fraction (at least +1 if P>0)
        int P = static_cast<int>(cell[0].size());
        if (P > 0) {
            int add = static_cast<int>(plantGrowth * P);
            if (add < 1) add = 1;
            popList(cell[0], "P", add);
        }

        // 2. Herbivores feed on plants
        int H_before = static_cast<int>(cell[1].size());
        int plantsAvail = static_cast<int>(cell[0].size());
        int fed = std::min(H_before, plantsAvail);
        for (int i = 0; i < fed; ++i) {
            if (!cell[0].empty()) cell[0].pop_front();
        }

        // 3. Herbivores reproduce or die based on feeding success
        int unfed = H_before - fed;
        int hStarve = unfed / 3; // remove every 3rd unfed herbivore
        while (hStarve-- > 0 && !cell[1].empty()) cell[1].pop_back();
        int hBabies = static_cast<int>(herbBirth * fed);
        popList(cell[1], "H", hBabies);

        // 4. Predators hunt herbivores
        int R_before = static_cast<int>(cell[2].size());
        int herbAvail = static_cast<int>(cell[1].size());
        int eaten = std::min(R_before, herbAvail);
        for (int i = 0; i < eaten; ++i) {
            if (!cell[1].empty()) cell[1].pop_front();
        }

        // 5. Starvation/removal of predators if no prey (every 2nd unfed)
        int unfedPred = R_before - eaten;
        int rStarve = unfedPred / 2;
        while (rStarve-- > 0 && !cell[2].empty()) cell[2].pop_back();

        // 6. Predator reproduction
        int rBabies = static_cast<int>(predBirth * eaten);
        popList(cell[2], "R", rBabies);
    }
}

// Seed a default small grid if no file is found
void seedDefault(EcoMap& eco) {
    GRID_ROWS = 2; GRID_COLS = 2; STEPS = 25;
    eco.clear();

    auto& a = eco[keyForCell(0,0)];
    popList(a[0], "P", 10); popList(a[1], "H", 3); popList(a[2], "R", 1);

    auto& b = eco[keyForCell(0,1)];
    popList(b[0], "P", 6);  popList(b[1], "H", 2); popList(b[2], "R", 0);

    auto& c = eco[keyForCell(1,0)];
    popList(c[0], "P", 8);  popList(c[1], "H", 1); popList(c[2], "R", 1);

    auto& d = eco[keyForCell(1,1)];
    popList(d[0], "P", 12); popList(d[1], "H", 4); popList(d[2], "R", 1);
}

// ==========================================================
// MAIN DRIVER
// ==========================================================

int main() {
    // 1. Declare the map data structure.
    EcoMap ecosystem;

    // 2. Attempt to load from external file.
    string filepath = "ecosim.txt";
    
    // 3. If file missing, seed with a tiny default grid.
    if (!loadWorld(filepath, ecosystem)) {
        cout << "[info] Using default grid.\n";
        seedDefault(ecosystem);
    }

    // 4. Display initial state.
    printGridHeader();
    printGridCount(ecosystem, "Initial State:");

    // 5. Run a time-based loop for at least 25 steps.
    for (int t = 1; t <= STEPS; ++t) {
        simStep(ecosystem, t);
        if (t % 5 == 0) {
            printGridCount(ecosystem, "Snapshot at t=" + to_string(t));
        }
    }

    // 6. Final state
    printGridCount(ecosystem, "Final State (t=" + to_string(STEPS) + ")");
    cout << "\nDone.\n";
    return 0;
}