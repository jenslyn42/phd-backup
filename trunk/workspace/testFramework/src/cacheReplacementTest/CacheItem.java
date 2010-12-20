package cacheReplacementTest;

import java.util.ArrayList;

public class CacheItem implements Comparable<CacheItem>
{
	public final int id, s, t;
	private int accessKey;
	public final int size;
	public final ArrayList<Integer> item;

	public CacheItem(int key, ArrayList<Integer> item)
	{
		this.id = key;
		this.accessKey = key;
		this.item = item;
		this.size = item.size();
		this.s = -1;
		this.t = -1;
	}
	
	public CacheItem(int key, ArrayList<Integer> item, int queryStartNode, int queryTargetNode)
	{
		this.id = key;
		this.accessKey = key;
		this.item = item;
		this.size = item.size();
		this.s = queryStartNode;
		this.t = queryTargetNode;
	}

	public void updateKey(int newKey){accessKey = newKey;}
	
	public int key(){return accessKey;}
	
	@Override
	public int compareTo(CacheItem ci) 
	{
		return this.accessKey - ci.accessKey;
	}
}