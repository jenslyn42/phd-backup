package cacheReplacementTest;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import java.util.Hashtable;
import java.util.Map;

public class RoadGraph {

	private static RoadGraph ref;

	//Vertex id -> Vertex object
	private Hashtable<Integer, Vertex> map = new Hashtable<Integer, Vertex>();
	private int mapSize = 0;

	private RoadGraph()	{readNetworkFile("map.txt");}

	/**
	 * Whoot!! for shizzle, this be a singleton object y'all
	 * 
	 * @return The roadGraph instance
	 */
	public static synchronized RoadGraph getMapObject()
	{
		if (ref == null)
			// it's ok, we can call this constructor
			ref = new RoadGraph();		
		return ref;
	}

	public Hashtable<Integer, Vertex> getMap()
	{
		return map;
	}

	/**
	 * Adds an edge to the map by updating the adjencency list of two vertices with the vertex id
	 * and weight to the other vertex.
	 * 
	 * @param v1 first vertex
	 * @param v2 second vertex
	 * @param w weight of edge
	 */
	private void addEdge(int v1, int v2, int w)
	{
		if(!map.containsKey(v1))
			map.put(v1, new Vertex(v1, v2, w));
		else
			map.get(v1).addNeighbour(v2, w);
		if(!map.containsKey(v2))
			map.put(v1, new Vertex(v2, v1, w));
		else
			map.get(v2).addNeighbour(v1, w);	
	}

	private void readNetworkFile(String fn)
	{
		try { 
			BufferedReader in = new BufferedReader(new FileReader(fn)); 
			String str, tokens[];
			while((str = in.readLine()) != null && str.split("\\s+").length < 3){}

			do{ 
				tokens = str.split("\\s+");
				addEdge(Integer.parseInt(tokens[0]),Integer.parseInt(tokens[1]),Integer.parseInt(tokens[2]));
			} while ((str = in.readLine()) != null);

			in.close(); 
		} catch (IOException e) {System.out.println("failed reading map: "+e.getMessage()); } 
	}

	/**
	 * Performs a single source shortest path search using the Dijkstra algorithm
	 * 
	 * @param s source vertex
	 * @param t target vertex
	 * @return vertices traversed between s and t inclusive, in the form (vertex, previous vertex)
	 * @throws Exception if t or s is not valid vertex ids.
	 */
	public ArrayList<Integer> dijkstraSSSP(int s, int t)
	{
		if(!map.containsKey(s) || !map.containsKey(t))
		{
			try {
				throw new Exception("SSSP query contains source and/or target node not in map");
			} catch (Exception e) {
				System.err.println(e.getMessage());
				e.printStackTrace();
			}
		}
		if(s == t) 
		{
			ArrayList<Integer> tmp = new ArrayList<Integer>();
			tmp.add(s);
			return tmp;
		}
			
		Hashtable<Integer, Integer> M = new Hashtable<Integer, Integer>();
		Hashtable<Integer, Integer> backtrace = new Hashtable<Integer, Integer>();
		MinHeap H = new MinHeap(new Pair<Integer, Integer>(s,0), mapSize);
		ArrayList<Integer> sp = new ArrayList<Integer>(); 
		//use pair to represent (vertex id, distance)
		Pair<Integer, Integer> entry = null;

		while(!H.isEmpty())
		{
			try {
				entry = H.removemin();
			} catch (Exception e) {
				System.err.println(e.getMessage());
				e.printStackTrace();
			} 
			Vertex u = map.get(entry.s);

			if(!M.containsKey(u.getId()) || M.get(u.getId()) > entry.t)
			{
				M.put(u.getId(), entry.t);
				Hashtable<Integer, Integer> adjacencylist = u.getAdjacencylist();

				//Neighbor<vertex id, edge weight>
				for (Map.Entry<Integer, Integer> neighbour : adjacencylist.entrySet()) 
				{
					if(!M.containsKey(u.getId()))
					{
						H.insert(new Pair<Integer, Integer>(neighbour.getKey(), entry.t+neighbour.getValue()));
						backtrace.put(neighbour.getKey(), u.getId());
					}
					if (M.get(u.getId()) > entry.t)
					{
						H.updateEntry(u.getId(), entry.t);
						backtrace.put(neighbour.getKey(), u.getId());
					}
				}
			}
			if(u.getId() == t)
			{
				int prevNode = t;
				sp.add(prevNode);
				do{
					prevNode = backtrace.get(prevNode);
					sp.add(prevNode);
				}while(prevNode != s);
				return sp;
			}
		}
		return sp;	
	}

	public Object clone() throws CloneNotSupportedException
	{
		throw new CloneNotSupportedException(); 
	}
}


