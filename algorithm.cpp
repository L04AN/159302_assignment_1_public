

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
											 
    string path ="";
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
            numOfLocalLoopsAvoided++;
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

            // Strct Expanded List:
            // Never expanded a state that has alreadt been expanded
            if(expanded.find(nextState) != expanded.end()) {
                numOfAttemptedNodeReExpansions++;
                delete next;
                return;
            }
            Q.push_back(next);
            push_heap(Q.begin(), Q.end(), compareG);
            if((int)Q.size() > maxQLength) {
                maxQLength = Q.size();
            }
        };


        // --------------------------------------------------
        // Generate successors in REQUIRED order:
        // Up, Right, Down, Left
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
    actualRunningTime =
        ((float)(clock() - startTime) / CLOCKS_PER_SEC);
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
											 
   string path;
   clock_t startTime;
   
   numOfDeletionsFromMiddleOfHeap=0;
   numOfLocalLoopsAvoided=0;
   numOfAttemptedNodeReExpansions=0;


    // cout << "------------------------------" << endl;
    // cout << "<<aStar_ExpandedList>>" << endl;
    // cout << "------------------------------" << endl;
	actualRunningTime=0.0;	
	startTime = clock();
	srand(time(NULL)); //RANDOM NUMBER GENERATOR - ONLY FOR THIS DEMO.  YOU REALLY DON'T NEED THIS! DISABLE THIS STATEMENT.
	maxQLength= rand() % 200; //AT THE MOMENT, THIS IS JUST GENERATING SOME DUMMY VALUE.  YOUR ALGORITHM IMPLEMENTATION SHOULD COMPUTE THIS PROPERLY.
	numOfStateExpansions = rand() % 200; //AT THE MOMENT, THIS IS JUST GENERATING SOME DUMMY VALUE.  YOUR ALGORITHM IMPLEMENTATION SHOULD COMPUTE THIS PROPERLY


	
	
//***********************************************************************************************************
	actualRunningTime = ((float)(clock() - startTime)/CLOCKS_PER_SEC);
	path = "DDRRLLLUUURDLUDURDLUU"; //this is just a dummy path for testing the function
	pathLength = path.size();
	return path;		
		
}

