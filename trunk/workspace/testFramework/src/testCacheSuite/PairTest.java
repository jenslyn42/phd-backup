/**
 * 
 */
package testCacheSuite;

import cacheReplacementTest.Pair;
import junit.framework.TestCase;

/**
 * @author administrator
 *
 */
public class PairTest extends TestCase {

	Pair<Integer, Integer> tp1;
	Pair<Object, Integer> tp2;
	int s,t;
	/**
	 * @param name
	 */
	public PairTest(String name) {
		super(name);
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#setUp()
	 */
	protected void setUp() throws Exception {
		super.setUp();
		s=24; t=67;
		tp1 = new Pair<Integer, Integer>(s,t);
		tp2 = new Pair<Object, Integer>(new Object(),t);
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#tearDown()
	 */
	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * Test method for {@link cacheReplacementTest.Pair#Pair(java.lang.Object, java.lang.Object)}.
	 */
	public final void testPair() {
		assertNotNull(tp1);
		assertNotNull(tp2);
		assertTrue(tp1.getClass() == Pair.class);
		assertTrue(tp2.getClass() == Pair.class);
	}

	/**
	 * Test method for {@link cacheReplacementTest.Pair#equalsTarget(cacheReplacementTest.Pair)}.
	 */
	public final void testEqualsTarget() {
		assertTrue(tp1.equalsTarget(new Pair<Integer, Integer>(2,67)));
		assertTrue(tp2.equalsTarget(tp1));
	}

	/**
	 * Test method for {@link cacheReplacementTest.Pair#toString()}.
	 */
	public final void testToString() {
		String expected = "<"+s+","+t+">";
		
		assertEquals(tp1.toString(), expected);
	}

}
