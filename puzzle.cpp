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
	//unAssignedGrids = new MinHeap(rowSize*columnSize, getHeuristic);
	//unAssignedGrids->initialize();
	vector<GridInfo*> row;
	GridInfo* newGridInfo = NULL;

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			if (arr[rowIdx][columnIdx] == '_')
			{
				newGridInfo = new GridInfo(rowIdx, columnIdx, columnSize, colors, false, 'U');
				// only push non source grids to queue since source grids are not involved in the push/pop process of CSP
				unAssignedGrids.push_back(newGridInfo);
			}
			else
			{
				newGridInfo = new GridInfo(rowIdx, columnIdx, columnSize, colors, true, arr[rowIdx][columnIdx]);
				numSourceGrids++;
			}
			row.push_back(newGridInfo);
			//unAssignedGrids.push(newGridInfo);
		}
		puzzle.push_back(row);
		row.clear();
	}
}

void CPuzzle::destroy()
{
	//unAssignedGrids->printAllValues();
	//unAssignedGrids->destroy();
	//delete unAssignedGrids;

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			free(puzzle[rowIdx][columnIdx]);
		}
	}
}

void CPuzzle::assignValue(GridInfo* grid, char val)
{
	vector<GridInfo*>::iterator it = find(unAssignedGrids.begin(),unAssignedGrids.end(),grid);
	numAssignment++;
	grid->color = val;
	if (it != unAssignedGrids.end())
	{
		unAssignedGrids.erase(it);
	}
	pendingGrids.push_back(grid);
	//unAssignedGrids->extractMin();
	//unAssignedGrids.pop();
}

GridInfo* CPuzzle::chooseGrid()
{
	GridInfo* minHeuristicGrid = NULL;

	// printUnAssignedGridHeu();

	if (!unAssignedGrids.empty())
	{
		minHeuristicGrid = unAssignedGrids[0];
		
		for (auto &pGrid : unAssignedGrids)
		{
			if (pGrid->heuristic < minHeuristicGrid->heuristic)
			{
				minHeuristicGrid = pGrid;
			}
		}
	}
	//GridInfo* nextCoordToAssign = unAssignedGrids.top();
	return minHeuristicGrid;
}

void CPuzzle::printUnAssignedGridHeu()
{
	for (auto &pGrid : unAssignedGrids)
	{
		printf("%d ", pGrid->heuristic);
	}
	printf("\n");
}

void CPuzzle::undoAssign(GridInfo* grid)
{
	GridInfo* gridToUndo = pendingGrids.back(); //pop the last element from stack
	//assert(gridToUndo->gridId == grid->gridId);
	pendingGrids.pop_back();
	gridToUndo->color = 'U';
	unAssignedGrids.push_back(gridToUndo);
	//unAssignedGrids.push(gridToUndo);
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

vector<GridInfo*> CPuzzle::getAdjGrids(Coord position)
{
	vector<GridInfo*> adjacentGrids;

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
	return adjacentGrids;
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
	
	// unassigned grid cannot be invalid
	if (currentGrid->color == 'U')
	{
		return true;
	}

	adjacentGrids = getAdjGrids(position);

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
	//print all recorded arc relationship
	for (auto &arcMap : arcToCheck)
	{
		printf("%d -> %d \n",arcMap.first->gridId, arcMap.second->gridId);
	}
	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			printf("%d ", puzzle[rowIdx][columnIdx]->legalVal.size());
		}
		printf("\n");
	}
	/*
	// test min heap
	GridInfo* temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
	GridInfo* insertBack = NULL;
	int insertEveryTwoItr = 0;
	while (temp)
	{
		printf("%d\n", temp->heuristic);
		unAssignedGrids->printAllValues();
		if (insertEveryTwoItr == 1)
		{
			insertBack = temp;
		}
		if (insertEveryTwoItr <=2)
		{
			temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
		}
		else
		{
			unAssignedGrids->insert(static_cast<void*>(insertBack));
			insertEveryTwoItr = 0;
		}
		insertEveryTwoItr++;
	}
	*/
	//for (int i = 0; i < (rowSize*columnSize); i++)
	//{
	//	GridInfo* dummy=NULL;
	//	pendingGrids.push_back(dummy);
	//}

	//Coord grid(3, 4);
	//printf("%d\n",puzzleViolationCheck());
	
	/*GridInfo* cause[8];
	GridInfo* effected[8];
	for (int i = 0; i < 8; i++)
	{
		cause[i] = new GridInfo;
		effected[i] = new GridInfo;
		pair<GridInfo*, GridInfo*> arcPair;
		arcPair.first = cause[i];
		arcPair.second = effected[i];
		arcToCheck[arcPair] = true;
	}
	pair<GridInfo*, GridInfo*> arcPair;
	arcPair.first = cause[7];
	arcPair.second = effected[7];
	printf("key cound:%d \n", arcToCheck.count(arcPair));
	printf("kfirst pair:%u -> %u\n", arcToCheck.begin()->first, arcToCheck.begin()->second);
	for (int i = 0; i < 8; i++)
	{
		delete cause[i];
		delete effected[i];
	}*/

	/*puzzleArcGen();
	printf("%d\n", arcToCheck.size());*/
	

}

void CPuzzle::printResult()
{
	COORD pos = { 0, 1 };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			printf("%c ", puzzle[rowIdx][columnIdx]->color);
		}
		printf("\n");
	}
	printf("\n");
}

bool CPuzzle::solve()
{
	GridInfo* nextGrid = chooseGrid();
	bool isValid = true;

	//assert(nextGrid != NULL);
	//assert((pendingGrids.size()+unAssignedGrids.size()) == ((columnSize*rowSize) - numSourceGrids));

	if (allAssigned())
	{
		return true;
	}

	for (vector<char>::iterator it = nextGrid->legalVal.begin(); it != nextGrid->legalVal.end(); it++)
	{
		assignValue(nextGrid, *it);
		//printResult();
		isValid = puzzleViolationCheck();

		if (isValid)
		{
			isValid = forwardChecking(nextGrid->coord);
		}

		if (!isValid)
		{
			undoAssign(nextGrid);
			restorAdjGridLegalVal(nextGrid->coord);
			continue;
		}
		else
		{
			isValid = solve();
			if (isValid)
			{
				return true;
			}
			else
			{
				undoAssign(nextGrid);
				restorAdjGridLegalVal(nextGrid->coord);
			}
		}
	}
	return false;
}

void CPuzzle::discardLegalVal(GridInfo* myGrid, int discardGridID, char val)
{
	//remove value from legalValue
	vector<char>::iterator it = find(myGrid->legalVal.begin(), myGrid->legalVal.end(), val);
	// == myVector.end() means the element was not found
	if (it != myGrid->legalVal.end())
	{
		myGrid->legalVal.erase(it);
	}
	//save removed value to discarded value
	if (myGrid->discardedValue.count(discardGridID) == 0)
	{
		vector<char> newVector = {val};
		myGrid->discardedValue.insert(pair<int, vector<char>>(discardGridID, newVector));
	}
	else
	{
		myGrid->discardedValue[discardGridID].push_back(val);
	}
	myGrid->heuristic = myGrid->legalVal.size();
}

bool CPuzzle::forwardChecking(Coord responsibleGrid)
{	
	GridInfo* pGrid = NULL;
	bool isValid = true;
	vector<char> legalValCopy;	// a copy of legalVector to iterate through. Because original vector will be shrinked during the iteration
	vector<GridInfo*> adjGrids = getAdjGrids(responsibleGrid);
	for (vector<GridInfo*>::iterator ppGrid = adjGrids.begin(); ppGrid != adjGrids.end(); ppGrid++)
	{
		pGrid = *ppGrid;
		if(pGrid->color == 'U')
		{
			//for debugging====================================
			//if (pGrid->legalVal.size() < 5)
			//{
			//	int y = 1;
			//}
			//=================================================
			legalValCopy = pGrid->legalVal;
			for(auto &val:legalValCopy)
			{
				pGrid->color = val;
				isValid = puzzleViolationCheck();
				pGrid->color = 'U';
				if (!isValid)
				{
					discardLegalVal(pGrid, puzzle[responsibleGrid.row][responsibleGrid.column]->gridId, val);
				}
			}
			if (pGrid->legalVal.empty())
			{
				restorAdjGridLegalVal(responsibleGrid);
				return false;
			}

		}
	}
	return true;
}

void CPuzzle::restorAdjGridLegalVal(Coord responsibleCoord)
{
	vector<GridInfo*> adjGrids = getAdjGrids(responsibleCoord);
	for (auto &pGrid : adjGrids)
	{
		if (pGrid->color == 'U')
		{
			restoreGridLegalVal(pGrid, puzzle[responsibleCoord.row][responsibleCoord.column]->gridId);
		}
	}
}

void CPuzzle::restoreGridLegalVal(GridInfo* thisGrid, int responsibleGridID)
{
	//GridInfo* pGrid = puzzle[currentCoord.row][currentCoord.column];
	for (auto &value : thisGrid->discardedValue[responsibleGridID])
	{
		thisGrid->legalVal.push_back(value);
	}
	thisGrid->discardedValue.erase(responsibleGridID);
	thisGrid->heuristic = thisGrid->legalVal.size();
}

void CPuzzle::changedGridArcGen(Coord coordinate)
{
	GridInfo* pEffectedGrid = puzzle[coordinate.row][coordinate.column];
	vector<GridInfo*> adjGrids = getAdjGrids(coordinate);
	for (auto &causingGrid : adjGrids)
	{
		if (!causingGrid->isSource)
		{
			pair<GridInfo*, GridInfo*> arcPair;
			arcPair.first = causingGrid;
			arcPair.second = pEffectedGrid;
			//if ( find(arcToCheck.begin(), arcToCheck.end(), arcPair)== arcToCheck.end())
			//{
				arcToCheck.push_back(arcPair);
			//}
		}
	}
}

void CPuzzle::gridArcGen(Coord coordinate)
{
	GridInfo* causeGrid = puzzle[coordinate.row][coordinate.column];
	vector<GridInfo*> adjGrids = getAdjGrids(coordinate);
	for (auto &pEffectedGrid : adjGrids)
	{
		//if (!pEffectedGrid->isSource)
		//{
			pair<GridInfo*, GridInfo*> arcPair;
			arcPair.first = causeGrid;
			arcPair.second = pEffectedGrid;
			//if (find(arcToCheck.begin(), arcToCheck.end(), arcPair) == arcToCheck.end())
			//{
				arcToCheck.push_back(arcPair);
			//}
		//}
	}
}

void CPuzzle::puzzleArcGen()
{
	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			if (!puzzle[rowIdx][columnIdx]->isSource)
			{
				gridArcGen(puzzle[rowIdx][columnIdx]->coord);
			}
		}
	}
}

//eliminate bad legal value of all grids in puzzle
void CPuzzle::puzzleArcCheck()
{
	bool isModified = false; //is legalVal of causing grid modified?
	pair<GridInfo*, GridInfo*> arcPair;
	puzzleArcGen();
	while (!arcToCheck.empty())
	{
		//arcPair.first is the causing grid, arcPair.second is the effected grid
		arcPair = arcToCheck.front();
		arcToCheck.erase(arcToCheck.begin());
		//for debugging==================
		if (puzzle[0][0]->color == 'U')
		{
			int o = 1;
		}
		//===============================
		// check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
		isModified = gridArcCheck(arcPair);
		if (isModified)
		{
			// causing grid has change in legal value, add arc of other grid to the causing grid
			changedGridArcGen(arcPair.first->coord);
		}
	}
}

// check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
bool CPuzzle::gridArcCheck(pair<GridInfo*, GridInfo*> arcPair)
{
	bool isModified = false;
	bool isValid = false;
	GridInfo* causingGrid = arcPair.first;
	GridInfo* effectedGrid = arcPair.second;
	vector<char> causeLegalValCopy = causingGrid->legalVal;
	vector<char>::iterator itLegalValToDiscard;
	for (auto &causeVal : causeLegalValCopy)
	{
		isValid = false;
		causingGrid->color = causeVal;
		for (auto &effVal : effectedGrid->legalVal)
		{
			if (!effectedGrid->isSource)
			{
				effectedGrid->color = effVal;
			}
			isValid = puzzleViolationCheck();
			if (!effectedGrid->isSource)
			{
				effectedGrid->color = 'U';
			}
			if (isValid)
			{
				break;
			}
		}
		causingGrid->color = 'U';
		if (!isValid)
		{
			itLegalValToDiscard = find(causingGrid->legalVal.begin(), causingGrid->legalVal.end(), causeVal);
			if (itLegalValToDiscard != causingGrid->legalVal.end())
			{
				causingGrid->legalVal.erase(itLegalValToDiscard);
				causingGrid->heuristic = causingGrid->legalVal.size();
			}

			if (!isModified)
			{
				isModified = true;
			}
		}
	}
	return isModified;
}

int main()
{
	//char* fileNames[3] = { "1212.txt","1214.txt","1414.txt" };
	//for (int i = 0; i < 3; i++)
	//{
		using namespace std;
		clock_t begin = clock();

		bool canSolve = true;
		CPuzzle* testPuzzle = new CPuzzle("puzzle.txt");
		testPuzzle->initialize();
		testPuzzle->puzzleArcCheck();
		//testPuzzle->puzzleArcGen();
		//testPuzzle->test();
		canSolve = testPuzzle->solve();
		testPuzzle->printResult();
		printf("numAssignment:%u \n", testPuzzle->numAssignment);
		if (!canSolve)
		{
			printf("cannot solve problem\n \n");
		}
		testPuzzle->destroy();

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC/1000);
		cout << "time: " << elapsed_secs << endl;
	//}

	return 0;
}