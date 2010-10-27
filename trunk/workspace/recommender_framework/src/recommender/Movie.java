package recommender;

import java.util.ArrayList;


class Movie
{
	public int movieID;
	public int movieYear;
	public String movieTitle;
	public int categoryId = 0;
	public ArrayList<Integer> Ratings = new ArrayList<Integer>();             //<rating,rating,...>
	public double ratingAverage;
	public ArrayList<String> Categories = new ArrayList<String>();

	public Movie(int id, int year, String title)
	{
		this.movieID = id;
		this.movieYear = year;
		this.movieTitle = title;	
	}


}
