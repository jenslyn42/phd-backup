package cacheReplacementTest;

public class MinHeap {
	private Pair<Integer, Integer>[] Heap;
	private int maxsize;
	private int size;

	public MinHeap(Pair<Integer, Integer> s, int max) {
		maxsize = max;
		Heap = new Pair[maxsize];
		size = 0 ;
		Heap[0] = s;
	}

	private int leftchild(int pos) {
		return 2*pos;
	}
	private int rightchild(int pos) {
		return 2*pos + 1;
	}

	private int parent(int pos) {
		return  pos / 2;
	}

	private boolean isleaf(int pos) {
		return ((pos > size/2) && (pos <= size));
	}

	private void swap(int pos1, int pos2) {
		Pair<Integer, Integer> tmp;

		tmp = Heap[pos1];
		Heap[pos1] = Heap[pos2];
		Heap[pos2] = tmp;
	}

	/**
	 * Finds the first occurrence of e
	 * 
	 * @param e the entry.s to find in the min-heap
	 * @return first occurrence of e
	 */
	private int findEntry(int e)
	{
		for(int i = 0; i < Heap.length; i++)
		{
			if(Heap[i].s == e)
				return i;
		}
		return -1;
	}

	public void insert(Pair<Integer, Integer> elem) {
		size++;
		Heap[size] = elem;
		int current = size;

		while (Heap[current].t < Heap[parent(current)].t) {
			swap(current, parent(current));
			current = parent(current);
		}	
	}

	/**
	 * 
	 */
	public void updateEntry(int s, int newVal){

		int elemHeapPos = findEntry(s);

		Heap[elemHeapPos] = new Pair<Integer, Integer>(s,newVal);
		int current = elemHeapPos;

		//if newVal is smaller than parent.t value
		if(Heap[current].t < Heap[parent(current)].t)
		{
			do{
				swap(current, parent(current));
				current = parent(current);
			}while (Heap[current].t < Heap[parent(current)].t);	
		}
		//if newVal is bigger than one of its children.t value
		else if(Heap[current].t > Heap[leftchild(current)].t || Heap[current].t > Heap[rightchild(current)].t) {
			do{
				//if current is bigger than leftchild.t value
				if(Heap[current].t > Heap[leftchild(current)].t)
				{
					swap(current, leftchild(current));
					current = leftchild(current);
				}
				//if current is bigger than rightchild.t value
				else if(Heap[current].t > Heap[rightchild(current)].t)
				{
					swap(current, rightchild(current));
					current = rightchild(current);
				}
			}while (Heap[current].t > Heap[leftchild(current)].t || Heap[current].t > Heap[rightchild(current)].t);
		}
	}

	public boolean isEmpty()
	{
		return (size == 0);
	}

	public void print() {
		int i;
		for (i=1; i<=size; i++)
			System.out.print(Heap[i] + " ");
		System.out.println();
	}

	public Pair<Integer, Integer> removeMin() throws Exception {
		if(isEmpty())
			throw new Exception("Heap Empty!");
		
			swap(1,size);
			size--;
			if (size != 0)
				pushdown(1);
			return Heap[size+1];
	}

	/**
	 * Returns the root of the Min-Heap without removing it
	 * 
	 * @return Minimum value in the Heap
	 * @throws Exception If heap is empty
	 */
	public Pair<Integer, Integer> peekMin() throws Exception {
		if(isEmpty())
			throw new Exception("Heap Empty!");
		
			return Heap[0];
	}
	
	private void pushdown(int position) {
		int smallestchild;
		while (!isleaf(position)) {
			smallestchild = leftchild(position);
			if ((smallestchild < size) && (Heap[smallestchild].t > Heap[smallestchild+1].t))
				smallestchild = smallestchild + 1;
			if (Heap[position].t <= Heap[smallestchild].t) return;
			swap(position,smallestchild);
			position = smallestchild;
		}
	}
}




