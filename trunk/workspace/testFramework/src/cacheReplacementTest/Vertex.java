package cacheReplacementTest;

import java.util.Hashtable;

public class Vertex
{
	private int id;
	//vertex id, edge weight
	private Hashtable<Integer, Integer> Adjacencylist;

	public Vertex(int id){this.id = id;}
	
	public Vertex(int id, int nId, int weight)
	{
		this.id = id;
		addNeighbour(nId, weight);
	}

	public void addNeighbour(int id, int weight)
	{
		if(!Adjacencylist.containsKey(id))
			Adjacencylist.put(id, weight);
	}

	public Hashtable<Integer, Integer> getAdjacencylist()
	{
		return Adjacencylist;
	}
	
	/**
	 * @return the id
	 */
	public int getId() {
		return id;
	}
}