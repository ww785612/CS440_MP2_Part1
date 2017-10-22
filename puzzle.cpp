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
	while (!unAssignedGrids.empty())
	{
		cout << unAssignedGrids.top()->heuristic << endl;
		unAssignedGrids.pop();
	}
}

int main()
{

	CPuzzle* testPuzzle = new CPuzzle("C:/Users/Zi/Documents/Visual Studio 2015/Projects/CS440_MP2_part1/CS440_MP2_part1/puzzle.txt");
	testPuzzle->initialize();
	testPuzzle->test();
	testPuzzle->destroy();
	return 0;
}