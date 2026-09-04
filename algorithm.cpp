

#include "algorithm.h"
#include <unordered_set>

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////
//
// Uniform Cost Search Comparator
// C++ Standard Library heap is max by default.
// 
// Returns true when 'a' has a larger g-cpst than 'b', 
// which makes the smallest g-cost rise to the top of the heap.
////////////////////////////////////////////////////////////////////////////////////////////
struct CompareGCost {

    bool operator()(Puzzle* a, Puzzle* b) const {
        return a->getGCost() > b->getGCost();
    }
};

struct CompareFCost {

    bool operator()(Puzzle* a, Puzzle* b) const {

        // Lowest f-cost has highest priority
        if(a->getFCost() != b->getFCost()) {
            return a->getFCost() > b->getFCost();
        }

        // If f-costs tie, prefer the node with LARGER g-cost
        return a->getGCost() < b->getGCost();
    }
};


///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  UC with Strict Expanded List
//
// Q Ordering:  
// Lowest g-cost first
// 
// Required Successor Generation Order:  Up, Right, Down, Left
////////////////////////////////////////////////////////////////////////////////////////////
string uc_explist(string const initialState, string const goalState, int& pathLength, int &numOfStateExpansions, int& maxQLength,
                               float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions){
											 
    string path = "";
    clock_t startTime = clock();

    // --------------------------------------------------------
    // Initialise statistics
    // --------------------------------------------------------
    pathLength = 0;
    numOfStateExpansions = 0;
    maxQLength = 0;
    actualRunningTime = 0.0;

    numOfDeletionsFromMiddleOfHeap=0;
    numOfLocalLoopsAvoided=0;
    numOfAttemptedNodeReExpansions=0;


    // --------------------------------------------------------
    // Q and Strict Expanded List
    // --------------------------------------------------------
    vector<Puzzle*> Q;
    unordered_set<string> expanded;
    CompareGCost compareG;


    // --------------------------------------------------
    // Create initial search node and add it to Q
    // --------------------------------------------------
    Puzzle* initialPuzzle = new Puzzle(initialState, goalState);
    Q.push_back(initialPuzzle);
    make_heap(Q.begin(), Q.end(), compareG);
    maxQLength = 1;


    // --------------------------------------------------
    // Main Uniform Cost Search loop
    // --------------------------------------------------
    while(!Q.empty()) {
        // Remove lowest g-cost node from the heap
        pop_heap(Q.begin(), Q.end(), compareG);
        Puzzle* current = Q.back();
        Q.pop_back();
        string currentState = current->getString();
        

        // --------------------------------------------------
        // Strict Expanded List check
        // --------------------------------------------------
        if(expanded.find(currentState) != expanded.end()) {
            // Node has already been expanded, skip it
            numOfAttemptedNodeReExpansions++;
            delete current;
            continue;
        }
        // This state is now expanded
        expanded.insert(currentState);
        numOfStateExpansions++;


        // --------------------------------------------------
        // Goal test
        // --------------------------------------------------
        if(current->goalMatch()) {
            path = current->getPath();
            pathLength = current->getPathLength();
            delete current;
            break;
        }


        // --------------------------------------------------
        // Add a successor to Q
        // --------------------------------------------------
        auto pushSuccessor = [&](Puzzle* next){
            string nextState = next->getString();

            // --------------------------------------------------
            // 1. Strict Expanded List check
            //
            // If this state has already been expanded, do not put it back into Q.
            // --------------------------------------------------
            if(expanded.find(nextState) != expanded.end()) {
                numOfAttemptedNodeReExpansions++;
                delete next;
                return;
            }

            // --------------------------------------------------
            // 2. Check whether the same state is already in Q
            // --------------------------------------------------
            for(size_t i = 0; i < Q.size(); i++) {
                if(Q[i]->getString() == nextState) {

                    // --------------------------------------------------
                    // A path to this state is already waiting in Q.
                    //
                    // If the new path is shorter, remove the old search node and replace it with the new one.
                    // --------------------------------------------------
                    if(next->getGCost() < Q[i]->getGCost()) {
                        Puzzle* oldNode = Q[i];
                        // Replace the deleted element with the final element in the heap vector.
                        Q[i] = Q.back();
                        Q.pop_back();
                        delete oldNode;


                        // Arbitrary removal breaks the heap ordering, so rebuild the heap.
                        make_heap(Q.begin(), Q.end(), compareG);
                        numOfDeletionsFromMiddleOfHeap++;
                        Q.push_back(next);
                        push_heap(Q.begin(), Q.end(), compareG);
                    } else {
                        delete next;
                    }
                    return;
                }
            }

            // --------------------------------------------------
            // 3. State is not Expande and not already in Q
            // Add it normally
            // --------------------------------------------------
            Q.push_back(next);
            push_heap(Q.begin(), Q.end(), compareG);

            // --------------------------------------------------
            // 4. Update max Q length
            // --------------------------------------------------
            if((int)Q.size() > maxQLength) {
                maxQLength = Q.size();  
            }
        };


        // --------------------------------------------------
        // Required successor generation order: Up, Right, Down, Left
        // --------------------------------------------------
        if(current->canMoveUp()) {
            pushSuccessor(current->moveUp());
        }
        if(current->canMoveRight()) {
            pushSuccessor(current->moveRight());
        }
        if(current->canMoveDown()) {
            pushSuccessor(current->moveDown());
        }
        if(current->canMoveLeft()) {
            pushSuccessor(current->moveLeft());
        }
        delete current;
    }


    // --------------------------------------------------
    // Delete anything still remaining in Q
    // --------------------------------------------------
    for(Puzzle* p : Q) {
        delete p;
    }
    Q.clear();

    // --------------------------------------------------------
    // Record execution time
    // --------------------------------------------------------
    actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);
    return path;
}




///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  A* with the Strict Expanded List
//
// Move Generator:  
//
////////////////////////////////////////////////////////////////////////////////////////////
string aStar_ExpandedList(string const initialState, string const goalState, int& pathLength, int &numOfStateExpansions, int& maxQLength,
                               float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions, heuristicFunction heuristic){
											 
    string path = "";
    clock_t startTime = clock();


    // --------------------------------------------------------
    // Initialise statistics
    // --------------------------------------------------------
    pathLength = 0;
    numOfStateExpansions = 0;
    maxQLength = 0;
    actualRunningTime = 0.0;

    numOfDeletionsFromMiddleOfHeap=0;
    numOfLocalLoopsAvoided=0;
    numOfAttemptedNodeReExpansions=0;


    // --------------------------------------------------------
    // Q and Strict Expanded List
    // --------------------------------------------------------
    vector<Puzzle*> Q;
    unordered_set<string> expanded;
    CompareFCost compareF;


    // --------------------------------------------------
    // Create initial node
    // --------------------------------------------------
    Puzzle* initialPuzzle = new Puzzle(initialState, goalState);

    // h and f values
    initialPuzzle->updateHCost(heuristic);
    initialPuzzle->updateFCost();

    Q.push_back(initialPuzzle);
    make_heap(Q.begin(), Q.end(), compareF);
    maxQLength = 1;


    // --------------------------------------------------
    // Main A* Search loop
    // --------------------------------------------------
    while(!Q.empty()) {
        // Remove node with the smallest f-cost.
        // If f ties, larger g-cost wins.
        pop_heap(Q.begin(), Q.end(), compareF);
        Puzzle* current = Q.back();
        Q.pop_back();
        string currentState = current->getString();


        // --------------------------------------------------
        // Strict Expanded List check
        // --------------------------------------------------
        if(expanded.find(currentState) != expanded.end()) {
            numOfAttemptedNodeReExpansions++;
            delete current;
            continue;
        }
        expanded.insert(currentState);
        numOfStateExpansions++;


        // --------------------------------------------------
        // Goal test 
        // -------------------------------------------------
        if(current->goalMatch()) {
            path = current->getPath();
            pathLength = current->getPathLength();
            delete current;
            break;
        }

        // --------------------------------------------------
        // Helper for processing successors
        // --------------------------------------------------
        auto pushSuccessor = [&](Puzzle* next){
            string nextState = next->getString();

            // Do not re-expand a state that is already in the Strict Expanded List
            if(expanded.find(nextState) != expanded.end()) {
                numOfAttemptedNodeReExpansions++;
                delete next;
                return;
            }
            // Calculate A* values for this successor
            next->updateHCost(heuristic);
            next->updateFCost();
            Q.push_back(next);
            push_heap(Q.begin(), Q.end(), compareF);
            if((int)Q.size() > maxQLength) {
                maxQLength = Q.size();
            }
        };

        // --------------------------------------------------
        // Required successor generation order: Up, Right, Down, Left
        // --------------------------------------------------
        if(current->canMoveUp()) {
            pushSuccessor(current->moveUp());
        }
        if(current->canMoveRight()) {
            pushSuccessor(current->moveRight());
        }
        if(current->canMoveDown()) {
            pushSuccessor(current->moveDown());
        }
        if(current->canMoveLeft()) {
            pushSuccessor(current->moveLeft());
        }
        delete current;
    }

    // --------------------------------------------------
    // Clean up any remaining nodes
    // --------------------------------------------------
    for(Puzzle* p : Q) {
        delete p;
    }
    Q.clear();

    // --------------------------------------------------------
    // Record execution time
    // --------------------------------------------------------
    actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);
    return path;
}

