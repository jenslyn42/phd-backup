package cacheReplacementTest;

public class TestSettings {
	private String testName;
	private int numRoadGraphNodes;
	private int numQueries;
	private int cacheSize;
	private int queryRangeStart;
	private int queryRangeEnd;
	private int[] activeTests;
	
	private double sigma;
	//is implemented as false
	private boolean fixedNumCacheBuckets;
	//is NOT implemented
	private boolean skewedData;
	//is implemented as true
	private boolean useOptimalSubstructure;
	private boolean useNodeScore;
	private boolean useHitScore;
	private boolean useGaussian; 


	public TestSettings(int numRoadGraphNodes, int numQueries, int cacheSize, 
			boolean fixedNumCacheBuckets, boolean skewedData, boolean useOptimalSubstructure, 
			boolean useNodeScore, boolean useHitScore, boolean useGaussian, 
			double sigma, int[] activeTests,String testName)
	{
		this.setNumRoadGraphNodes(numRoadGraphNodes);
		this.setNumQueries(numQueries);
		this.setCacheSize(cacheSize);
		this.setQueryRangeStart(0);
		this.setQueryRangeEnd(numRoadGraphNodes);
		this.setFixedNumCacheBuckets(fixedNumCacheBuckets);
		this.setSkewedData(skewedData);
		this.setUseOptimalSubstructure(useOptimalSubstructure);
		this.useNodeScore = useNodeScore;
		this.useHitScore = useHitScore;
		this.useGaussian = useGaussian;
		this.sigma = sigma;
		this.activeTests = activeTests;
		this.setTestName(testName);
	}
	
	public TestSettings(TestSettings ts)
	{
		this.numRoadGraphNodes = ts.getNumRoadGraphNodes();
		this.numQueries = ts.getNumQueries();
		this.cacheSize = ts.getCacheSize();
		this.queryRangeStart = ts.getQueryRangeStart();
		this.queryRangeEnd = ts.getQueryRangeEnd();
		this.fixedNumCacheBuckets = ts.isFixedNumCacheBuckets();
		this.skewedData = ts.isSkewedData();
		this.useOptimalSubstructure = ts.isUseOptimalSubstructure();
		this.useNodeScore = ts.isUseNodeScore();
		this.useHitScore = ts.isUseHitScore();
		this.useGaussian = ts.isUseGaussian();
		this.sigma = ts.getSigma();
		this.activeTests = ts.getActiveTests();
		this.testName = ts.getTestName();
	}

	/**
	 * @param useNodeScore the useNodeScore to set
	 */
	public void setUseNodeScore(boolean useNodeScore) {
		this.useNodeScore = useNodeScore;
	}

	/**
	 * @return the useNodeScore
	 */
	public boolean isUseNodeScore() {
		return useNodeScore;
	}

	/**
	 * @param useHitScore the useHitScore to set
	 */
	public void setUseHitScore(boolean useHitScore) {
		this.useHitScore = useHitScore;
	}

	/**
	 * @return the useHitScore
	 */
	public boolean isUseHitScore() {
		return useHitScore;
	}

	/**
	 * @param useOptimalSubstructure the useOptimalSubstructure to set
	 */
	public void setUseOptimalSubstructure(boolean useOptimalSubstructure) {
		this.useOptimalSubstructure = useOptimalSubstructure;
	}

	/**
	 * @return the useOptimalSubstructure
	 */
	public boolean isUseOptimalSubstructure() {
		return useOptimalSubstructure;
	}

	/**
	 * @param skewedData the skewedData to set
	 */
	public void setSkewedData(boolean skewedData) {
		this.skewedData = skewedData;
	}

	/**
	 * @return the skewedData
	 */
	public boolean isSkewedData() {
		return skewedData;
	}

	/**
	 * @param fixedNumCacheBuckets the fixedNumCacheBuckets to set
	 */
	public void setFixedNumCacheBuckets(boolean fixedNumCacheBuckets) {
		this.fixedNumCacheBuckets = fixedNumCacheBuckets;
	}

	/**
	 * @return the fixedNumCacheBuckets
	 */
	public boolean isFixedNumCacheBuckets() {
		return fixedNumCacheBuckets;
	}

	/**
	 * @param queryRangeEnd the queryRangeEnd to set
	 */
	public void setQueryRangeEnd(int queryRangeEnd) {
		this.queryRangeEnd = queryRangeEnd;
	}

	/**
	 * @return the queryRangeEnd
	 */
	public int getQueryRangeEnd() {
		return queryRangeEnd;
	}

	/**
	 * @param queryRangeStart the queryRangeStart to set
	 */
	public void setQueryRangeStart(int queryRangeStart) {
		this.queryRangeStart = queryRangeStart;
	}

	/**
	 * @return the queryRangeStart
	 */
	public int getQueryRangeStart() {
		return queryRangeStart;
	}

	/**
	 * @param cacheSize the cacheSize to set
	 */
	public void setCacheSize(int cacheSize) {
		this.cacheSize = cacheSize;
	}

	/**
	 * @return the cacheSize
	 */
	public int getCacheSize() {
		return cacheSize;
	}

	/**
	 * @param numRoadGraphNodes the numRoadGraphNodes to set
	 */
	public void setNumRoadGraphNodes(int numRoadGraphNodes) {
		this.numRoadGraphNodes = numRoadGraphNodes;
	}

	/**
	 * @return the numRoadGraphNodes
	 */
	public int getNumRoadGraphNodes() {
		return numRoadGraphNodes;
	}

	/**
	 * @param testName the testName to set
	 */
	public void setTestName(String testName) {
		this.testName = testName;
	}

	/**
	 * @return the testName
	 */
	public String getTestName() {
		return testName;
	}

	/**
	 * @param numQueries the numQueries to set
	 */
	public void setNumQueries(int numQueries) {
		this.numQueries = numQueries;
	}

	/**
	 * @return the numQueries
	 */
	public int getNumQueries() {
		return numQueries;
	}

	/**
	 * @param useGaussian the useGaussian to set
	 */
	public void setUseGaussian(boolean useGaussian) {
		this.useGaussian = useGaussian;
	}

	/**
	 * @return the useGaussian
	 */
	public boolean isUseGaussian() {
		return useGaussian;
	}

	/**
	 * @param activeTests the activeTests to set
	 */
	public void setActiveTests(int[] activeTests) {
		this.activeTests = activeTests;
	}

	/**
	 * @return the activeTests
	 */
	public int[] getActiveTests() {
		return activeTests;
	}

	/**
	 * @param sigma the sigma to set
	 */
	public void setSigma(double sigma) {
		this.sigma = sigma;
	}

	/**
	 * @return the sigma
	 */
	public double getSigma() {
		return sigma;
	}
}
