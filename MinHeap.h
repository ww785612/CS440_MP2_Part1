#include <stdlib.h>
#include <iostream>

//=============================maze==============================
//typedef struct mazePos MazePos;
//struct mazePos;
//
//typedef struct searchTreeNode SearchTreeNod;
//struct searchTreeNode;
//==========================MinHeap===============================
class MinHeap
{
private:
	void**			heapArray;
	int				heapSize;				// how many elements are currently in the heap
	int				capacity;
	int   (*getValue)(void*);					//function pointer for user class to access value in its structure
private:
	int				left(int i);
	int				right(int i);
	int				parent(int i);
	void			swap(int i1, int i2);
public:
	//============utility===========================
	MinHeap(int arraySize, int(*GetValFunct)(void*));
	void initialize();
	void reinitialize();
	void destroy();
	//============functional========================
	void			insert(void* newNode);
	void			minHeapify(int i);
	void*			extractMin();
	void*			returnMin();
	void			printAllValues();
};