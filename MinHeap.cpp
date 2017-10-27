#include "MinHeap.h"

//=================private functions====================
int MinHeap::left(int i)
{
	if (2 * i <= heapSize)
	{
		return 2 * i;
	}
	else
	{
		return NULL;
	}
}

int MinHeap::right(int i)
{
	if ((2 * i + 1) <= heapSize)
	{
		return 2 * i + 1;
	}
	else
	{
		return NULL;
	}
}

int MinHeap::parent(int i)
{
	return i / 2;
}

void MinHeap::swap(int i1, int i2)
{
	void* temp = heapArray[i1];

	heapArray[i1] = heapArray[i2];
	heapArray[i2] = temp;
}

//=============public functions===========
MinHeap::MinHeap(int arraySize, int(*GetValFunct)(void*))
{
	heapArray = NULL;
	heapSize = 1; //initialized to 1 because index 0 of the array is not used
	capacity = arraySize + 1; //element 0 is not used(wasted). Therefore need to allocate one more to fit entire "arraySize"
	getValue = GetValFunct;
}

void MinHeap::initialize()
{
	heapArray = (void**)malloc(capacity*sizeof(void*));
	heapArray[0] = NULL; //because index 0 is not used. Setting it to NULL helps prevent dereferencing it
}

void MinHeap::destroy()
{
	void* temp = extractMin();
	while (temp)
	{
		temp = extractMin();
	}
	free(heapArray);
}

void MinHeap::insert(void* newNode)
{
	if (heapSize == capacity)
	{
		std::cout << "\nOverflow: Could not insertKey\n";
		return;
	}
	
	// First insert the new key at the end
	heapSize++;
	int i = heapSize - 1;
	heapArray[i] = newNode;

	// Fix the min heap property if it is violated
	if (heapSize > 2) // first element inserts into a empty heap is a special case. This if block excludes the special case.
	{
		int parentIdx = parent(i);	//Never touch index 0, neither parent idx nor child idx should ever reach index 0
		while ((parentIdx > 0) && (i > 0) && (getValue(heapArray[parentIdx]) > getValue(heapArray[i])))
		{
			swap(i, parent(i));
			i = parent(i);
			parentIdx = parent(i);
		}
	}
}

void MinHeap::minHeapify(int i) //i is the index in array from there MinHeapify starts
{
	int l = left(i);
	int r = right(i);
	int smallest = i;

	if (l == 0) // left child index out of bound. It is empty(no child, no lower level). We are at the buttom already 
	{
		return;
	}

	if (l < heapSize && (getValue(heapArray[l]) < getValue(heapArray[i])))
	{
		smallest = l;
	}

	if (r == 0) //right child index out of bound. current node has no right child. We are at the bottom of tree, swap with left child if left child is smaller
	{
		if (smallest != i)
		{
			swap(i, smallest);
		}
		return;
	}

	if (r < heapSize && (getValue(heapArray[r]) < getValue(heapArray[smallest])))
	{
		smallest = r;
	}

	if (smallest != i)
	{
		swap(i, smallest);
		minHeapify(smallest);          //if a swap happened, trace down the swapped node.
	}
}

void* MinHeap::extractMin()
{
	if (heapSize <= 1)					// heap empty, return NULL pointer
		return NULL;
	if (heapSize == 2)					// only 1 element in min heap, retuen that one
	{
		heapSize--;
		return heapArray[1];
	}

	void* minVal = heapArray[1]; //first element in heap has index 1, not 0
	heapArray[1] = heapArray[heapSize - 1];
	heapSize--;
	minHeapify(1);

	return minVal;
}

void*  MinHeap::returnMin()
{
	if (heapSize <= 1)					// heap empty, return NULL pointer
		return NULL;

	return heapArray[1]; //first element in heap has index 1, not 0
}

void	MinHeap::reinitialize()
{
	heapSize = 1; //initialized to 1 because index 0 of the array is not used
}

void	MinHeap::printAllValues()
{
	/*int nodeCntOnCurrentLevl = 1;
	int nodePrinted = 0;
	for (int i = 1; i < heapSize; i++)
	{
		printf(" %d", getValue(heapArray[i]));
		nodePrinted++;
		if (nodePrinted == nodeCntOnCurrentLevl)
		{
			nodeCntOnCurrentLevl = nodeCntOnCurrentLevl * 2;
			nodePrinted = 0;
			printf("\n");
		}
	}*/
	for (int i = 1; i < heapSize; i++)
	{
		printf("%d ", getValue(heapArray[i]));
	}
	printf("\n");
}

//int main()
//{
//	SearchTreeNod* newNode;
//	MinHeap* minHeap = new MinHeap(100);
//	minHeap->initialize();
//	for (int i = 0; i < 9; i++)
//	{
//		newNode = (SearchTreeNod*)malloc(sizeof(SearchTreeNod));
//		newNode->costSoFar = i;
//		minHeap->insert(newNode);
//	}
//	for (int i = 0; i < 9; i++)
//	{ 
//		SearchTreeNod* temp = minHeap->extractMin();
//		if (temp)
//		{
//			std::cout << temp->costSoFar << std::endl;
//			free(temp);
//		}
//	}
//	minHeap->destroy();
//	delete minHeap;
//	return 0;
//}

