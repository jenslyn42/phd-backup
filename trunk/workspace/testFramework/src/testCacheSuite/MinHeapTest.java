/**
 * 
 */
package testCacheSuite;

import cacheReplacementTest.MinHeap;
import cacheReplacementTest.Pair;
import junit.framework.TestCase;

/**
 * @author administrator
 *
 */
public class MinHeapTest extends TestCase {

	/**
	 * @param name
	 */
	public MinHeapTest(String name) {
		super(name);
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#setUp()
	 */
	protected void setUp() throws Exception {
		super.setUp();
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#tearDown()
	 */
	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#MinHeap(cacheReplacementTest.Pair, int)}.
	 */
	public final void testMinHeap() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#insert(cacheReplacementTest.Pair)}.
	 */
	public final void testInsert() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#updateEntry(int, int)}.
	 */
	public final void testUpdateEntry() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#isEmpty()}.
	 */
	public final void testIsEmpty() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#print()}.
	 */
	public final void testPrint() {
		fail("Not yet implemented"); // TODO
	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#removeMin()}.
	 * @throws Exception 
	 */
	public final void testRemoveMin() throws Exception {
		MinHeap exp = new MinHeap(new Pair<Integer, Integer>(4,104), 10);
		exp.insert(new Pair<Integer, Integer>(13,113));
		exp.insert(new Pair<Integer, Integer>(15,115));		
		exp.insert(new Pair<Integer, Integer>(5,105));
		exp.insert(new Pair<Integer, Integer>(3,103));
		
		int minVal1 = exp.removeMin().t;
		assertEquals("A103",minVal1,103);
		
		int minVal2 = exp.removeMin().t;
		assertEquals("B104",minVal2,104);
		
		exp.insert(new Pair<Integer, Integer>(1,101));
		int minVal3 = exp.removeMin().t;
		assertEquals("C101",minVal3,101);
	
		exp.insert(new Pair<Integer, Integer>(10,110));
		int minVal4 = exp.removeMin().t;
		assertEquals("D105",minVal4,105);

	}

	/**
	 * Test method for {@link cacheReplacementTest.MinHeap#peekMin()}.
	 * @throws Exception 
	 */
	public final void testPeekMin() throws Exception {
		MinHeap exp = new MinHeap(new Pair<Integer, Integer>(4,104), 10);
		exp.insert(new Pair<Integer, Integer>(13,113));
		exp.insert(new Pair<Integer, Integer>(15,115));		
		exp.insert(new Pair<Integer, Integer>(5,105));
		exp.insert(new Pair<Integer, Integer>(3,103));
		
		int minVal = exp.peekMin().t;
		assertEquals("Root",minVal,103);
	}

}
