package testCacheSuite;

import junit.framework.Test;
import junit.framework.TestSuite;

public class controler {
	
	public static Test suite(){
    TestSuite suite = new TestSuite();
    suite.addTest(new testRoadGraph("Some test"));
	return suite;
}
}
