package cacheReplacementTest;

import java.util.ArrayList;


public class runMain {

	public static void main(String[] args) throws Exception {
		final int [] all = {1,1,1};
//		final int [] t100 = {1,0,0};
		final int [] t010 = {0,1,0};
//		final int [] t001 = {0,0,1};
//		final int [] t110 = {1,1,0};
//		final int [] t101 = {1,0,1};
//		final int [] t011 = {0,0,1};
		//		int numRoadGraphNodes, int numQueries, int cacheSize, 
		//		boolean fixedNumCacheBuckets, boolean skewedData, boolean useOptimalSubstructure, 
		//		boolean useNodeScore, boolean useHitScore, boolean useGaussian, 
		//		double sigma, int activeTests,String testName)

//		//CacheSize test
		final String cacheTestG025 = "cacheTest-G025.test1";
//
		( new Thread() {
			public void run() {
				RoadGraph g = RoadGraph.getMapObject();
				ArrayList<Integer> result = g.dijkstraSSSP(2,7);
				for(Integer r : result)
				{
					System.out.println(r);
				}
				


				int i = 8;
				i = i+2;
//				new testObj(10000, 100000, 100, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, true, true,0.25,all, cacheTestG025).runTest();//100 pct cache size
			}
		} ).start();
//
//		final String cacheTestNHSG025 = "cacheTestNoHitScore-G025.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, false, true,0.25,t010, cacheTestNHSG025).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNNSG025 = "cacheTestNoNodeScore-G025.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,false, true, true,0.25,t010, cacheTestNNSG025).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		//		final String cacheTest50 = "cacheTest50-"+Calendar.DAY_OF_YEAR+"-"+Calendar.HOUR+".test";
//		//		( new Thread() {
//		//			public void run() {	
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//				new testObj(10000, 100000, 5000, false, false, true, true, true, true,0.25,all, cacheTest50).runTest();//50 pct cache size
//		//			}
//		//		} ).start();
//
//		//map size test
//		final String mapSize = "mapSize-"+Calendar.DAY_OF_YEAR+"-"+Calendar.HOUR+".test";
//		( new Thread() {
//			public void run() {		
//				new testObj(10, 100000, 1, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//				new testObj(100, 100000, 10, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//				new testObj(1000, 100000, 100, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//				new testObj(100000, 100000, 10000, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//				new testObj(1000000, 100000, 100000, false, false, true,true, true, true,0.25,all, mapSize).runTest();
//			}
//		} ).start();
//
//		//number of queries before convergence (of cache hit rate) test
//		final String queryTestG025 = "querySize-G025.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 10, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//				new testObj(10000, 100, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//				new testObj(10000, 1000, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//				new testObj(10000, 10000, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//				new testObj(10000, 100000, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//				new testObj(10000, 1000000, 1000, false, false, true, true, true, true,0.25,all, queryTestG025).runTest();
//			}
//		} ).start();
//
//		final String cacheTestG050 = "cacheTest-G050.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, true, true,0.50,all, cacheTestG050).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNHSG050 = "cacheTestNoHitScore-G050.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, false, true,0.50,t010, cacheTestNHSG050).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNNSG050 = "cacheTestNoNodeScore-G050.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,false, true, true,0.50,t010, cacheTestNNSG050).runTest();//100 pct cache size
//			}
//		} ).start();
//
//
//
//		//number of queries before convergence (of cache hit rate) test
//		final String queryTestG050 = "querySize-G050.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 10, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//				new testObj(10000, 100, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//				new testObj(10000, 1000, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//				new testObj(10000, 10000, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//				new testObj(10000, 100000, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//				new testObj(10000, 1000000, 1000, false, false, true, true, true, true,0.50,all, queryTestG050).runTest();
//			}
//		} ).start();
//		
//		
//		final String cacheTestG100 = "cacheTest-G100.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, true, true,1,all, cacheTestG100).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNHSG100 = "cacheTestNoHitScore-G100.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, false, true,1,t010, cacheTestNHSG100).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNNSG100 = "cacheTestNoNodeScore-G100.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,false, true, true,1,t010, cacheTestNNSG100).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		//number of queries before convergence (of cache hit rate) test
//		final String queryTestG100 = "querySize-G100.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 10, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//				new testObj(10000, 100, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//				new testObj(10000, 1000, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//				new testObj(10000, 10000, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//				new testObj(10000, 100000, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//				new testObj(10000, 1000000, 1000, false, false, true, true, true, true,1,all, queryTestG100).runTest();
//			}
//		} ).start();
//		
//		final String cacheTestG200 = "cacheTest-G200.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, true, true,2,all, cacheTestG200).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNHSG200 = "cacheTestNoHitScore-G200.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, false, true,2,t010, cacheTestNHSG200).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNNSG200 = "cacheTestNoNodeScore-G200.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,false, true, true,2,t010, cacheTestNNSG200).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		//number of queries before convergence (of cache hit rate) test
//		final String queryTestG200 = "querySize-G200.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 10, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//				new testObj(10000, 100, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//				new testObj(10000, 1000, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//				new testObj(10000, 10000, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//				new testObj(10000, 100000, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//				new testObj(10000, 1000000, 1000, false, false, true, true, true, true,2,all, queryTestG200).runTest();
//			}
//		} ).start();
//		
//		final String cacheTestG400 = "cacheTest-G400.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, true, true,4,all, cacheTestG400).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNHSG400 = "cacheTestNoHitScore-G400.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,true, false, true,4,t010, cacheTestNHSG400).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		final String cacheTestNNSG400 = "cacheTestNoNodeScore-G400.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 100000, 100, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//1 pct cache size		  
//				new testObj(10000, 100000, 200, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//2 pct cache size	  
//				new testObj(10000, 100000, 500, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//5 pct cache size	  
//				new testObj(10000, 100000, 1000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//10 pct cache size  
//				new testObj(10000, 100000, 2000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//20 pct cache size
//				new testObj(10000, 100000, 3000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//30 pct cache size
//				new testObj(10000, 100000, 4000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//40 pct cache size
//				new testObj(10000, 100000, 5000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//50 pct cache size
//				new testObj(10000, 100000, 6000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//60 pct cache size
//				new testObj(10000, 100000, 7000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//70 pct cache size
//				new testObj(10000, 100000, 8000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//80 pct cache size
//				new testObj(10000, 100000, 9000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//90 pct cache size
//				new testObj(10000, 100000, 10000, false, false, true,false, true, true,4,t010, cacheTestNNSG400).runTest();//100 pct cache size
//			}
//		} ).start();
//
//		//number of queries before convergence (of cache hit rate) test
//		final String queryTestG400 = "querySize-G400.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 10, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//				new testObj(10000, 100, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//				new testObj(10000, 1000, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//				new testObj(10000, 10000, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//				new testObj(10000, 100000, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//				new testObj(10000, 1000000, 1000, false, false, true, true, true, true,4,all, queryTestG400).runTest();
//			}
//		} ).start();

//		final String GaussianEffect20pctCache = "GaussianEffect-20pctCache.test";
//		final String GaussianEffectNS20pctCache = "GaussianEffectNS-20pctCache.test";
//		final String GaussianEffectHS20pctCache = "GaussianEffectHS-20pctCache.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 1000000, 2000, false, false, true, true, true, true,0.25,all, GaussianEffect20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true, true, true, true,0.5,all, GaussianEffect20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true, true, true, true,1,all, GaussianEffect20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true, true, true, true,2,all, GaussianEffect20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true, true, true, true,4,all, GaussianEffect20pctCache).runTest();
//			}
//		} ).start();
//		( new Thread() {
//			public void run() {			
//				new testObj(10000, 1000000, 2000, false, false, true,true, false, true,0.25,t010, GaussianEffectNS20pctCache).runTest();	
//				new testObj(10000, 1000000, 2000, false, false, true,true, false, true,0.5,t010, GaussianEffectNS20pctCache).runTest();	
//				new testObj(10000, 1000000, 2000, false, false, true,true, false, true,1,t010, GaussianEffectNS20pctCache).runTest();	
//				new testObj(10000, 1000000, 2000, false, false, true,true, false, true,2,t010, GaussianEffectNS20pctCache).runTest();	
//				new testObj(10000, 1000000, 2000, false, false, true,true, false, true,4,t010, GaussianEffectNS20pctCache).runTest();	
//			}
//		} ).start();
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 1000000, 2000, false, false, true,false, true, true,0.25,t010, GaussianEffectHS20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true,false, true, true,0.5,t010, GaussianEffectHS20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true,false, true, true,1,t010, GaussianEffectHS20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true,false, true, true,2,t010, GaussianEffectHS20pctCache).runTest();
//				new testObj(10000, 1000000, 2000, false, false, true,false, true, true,4,t010, GaussianEffectHS20pctCache).runTest();
//			}
//		} ).start();
//
//		final String GaussianEffect50pctCache = "GaussianEffect-50pctCache.test";
//		final String GaussianEffectNS50pctCache = "GaussianEffectNS-50pctCache.test";
//		final String GaussianEffectHS50pctCache = "GaussianEffectHS-50pctCache.test";
//		( new Thread() {
//			public void run() {
//				new testObj(10000, 1000000, 5000, false, false, true, true, true, true,0.25,all, GaussianEffect50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true, true, true, true,0.5,all, GaussianEffect50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true, true, true, true,1,all, GaussianEffect50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true, true, true, true,2,all, GaussianEffect50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true, true, true, true,4,all, GaussianEffect50pctCache).runTest();
//			}
//		} ).start();
//		( new Thread() {
//					public void run() {			
//				new testObj(10000, 1000000, 5000, false, false, true,true, false, true,0.25,t010, GaussianEffectNS50pctCache).runTest();	
//				new testObj(10000, 1000000, 5000, false, false, true,true, false, true,0.5,t010, GaussianEffectNS50pctCache).runTest();	
//				new testObj(10000, 1000000, 5000, false, false, true,true, false, true,1,t010, GaussianEffectNS50pctCache).runTest();	
//				new testObj(10000, 1000000, 5000, false, false, true,true, false, true,2,t010, GaussianEffectNS50pctCache).runTest();	
//				new testObj(10000, 1000000, 5000, false, false, true,true, false, true,4,t010, GaussianEffectNS50pctCache).runTest();	
//					}
//				} ).start();
//		( new Thread() {
//					public void run() {
//				new testObj(10000, 1000000, 5000, false, false, true,false, true, true,0.25,t010, GaussianEffectHS50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true,false, true, true,0.5,t010, GaussianEffectHS50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true,false, true, true,1,t010, GaussianEffectHS50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true,false, true, true,2,t010, GaussianEffectHS50pctCache).runTest();
//				new testObj(10000, 1000000, 5000, false, false, true,false, true, true,4,t010, GaussianEffectHS50pctCache).runTest();
//			}
//		} ).start();

	}
}

