#include<stack>
#include<vector>
#include<queue>
#include<iostream>
#include <fstream>
#include <assert.h>
#include <Windows.h>
#include <map>
#include <ctime>


using namespace std;
typedef struct coord Coord;
struct coord
{
	int column;
	int row;

	Coord(){}	//default constructor

	Coord(int inRow, int inColumn) : //custom constructor
	column(inColumn),
	row(inRow)
	{}
	void operator=(const Coord& inCoord)
	{
		this->column = inCoord.column;
		this->row = inCoord.row;
	}

	bool operator==(const Coord& inCoord)
	{
		if ((this->column == inCoord.column) && (this->row == inCoord.row))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Coord left()
	{
		Coord temp(this->row, (this->column - 1));
		return temp;
	}

	Coord right()
	{
		Coord temp(this->row, (this->column + 1));
		return temp;
	}

	Coord up()
	{
		Coord temp((this->row - 1), this->column);
		return temp;
	}

	Coord down()
	{
		Coord temp((this->row + 1), this->column);
		return temp;
	}
};

typedef struct gridInfo GridInfo;
struct gridInfo
{
	Coord coord;
	bool isSource;
	int gridId;
	int heuristic;	// heuristic = number of legal values
	char color;		//'U' is means unassigned
	vector<char> legalVal;
	map<int, vector<char>>  discardedValue;

	GridInfo(int row, int column, int columnSize, vector<char> colors, bool inIsSource, char inColor)
	{
		coord.row = row;
		coord.column = column;
		isSource = inIsSource;
		gridId = columnSize*row + column;
		color = inColor;
		if (!isSource)
		{
			legalVal = colors;
		}
		else
		{
			legalVal.push_back(inColor);
		}
		heuristic = legalVal.size();
	}
	GridInfo(){}
};

struct gridInfoLess
{
	bool operator()(const GridInfo* left, const GridInfo* right) const
	{
		return left->heuristic < right->heuristic;
	}
};

class CPuzzle
{
	const char* filePath;
	vector<vector<char>> arr;	//raw puzzle in characters
	vector<vector<GridInfo*>> puzzle;	// 2D array of the puzzle. Each grid contains info like color, heuristic, ID etc.
	vector<char> colors;		// all values that appear in the problem
	int rowSize;
	int columnSize;
	vector<pair<GridInfo*, GridInfo*>> arcToCheck;	//only key is used in the map for lookup, vlaue is not used.
private:
	vector<GridInfo*> pendingGrids;		// the assigned grids. Use vector to function as stack
	//vector<GridInfo*> sourceGrid;		// Source grids are kept in a seperate queue. Not to be touched in the pop/push process of CSP
	// priority_queue<GridInfo*, vector<GridInfo*>, gridInfoLess> unAssignedGrids; // grids awaiting color assignment
	vector<GridInfo*> unAssignedGrids;		//Heuristic is the number of legal value left
	int numSourceGrids;
public:
	unsigned int numAssignment;
	void printUnAssignedGridHeu();
	void loadPuzzle();
public:
	CPuzzle(const char* inFilePath)
	{
		filePath = inFilePath;
		numSourceGrids = 0;
		numAssignment = 0;
	}
	static int getHeuristic(void* inGrid) { return static_cast<GridInfo*>(inGrid)->heuristic; }
	void initialize();
	void destroy();
	bool allAssigned(){return (pendingGrids.size() == (rowSize*columnSize - numSourceGrids));}
	void assignValue(GridInfo* pCoordinate, char val);
	GridInfo* chooseGrid();
	void undoAssign(GridInfo* grid);
	bool puzzleViolationCheck();
	bool gridViolationCheck(Coord position);
	bool solve();
	void printResult();
	bool forwardChecking(Coord currentCoord);
	void discardLegalVal(GridInfo* myGrid, int discardGridID, char val);
	void restorAdjGridLegalVal(Coord currentCoord);
	void restoreGridLegalVal(GridInfo* thisGrid, int responsibleGridID);
	vector<GridInfo*> getAdjGrids(Coord coordinate);

	//for Arc Consistency
	void changedGridArcGen(Coord coordinate); //push arc of adjacent grids to me
	void gridArcGen(Coord coordinate);		  //push arc of me to adjacent grids
	void puzzleArcGen();
	void puzzleArcCheck();
	bool gridArcCheck(pair<GridInfo*, GridInfo*> arcPair);
	void test();
};