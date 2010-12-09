package cacheReplacementTest;

import java.security.SecureRandom;
import java.util.Random;

public class Queries {

	TestSettings ts;
	SecureRandom random;
	Random rand;

	public Queries(TestSettings ts)
	{
		this.ts = ts;
		Long tmp = new Long(1854775807);
		if(ts.isUseGaussian())
		{
			rand = new Random(tmp);

		}
			random = new SecureRandom();
			random.setSeed(tmp);
	}

	public Pair<Integer, Integer> generateNext(int rangeStart, int rangeEnd)
	{		
		if(ts.isUseGaussian())
		{		
			return new Pair<Integer, Integer>((int)gaussianInRange(ts.getSigma(), ts.getNumRoadGraphNodes()),(int)gaussianInRange(ts.getSigma(), ts.getNumRoadGraphNodes()));
		}
		else
			return new Pair<Integer, Integer>(rangeStart+Math.abs(random.nextInt((rangeEnd-rangeStart))+1),rangeStart+Math.abs(random.nextInt((rangeEnd-rangeStart))+1));
	}

	public Pair<Integer, Integer>[] generateList(int numGenerate, int rangeStart, int rangeEnd)
	{	
		Pair<Integer, Integer>[] qList = new Pair[numGenerate];

		for(int i=0; i<numGenerate;i++)
		{
			qList[i] = generateNext(rangeStart, rangeEnd);
		}

		return qList;
	}
	
	private double gaussianInRange(double sigma_, int numNodes) {
	       double v1,v2,s,x;
	       double sigma = sigma_ * numNodes;
	       do{
	       do {
	               v1 = rand.nextDouble();
	               if ((random.nextInt()%2) == 1) v1 = -v1;
	               v2 = rand.nextDouble();
	               if ((random.nextInt()%2) == 1) v2 = -v2;
	               s = v1*v1 + v2*v2;
	       } while (s >= 1.);
	       x = v1 * Math.sqrt( -2.0 * Math.log(s) / s);
	       //  x is normally distributed with mean 0 and sigma 1.
	       x = Math.abs(x * sigma);
	       }while(x >= numNodes || 0 > x);
	       return x;
	}
}
