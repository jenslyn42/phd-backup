package testCacheSuite;

import java.util.ArrayList;
import java.util.Collection;

import junit.framework.TestCase;
import junit.framework.TestSuite;

public class testRoadGraph extends TestCase  { 

	
    private Collection<Integer> collection;
    //RoadGraph rg;
    
    public testRoadGraph(
            String name) {
            super(name);
        }


    protected void setUp() {
    try {
		super.setUp();
	} catch (Exception e) {
		e.printStackTrace();
	}
       collection = new ArrayList<Integer>();
       collection.add(new Integer(3));
//       rg = RoadGraph.getMapObject();
       
    }

    protected void tearDown() {
    	try {
			super.tearDown();
		} catch (Exception e) {
			e.printStackTrace();
		}
	 collection.clear();
	 //rg = null;
    }

    public void testEmptyCollection(){
    	assertTrue(collection.isEmpty());
//	 assertTrue(rg.dijkstraSSSP(2, 5).containsAll(collection));
    }
    public void testEmptyCollection2(){
    	assertFalse(collection.isEmpty());
//	 assertTrue(rg.dijkstraSSSP(2, 5).containsAll(collection));
    }

    

	public static TestSuite suite(){
		return new TestSuite(testRoadGraph.class);
	}
	
}
