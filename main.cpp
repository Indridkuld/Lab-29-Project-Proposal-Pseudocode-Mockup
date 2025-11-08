/* 
   EcoSim: A simple ecosystem simulation with plants, herbivores, and predators.
*/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <array>
#include <list>

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
void popListN(list<string>& lst, const string& token, int n) {
    for (int i = 0; i < n; ++i) lst.push_back(token);
}

// ==========================================================
// MAIN FUNCTIONS
// ==========================================================

// File Loading function:

bool loadWorld(const string& path, EcoMap& eco) { } 
//   - open file
//   - parse lines for GRID, PARAM, and CELL entries
//   - populate the map accordingly
//   - return true if successful, false otherwise

// Prints global parameters function:
void printGridHeader() { }

// Prints a formatted grid of (P,H,R) counts for each cell
void printCountsTable(const EcoMap& eco, const string& title) {}


// Simulation Step function:
void simStep(EcoMap& eco, int t) { }
//   For each cell in the map:
//     1. Plants grow by growthRate fraction.
//     2. Herbivores feed on plants (remove one plant per herbivore if possible).
//     3. Herbivores reproduce or die based on feeding success.
//     4. Predators hunt herbivores (remove one herbivore per predator if possible).
//     5. Starvation/removal of predators if no prey.
//     6. Predator reproduction.
//     7. Migration

// ==========================================================
// MAIN DRIVER
// ==========================================================
//   1. Declare the map data structure.
//   2. Attempt to load from external file.
//   3. If file missing, seed with a tiny default grid.
//   4. Display initial state.
//   5. Run a time-based loop for at least 25 steps.
//   6. Show snapshots every few steps.
//   7. Print the final state neatly.

int main() {
    //   1. Declare the map data structure.
    EcoMap ecosystem;
}