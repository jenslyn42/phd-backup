package cacheReplacementTest;

import java.util.Hashtable;
import java.util.Map;

public class Vertex
{
	private final int id;
	//vertex id, edge weight
	private Hashtable<Integer, Integer> Adjacencylist = new Hashtable<Integer, Integer>();

	private Vertex(int id){this.id = id;}
	
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
	
	public String toString()
	{
		String v = "id:"+id+"{";
		for(Map.Entry<Integer, Integer> nb : Adjacencylist.entrySet())
			v= v+nb.getKey()+"("+nb.getValue()+"),";

		v.substring(0,v.length()-1);
		
		return v.substring(0,v.length()-1)+"}";
	}
}