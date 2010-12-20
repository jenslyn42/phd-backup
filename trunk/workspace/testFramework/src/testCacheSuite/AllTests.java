package testCacheSuite;

import junit.framework.Test;
import junit.framework.TestSuite;

public class AllTests {

	public static Test suite() {
		TestSuite suite = new TestSuite(AllTests.class.getName());
		//$JUnit-BEGIN$
		suite.addTestSuite(QueriesTest.class);
		suite.addTestSuite(FIFOTest.class);
		suite.addTestSuite(MinHeapTest.class);
		suite.addTestSuite(RoadGraphTest.class);
		suite.addTestSuite(PairTest.class);
		suite.addTestSuite(LRUTest.class);
		suite.addTestSuite(OSCTest.class);
		suite.addTestSuite(CacheItemTest.class);
		suite.addTestSuite(VertexTest.class);
		//$JUnit-END$
		return suite;
	}

}
