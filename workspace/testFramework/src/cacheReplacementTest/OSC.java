package cacheReplacementTest;



import java.util.ArrayList;
import java.util.Collections;


public class OSC { //Optimal Substructure Cache

	//cache
	ArrayList<CacheItem> cache;

	private int numTotalQueries = 0;
	private int numCacheHits = 0;

	private int cacheSize = 0;
	private int cacheUsed = 0;
	
	boolean useNodeScore = false;
	boolean useHitScore = false;

	private ArrayList<Integer> nodeHits;

	public OSC(TestSettings ts)
	{
		cache = new ArrayList<CacheItem>();
		this.cacheSize = ts.getCacheSize();

		nodeHits  = new ArrayList<Integer>(ts.getQueryRangeEnd());

		for(int i=0; i < ts.getQueryRangeEnd(); i++)
		{
			nodeHits.add(i, 0);
		}
		
		useNodeScore = ts.isUseNodeScore();
		useHitScore = ts.isUseHitScore();
	}

	public void readQuery(Pair<Integer, Integer> query)
	{
		try {
			checkAndUpdateCache(query);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		numTotalQueries++;
	}

	public void readQueryList(ArrayList<Pair<Integer, Integer>> queryList)
	{
		for(Pair<Integer, Integer> q : queryList)
		{
			readQuery(q);
		}
	}

	private void checkAndUpdateCache(Pair<Integer, Integer> query) throws Exception
	{
		boolean cacheHit = false;

		for(CacheItem ci : cache)
		{
			if(ci.item.contains(query.s) && ci.item.contains(query.t))
			{
				numCacheHits++;
				ci.updateKey(numTotalQueries);
				Collections.sort(cache);
				cacheHit = true;
				int matchFrom = ci.item.indexOf(query.s);
				int matchTo = ci.item.indexOf(query.t);
				ArrayList<Integer> itemSubMatch = new ArrayList<Integer>();
				if(matchFrom < matchTo)
					ci.item.subList(matchFrom, matchTo).addAll(itemSubMatch);
				else
					ci.item.subList(matchTo, matchFrom).addAll(itemSubMatch);

				updateNodeHits(itemSubMatch);
				break;
			}
		}

		if(!cacheHit)
		{
			ArrayList<Integer> spResult = RoadGraph.getMapObject().dijkstraSSSP(query.s, query.t);
			updateNodeHits(spResult);
			int querySize = spResult.size();
			ArrayList<Integer> nodesInQueryResult = spResult;
		
			if(cache.size()!= 0)
				testToReplaceItem(querySize, nodesInQueryResult);
			else
			{
				CacheItem e = new CacheItem(numTotalQueries, nodesInQueryResult);
				cache.add(e);
				cacheUsed = cacheUsed + e.size;
			}
		}
	}

	private void updateNodeHits(ArrayList<Integer> queryResult)
	{
		for(int i : queryResult)
			nodeHits.set(i, nodeHits.get(i)+1);
	}
	
	private void testToReplaceItem(int querySize, ArrayList<Integer> nodesInQueryResult) throws Exception
	{
		boolean notEnoughSpace = true;
		int qSum = 0;
		int qScore = 0;
		//calculate score for query		
		for(int i : nodesInQueryResult)
			qSum += nodeHits.get(i);

		if(querySize > cacheSize)
			return;
		
		//Pair(cache index, score) - Min-heap property based on second value of Pair
		MinHeap removeCandidate;
		ArrayList<Integer> ci;
		ci = cache.get(0).item;
		int nodes = ci.size();
		int sum = 0;

		for(int i : ci)
			sum += nodeHits.get(i);
		
		//calculate score based on test flags
		if(useNodeScore && useHitScore){
			removeCandidate = new MinHeap(new Pair<Integer,Integer>(0,nodes+sum),cache.size());
			qScore = qSum+querySize;
		}
		else if(useNodeScore){
			removeCandidate = new MinHeap(new Pair<Integer,Integer>(0,nodes),cache.size());
			qScore = querySize;
		}
		else if(useHitScore){
			removeCandidate = new MinHeap(new Pair<Integer,Integer>(0,sum),cache.size());
			qScore = qSum;
		}
		else{
			System.err.println("Can't set both nodeScore and hitScore to false, setting both true");
			removeCandidate = new MinHeap(new Pair<Integer,Integer>(0,nodes+sum),cache.size());
			qScore = qSum+querySize;
		}
		//calculate scores of all items, and place them in min-heap
		for(int k = 1; k < cache.size(); k++)
		{
			ci = cache.get(k).item;
			nodes = ci.size();
			sum = 0;

			for(int i : ci)
				sum += nodeHits.get(i);

			//calculate score based on test flags
			if(useNodeScore && useHitScore)
				removeCandidate.insert(new Pair<Integer,Integer>(k,nodes+sum));
			else if(useNodeScore)
				removeCandidate.insert(new Pair<Integer,Integer>(k,nodes));
			else if(useHitScore)
				removeCandidate.insert(new Pair<Integer,Integer>(k,sum));
			else{
				System.err.println("Can't set both nodeScore and hitScore to false, setting both true");
				removeCandidate.insert(new Pair<Integer,Integer>(k,nodes+sum));
			}
		}

		if(qScore > removeCandidate.peekMin().t)
		{		
			ArrayList<Integer> removeIndexes = new ArrayList<Integer>();
			int tmpRemIndex = 0;
			do{
				if((cacheSize - cacheUsed) >= querySize )
				{
					//remove all cacheItems marked for removal
					Collections.sort(removeIndexes, Collections.reverseOrder());
					for(int i = 0; i < removeIndexes.size(); i++)
					{
						cache.remove(removeIndexes.get(i));
					}
					
					//add new cache item to cache
					CacheItem e = new CacheItem(numTotalQueries, nodesInQueryResult);
					cache.add(e);
					cacheUsed = cacheUsed + e.size;
					notEnoughSpace = false;
				}
				else
				{
					Pair<Integer,Integer> tmp = removeCandidate.removeMin();
					tmpRemIndex = tmp.s.intValue();
					//mark for removal later
					removeIndexes.add(tmpRemIndex);
					cacheUsed -= cache.get(tmpRemIndex).size;
				}
			}while(notEnoughSpace);
			
			
		}
	}

	public int getCacheHits(){return numCacheHits;}

	public int getTotalQueries(){return numTotalQueries;}
}
