package cacheReplacementTest;

public class Pair<S, T>
{
	public final S s;
	public final T t;

	public Pair(S s, T t)
	{			
		this.s = s;
		this.t = t;
	}

	public boolean equalsTarget(Pair<Integer, Integer> p)
	{
		boolean isEqual = false;

		if (p.t == this.t)
			isEqual = true;

		return isEqual;
	}
	
    public String toString( ) {
        return "<"+s+","+t+">";
    }
}
