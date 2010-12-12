/**
 * 
 */
package testCacheSuite;

import cacheReplacementTest.RoadGraph;
import junit.framework.TestCase;

/**
 * @author Jenslyn
 *
 */
public class RoadGraphTest extends TestCase {

	RoadGraph rg;
	/**
	 * @param name
	 */
	public RoadGraphTest(String name) {
		super(name);
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#setUp()
	 */
	protected void setUp() throws Exception {
		super.setUp();
		rg = RoadGraph.getMapObject();
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#tearDown()
	 */
	protected void tearDown() throws Exception {
		super.tearDown();
		rg = null;
	}

	/**
	 * Test method for {@link cacheReplacementTest.RoadGraph#getMapObject()}.
	 */
	public final void testGetMapObject() {
		RoadGraph expected = RoadGraph.getMapObject();
		
		assertTrue(expected == rg);
	}

	/**
	 * Test method for {@link cacheReplacementTest.RoadGraph#getMap()}.
	 */
	public final void testGetMap() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.RoadGraph#dijkstraSSSP(int, int)}.
	 */
	public final void testDijkstraSSSP() {
//		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.RoadGraph#clone()}.
	 */
	public final void testClone() {
		fail("Not yet implemented"); // TODO
	}

}
