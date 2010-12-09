package cacheReplacementTest;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;

public class testObj {

	private TestSettings ts = null;

	public testObj(int numRoadGraphNodes, int numQueries, int cacheSize, 
			boolean fixedNumCacheBuckets, boolean skewedData, boolean useOptimalSubstructure, 
			boolean useNodeScore, boolean useHitScore, boolean useGaussian, double sigma, 
			int[] activeTests, String testName)
	{
		ts = new TestSettings(numRoadGraphNodes, numQueries, cacheSize, fixedNumCacheBuckets,
				skewedData, useOptimalSubstructure, useNodeScore, useHitScore, useGaussian, 
				sigma, activeTests, testName);
	}

	public void runTest()
	{
		int [] tests = ts.getActiveTests();
		//	RoadGraph rg = new RoadGraph(numRoadGraphNodes);
		//LinkedList<Integer> ll = rg.getMap();
		Queries qGenerator = new Queries(ts);
		Pair<Integer, Integer> q; 
		LRU test1 = new LRU(ts);
		OSC test2 = new OSC(ts);
		FIFO test3 = new FIFO(ts);

		int i = ts.getNumQueries();

		while(i != 0){
			q = qGenerator.generateNext(ts.getQueryRangeStart(), ts.getQueryRangeEnd());

			if(i%10000 == 0)
				//				System.out.println("("+q.s+","+q.t+") "+i);
				System.out.print("*");
			if(tests[0] == 1)
				test1.readQuery(q);
			if(tests[1] == 1)
				test2.readQuery(q);
			if(tests[2] == 1)
				test3.readQuery(q);
			i--;
		}

		try{
			// Create file 
			File f = new File(ts.getTestName());
			FileWriter fstream;
			boolean fileExist = f.exists();
			if(!fileExist)		
				fstream = new FileWriter(f, false);
			else
				fstream = new FileWriter(f, true);

			BufferedWriter out = new BufferedWriter(fstream);

			if(!fileExist){
				out.write("TestName\tMapSize\tnumQueries\tCache Size\tCache pct.\tqueryRange-S\tqueryRange-E\t" +
						"FixedNumBuckets\tSkewed Data\tUseOptimalSubstructure\tuseGaussian\tSigma(Gaussian)\t" +
						"LRU-CacheHits\tOSC-CacheHits\tFIFO-CacheHits\t" +
						"LRU-CacheHitrate\t"+
						"OSC-CacheHitrate\t" +
						"FIFO-CacheHitrate");
				out.newLine();
			}
			out.write(ts.getTestName()+"\t"+ts.getNumRoadGraphNodes()+"\t"+ts.getNumQueries()+"\t"+ts.getCacheSize()+"\t"+
					(ts.getCacheSize()*100)/ts.getNumRoadGraphNodes()+"\t"+ts.getQueryRangeStart()+"\t"+
					ts.getQueryRangeEnd()+"\t"+ts.isFixedNumCacheBuckets()+"\t"+ts.isSkewedData()+"\t"+
					ts.isUseOptimalSubstructure()+ts.isUseGaussian()+"\t"+ts.getSigma()+"\t"+
					"\t"+test1.getCacheHits()+"\t"+test2.getCacheHits()+"\t"+test3.getCacheHits()+"\t"+
					((double)test1.getCacheHits()*100)/(double)test1.getTotalQueries()+"\t"+
					((double)test2.getCacheHits()*100)/(double)test2.getTotalQueries()+"\t"+
					((double)test3.getCacheHits()*100)/(double)test3.getTotalQueries());
			out.newLine();
			out.close();
		}catch (Exception e){//Catch exception if any
			System.err.println("Write Error: " + e.getMessage());
		}

		System.out.println("\n------------- Test Parameters --------------\n"+
				"Test Name: "+ ts.getTestName() + "\n"+
				"Map Size: "+ ts.getNumRoadGraphNodes() + "\n"+
				"Number of Queries: "+ ts.getNumQueries() + "\n"+
				"Cache Size :" + ts.getCacheSize() + " (" + (ts.getCacheSize()*100)/ts.getNumRoadGraphNodes() + "% of total map)" + "\n"+
				"Start/end of query range: " + ts.getQueryRangeStart()+" / "+ts.getQueryRangeEnd() + "\n"+
				"Fixed number of buckets: " + ts.isFixedNumCacheBuckets() + "\n"+
				"Use optimal substructure: " + ts.isUseOptimalSubstructure() + "\n");

		System.out.println("LRU total queries: "+test1.getTotalQueries()+"\nCachehits: "+test1.getCacheHits());
		System.out.println("OSC total queries: "+test2.getTotalQueries()+"\nCachehits: "+test2.getCacheHits());
		System.out.println("FIFO total queries: "+test3.getTotalQueries()+"\nCachehits: "+test3.getCacheHits());
	}
}
