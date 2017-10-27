#include "puzzle.h"

void CPuzzle::loadPuzzle()
{
	char c;
	vector<char> row;
	ifstream myfile(filePath);
	if (myfile.is_open())
	{
		while (myfile.get(c))
		{
			if (c != '\n')
			{
				// if char is a color never seen before
				if ((c != '_') && (find(colors.begin(), colors.end(), c) == colors.end()) )
				{
					colors.push_back(c);
				}
				row.push_back(c);
			}
			else
			{
				arr.push_back(row);
				row.clear();
			}
		}
		arr.push_back(row);//push in the last line
		row.clear();
		myfile.close();
	}
}

void CPuzzle::initialize()
{
	loadPuzzle();
	columnSize = arr[0].size();
	rowSize = arr.size();
	vector<GridInfo*> row;
	GridInfo* newGridInfo = NULL;

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			if (arr[rowIdx][columnIdx] == '_')
			{
				newGridInfo = new GridInfo(rowIdx, columnIdx, columnSize, colors, false, 'U');
			}
			else
			{
				newGridInfo = new GridInfo(rowIdx, columnIdx, columnSize, colors, true, arr[rowIdx][columnIdx]);
			}
			row.push_back(newGridInfo);
			unAssignedGrids.push(newGridInfo);
		}
		puzzle.push_back(row);
		row.clear();
	}
}

void CPuzzle::destroy()
{
	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			free(puzzle[rowIdx][columnIdx]);
		}
	}
}

void CPuzzle::assignValue(Coord coordinate, char val)
{
	puzzle[coordinate.row][coordinate.column]->color = val;
	pendingGrids.push(puzzle[coordinate.row][coordinate.column]);
	unAssignedGrids.pop();
}

GridInfo* CPuzzle::choosGrid()
{
	GridInfo* nextCoordToAssign = unAssignedGrids.top();
	return nextCoordToAssign;
}

void CPuzzle::undoAssign(GridInfo* grid)
{
	GridInfo* gridToUndo = pendingGrids.top();
	assert(gridToUndo == grid);
	pendingGrids.pop();
	gridToUndo->color = 'U';
	unAssignedGrids.push(gridToUndo);
}

bool CPuzzle::puzzleViolationCheck()
{
	bool checkResult = true;
	Coord puzzleIterator(0, 0);
	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			puzzleIterator.row = rowIdx;
			puzzleIterator.column = columnIdx;
			checkResult = gridViolationCheck(puzzleIterator);
			if (checkResult == false)
			{
				break;
			}
		}
		if (checkResult == false)
		{
			break;
		}
	}

	return checkResult;
}

bool CPuzzle::gridViolationCheck(Coord position)
{
	//	 1
	//	 ^
	//0<   >2
	//	 v
	//	 3
	// x->column
	// y->row
	GridInfo* currentGrid = puzzle[position.row][position.column];
	vector<GridInfo*> adjacentGrids;
	int numMatch = 0;
	int numOpenGrid = 0;

	Coord coordinate = position.left();
	if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
		(coordinate.row >= 0) && (coordinate.row < rowSize))
	{
		adjacentGrids.push_back(puzzle[coordinate.row][coordinate.column]);
	}

	coordinate = position.right();
	if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
		(coordinate.row >= 0) && (coordinate.row < rowSize))
	{
		adjacentGrids.push_back(puzzle[coordinate.row][coordinate.column]);
	}

	coordinate = position.up();
	if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
		(coordinate.row >= 0) && (coordinate.row < rowSize))
	{
		adjacentGrids.push_back(puzzle[coordinate.row][coordinate.column]);
	}

	coordinate = position.down();
	if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
		(coordinate.row >= 0) && (coordinate.row < rowSize))
	{
		adjacentGrids.push_back(puzzle[coordinate.row][coordinate.column]);
	}

	for (vector<GridInfo*>::iterator it = adjacentGrids.begin(); it != adjacentGrids.end(); it++)
	{
		if ((*it)->color == currentGrid->color)
		{
			numMatch++;
		}
		else if ((*it)->color == 'U')
		{
			numOpenGrid++;
		}
	}
	
	//is source grid
	if (!currentGrid->isSource)
	{
		// no matching, only good when more than 2 unassigned grids are available
		if (numMatch == 0)
		{
			if (numOpenGrid >= 2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (numMatch == 1)
		{
			if (numOpenGrid >= 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (numMatch == 2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	//not source grid
	else
	{
		if (numMatch == 1)
		{
			return true;
		}
		else if (numMatch == 0)
		{
			if (numOpenGrid >= 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

}

void CPuzzle::test()
{
	//for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	//{
	//	for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
	//	{
	//		printf("%d ", puzzle[rowIdx][columnIdx]->heuristic);
	//	}
	//	printf("\n");
	//}
	/*while (!unAssignedGrids.empty())
	{
		cout << unAssignedGrids.top()->heuristic << endl;
		unAssignedGrids.pop();
	}*/

	//for (int i = 0; i < (rowSize*columnSize); i++)
	//{
	//	GridInfo* dummy=NULL;
	//	pendingGrids.push(dummy);
	//}

	Coord grid(3, 4);
	printf("%d\n",puzzleViolationCheck());
		

}

int main()
{

	CPuzzle* testPuzzle = new CPuzzle("C:/Users/Zi/Documents/Visual Studio 2015/Projects/CS440_MP2_part1/CS440_MP2_part1/puzzle.txt");
	testPuzzle->initialize();
	testPuzzle->test();
	testPuzzle->destroy();
	return 0;
}