

package recommender;

import java.util.ArrayList;
import java.util.HashMap;

class User
{
	public int userID;
	public HashMap<Integer, Integer> Ratings = new HashMap<Integer, Integer>();               		//<movieID, rating>
	public HashMap<Integer, ArrayList<Integer>> Tests = new HashMap<Integer, ArrayList<Integer>>(); //<movieID, <rating, guess>>
	public HashMap<Integer, Double>  Sims = new HashMap<Integer, Double>();            				//<userID, similarity>
	public double ratingAverage;

	public User(int id)
	{
		this.userID = id;
	}


}
