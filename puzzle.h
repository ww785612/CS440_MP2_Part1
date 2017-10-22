#include<stack>
#include<vector>
#include<queue>
#include<iostream>
#include <fstream>


using namespace std;
typedef struct coord Coord;
struct coord
{
	int column;
	int row;
	Coord(int inRow, int inColumn) :
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
};

typedef struct gridInfo GridInfo;
struct gridInfo
{
	//Coord coord;
	bool isSource;
	int gridId;
	int heuristic;
	char color;		//'U' is means unassigned
	std::vector<char> legalVal;

	GridInfo(int row, int column, int columnSize, vector<char> colors, bool inIsSource, char inColor)
	{
		isSource = inIsSource;
		gridId = columnSize*row + column;
		heuristic = gridId;
		color = inColor;
		legalVal = colors;
	}
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
private:
	std::stack<GridInfo*> pendingGrids;		// the assigned grids
	std::priority_queue<GridInfo*, vector<GridInfo*>, gridInfoLess> unAssignedGrids; // grids awaiting color assignment
	void loadPuzzle();
public:
	CPuzzle(const char* inFilePath)
	{
		filePath = inFilePath;
	}
	void initialize();
	void destroy();
	void test();
};