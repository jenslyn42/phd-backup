package recommender;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

class Program
{
	public static void Main(String[] args)
	{

		Boolean useCategories = true;

		try{

			BufferedWriter report = new BufferedWriter(new FileWriter("Report.txt", true)); //true=append


			ArrayList<ArrayList<Integer>> mTests = new ArrayList<ArrayList<Integer>>(); //<userID,movieID>
			//<categoryID, HashMap<movieID, Movie object>>
			HashMap<Integer, HashMap<Integer, Movie>> mMovies = new HashMap<Integer, HashMap<Integer, Movie>>();
			//HashMap<movieID, categoryID>
			HashMap<Integer, Integer> movieCat = new HashMap<Integer, Integer>();
			loadMovies(mMovies, movieCat, useCategories);
			HashMap<Integer, User> mUsers = new HashMap<Integer, User>();		        //<userID,User>
			loadUsers(mUsers, mMovies, movieCat, useCategories);	                                //create Users from ratings.training
			loadTests(mUsers, mTests, useCategories);
			processUsers(mUsers);                                                       //process Users statistics
			processMovies(mMovies, useCategories);                                  //process Movies statistics
			setSimilarity(mUsers, report);                                          //find Similar Users
			predictPCC(mUsers, mTests, mMovies);                                    //make predictions

			report.close();
		}
		catch (Exception e) {
			System.err.println("--------Error---------\n"+e.getMessage()+"\n\n");
		}
	}


/**
 * Fill in predicted ratings based on similar users and user/movie trends
 * 
 * @param mUsers map of users 
 * @param mTests
 * @param mMovies
 * @throws IOException
 */
	static void predictPCC(HashMap<Integer, User> mUsers, ArrayList<ArrayList<Integer>> mTests ,HashMap<Integer, HashMap<Integer, Movie>> mMovies) throws IOException
	{	
		System.out.println("Calculating Test Case Predictions: ");
		int testLine = 0;

		BufferedWriter testOut = null;                								 //test Output file			
		try {
			testOut = new BufferedWriter(new FileWriter("testOut.txt", true)); //true=append
		} 
		catch (IOException e) {System.err.println("can't write to testOut.txt: \n" + e.getMessage()); }

		for (ArrayList<Integer> Test : mTests)                                       //each mTests(<userID,movieID>)
		{
			User testUser = mUsers.get(Test.get(0));
			double numerator = 0.0;
			double denominator = 0.0;
			testLine++;

			//			for (Map.Entry<String, Integer> entry : nameAndAges.entrySet())
			//				for ( String key : h.keySet() )

			for (Map.Entry<Integer, Double> Sim : testUser.Sims.entrySet())               //each User.Sims(<userID, Similarity>)
			{
				if (mUsers.get(Sim.getKey()).Ratings.containsKey(Test.get(1)))                //add to sum weighted values
				{
					numerator += (Sim.getValue() * (mUsers.get(Sim.getKey()).Ratings.get(Test.get(1)) - mUsers.get(Sim.getKey()).ratingAverage));
					denominator += Math.abs(Sim.getValue());
				}
			}
			double dPrediction = 0.0;
			if ((numerator != 0.0) && (denominator != 0.0))                         //prediction based on similar users (User.sims)
			{
				dPrediction = (testUser.ratingAverage + (numerator / denominator));
				testOut.write((int)Math.round(dPrediction));
			}
			else
			{                                                                       //prediction based on User.ratingAverage and Movie.ratingAverage
				double noSims = 0.0;
				if (mMovies.get(Test.get(1)).get(0).ratingAverage > 0)
				{
					noSims = (((2 * testUser.ratingAverage) + mMovies.get(Test.get(1)).get(0).ratingAverage) / 3);
				}
				else
				{
					noSims = testUser.ratingAverage;
				}
				testOut.write((int)Math.round(noSims));
			}

			//report progress
			if (testLine % 20000 == 0)
			{
				System.out.println("*");
			}

		}
		testOut.close();
		System.out.println("\n\t" + testLine + " Test Lines processed");
	}


	/**
	 * 
	 * 
	 * @param mUsers
	 * @param report
	 * @throws IOException
	 */
	static void setSimilarity(HashMap<Integer, User> mUsers, BufferedWriter report) throws IOException
	{
		int maxThresh = 7500;    //100                                                   //min-max number user's movie rankings
		int minThresh = 60;         //3
		double minSimScore = 0.01;  //.2                                                 //min Similarity score
		int minNumSims = 35;         //10                                                //min common movies rated

		System.out.println("Calculating User Similarities: ");
		report.write("Similarity Thresholds");
		report.write("\tUser Rankings: max=" + maxThresh + ", min=" + minThresh);
		report.write("\tSimilarities: min#=" + minNumSims + ", minScore=" + minSimScore + "\n");

		//Get Users within Thresholds
		ArrayList<User> Users = new ArrayList<User>();
		for (User mUser : mUsers.values())                                           //each mUser(<userID,User>)
		{
			if ((mUser.Ratings.size() > maxThresh) || (mUser.Ratings.size() < minThresh))
				continue;
			else
				Users.add(mUser);
		}

		int userCounter = 0;
		int numUsers = Users.size();
		if (numUsers > 1)
		{
			for (int x=0; x<numUsers; x++)                                          //loop through Valid users
			{
				double numerator = 0.0;
				double denominator1 = 0.0;
				double denominator2 = 0.0;
				User One = mUsers.get(Users.get(x).userID);
				int numRatings1 = One.Ratings.size();
				for (int y=(x+1); y<numUsers; y++)                                  //compare with rest of list
				{
					int numSims = 0;
					User Two = mUsers.get(Users.get(y).userID);
					int numRatings2 = Two.Ratings.size();
					for (Map.Entry<Integer, Integer> movie1 : One.Ratings.entrySet())          //each user1.Ratings(<movieID, rating>)
					{
						if (Two.Ratings.containsKey(movie1.getKey()))                    //other rated movie
						{                         
							numSims++;                                              //bump number of similar movies
							double oneScore = (double)movie1.getValue() - One.ratingAverage;             //=(Ryo(x) - Ryo(Ave))
							double twoScore = (double)Two.Ratings.get(movie1.getKey()) - Two.ratingAverage;  //=(Ry(x) - Ry(Ave))
							numerator += (twoScore * oneScore);
							denominator1 += Math.pow(oneScore, 2);
							denominator2 += Math.pow(twoScore, 2);
						}
					}
					if (numSims > minNumSims)                                       //keep Users with # similarities above threshold
					{
						double simScore = 0.0;
						if ((numerator != 0.0) && (denominator1 != 0.0) && (denominator2 != 0.0))
						{
							simScore = ( numerator / ( Math.sqrt(denominator1) * Math.sqrt(denominator2)));
						}
						if (Math.abs(simScore) >= minSimScore)                      //keep Users with similarity above threshold
						{
							double simScore1 = (simScore * numRatings1 * numSims);
							double simScore2 = (simScore * numRatings2 * numSims);

							One.Sims.put(Two.userID, simScore2);                     //add to both User.Sims(<userID, Similarity>)
							Two.Sims.put(One.userID, simScore1);
						}
					} 
				}
				//progress
				userCounter++;
				if (userCounter % 200 == 0)
				{
					System.out.println("*");
				}
			}
		}
		System.out.println("\n\t" + (userCounter) + " similarities processed");
	}

	
	/**
	 * Process Movie information after all Ratings have been loaded
	 * 
	 * @param mMovies
	 * @param category
	 */
	static void processMovies(HashMap<Integer, HashMap<Integer, Movie>> mMovies, Boolean category)
	{
		System.out.println("Processing Users: ");
		int movieCounter = 0;

		if(category)
		{
			for(HashMap<Integer, Movie> cat : mMovies.values())
			{
				//the code in this loop is exactly the same as the loop after the 'else'
				for (Movie mMovie : cat.values())                                         //each mMovies(<movieID,Movie>)
				{
					int sumRating = -1;

					//find Movie.ratingAverage
					for (int rating : mMovie.Ratings)                                      //each mMovie.Ratings(<rating,rating,...>)
					{
						sumRating += rating;
					}
					mMovie.ratingAverage = 0.0;
					double tempAverage = ((double)sumRating / (double)mMovie.Ratings.size());
					if (tempAverage > mMovie.ratingAverage)
					{
						mMovie.ratingAverage = tempAverage;
					}

					//progress
					movieCounter++;
					if (movieCounter % 200 == 0)
					{
						System.out.println("*");
					}
				}
			}
		}else{
			for (Movie mMovie : mMovies.get(0).values())                                         //each mMovies(<movieID,Movie>)
			{
				int sumRating = -1;

				//find Movie.ratingAverage
				for (int rating : mMovie.Ratings)                                      //each mMovie.Ratings(<rating,rating,...>)
				{
					sumRating += rating;
				}
				mMovie.ratingAverage = 0.0;
				double tempAverage = ((double)sumRating / (double)mMovie.Ratings.size());
				if (tempAverage > mMovie.ratingAverage)
				{
					mMovie.ratingAverage = tempAverage;
				}

				//progress
				movieCounter++;
				if (movieCounter % 200 == 0)
				{
					System.out.println("*");
				}
			}
		}
		System.out.println("\n\t" + movieCounter + " Movies processed");
	}


	/**
	 * Process User information after all Ratings have been loaded
	 * 
	 * @param mUsers 
	 */
	static void processUsers(HashMap<Integer, User> mUsers)
	{
		System.out.println("Processing Users: ");
		int userCounter = 0;
		int ratingCount = 0;

		for (User mUser : mUsers.values())                                           //each mUsers(<userID,User>)
		{
			int sumRating = 0;
			int ratingCounter = 0;

			//find User.ratingAveraqe
			for(Integer rating : mUser.Ratings.values())                                //each User.Ratings(<movieID, rating>)
			{
				sumRating += rating;
				ratingCounter++;
			}
			mUser.ratingAverage = ((double)sumRating / (double)mUser.Ratings.size());

			//progress
			if (ratingCounter > ratingCount)
			{
				ratingCount = ratingCounter;
			}
			userCounter++;
			if (userCounter % 500 == 0)
			{
				System.out.println("*");
			}

		}
		System.out.println("\n\t" + userCounter + " Users processed, Max # ratings: " + ratingCount);
	}


	
	/**
	 * Parse "ratings.test, populate mUser with info, and populate mTest with info
	 * 
	 * @param mUsers map <user id -> user object>
	 * @param mTests List of Lists, each containing user id, movie id
	 * @param category weather to categories or not
	 */
	static void loadTests(HashMap<Integer, User> mUsers, ArrayList<ArrayList<Integer>> mTests, Boolean category)
	{
		System.out.println("Loading Test Cases: ");

		//Read from file of movieID, userID

		ArrayList<ArrayList<String>> testList = FileParser.mapFile("ratings.test");


		int movieID;
		int userID;

		for(int i = 0; i < testList.get(0).size(); i++)
		{
			movieID = Integer.parseInt(testList.get(0).get(i));
			userID = Integer.parseInt(testList.get(1).get(i));

			//make new user if not in mUsers
			if (!mUsers.containsKey(userID))
			{
				User tempUser = new User(userID);                       //make user
				mUsers.put(userID, tempUser);                           //add user to mUsers
			}

			ArrayList<Integer> tmpTestsList = new ArrayList<Integer>();
			tmpTestsList.add(userID);
			tmpTestsList.add(movieID);
			mTests.add(tmpTestsList);

		}
		System.out.println("\n\t" + testList.get(0).size() + " Test Lines Read");
	}

	
	/**
	 * Parse "ratings.training" via random bool, create User objects and add to mUsers, add ratings to mMovies
	 * 
	 * @param mUsers map <user id -> user object>
	 * @param mMovies map <movie id -> movie objects>
	 * @param movieCategoryList <movie id -> id of movie category >
	 * @param category whether to use categories
	 */
	static void loadUsers(HashMap<Integer, User> mUsers, HashMap<Integer, HashMap<Integer, Movie>> mMovies, HashMap<Integer, Integer> movieCategoryList, Boolean category)
	{
		System.out.println("Loading User Movie Ratings: ");

		//Read from file of movieID, userID, Rating

		ArrayList<ArrayList<String>> trainingList = FileParser.mapFile("ratings.training");

		int movieID = 0;
		int userID = 0; 
		int rating = 0;
		
		

		for(int i = 0; i < trainingList.get(0).size(); i++)
		{
			movieID = Integer.parseInt(trainingList.get(0).get(i));
			userID = Integer.parseInt(trainingList.get(1).get(i));
			rating = Integer.parseInt(trainingList.get(2).get(i));

			//make new user if not in mUsers
			if (!mUsers.containsKey(userID))
			{
				User tempUser = new User(userID);                       //make user
				mUsers.put(userID, tempUser);                           //add user to mUsers
			}
			
			mUsers.get(userID).Ratings.put(movieID, rating);            //add rating to User.Ratings(<movieID, rating>)
			
			if(category)
				mMovies.get(movieCategoryList.get(movieID)).get(movieID).Ratings.add(rating);     //add rating to Movie.Ratings(<rating,rating,...)
			else
				mMovies.get(0).get(movieID).Ratings.add(rating);                   //add rating to Movie.Ratings(<rating,rating,...)
		}
		System.out.println("\n\t" + trainingList.get(0).size() + " User ratings");
	}



	/**
	 * Parse "movie_titles.txt", create Movie objects, then add them to mMovie dictionary
	 * 
	 * @param mMovies map of <id,movie objects>
	 * @param movieCategoryList movie id -> movie category id 
	 * @param category whether to use categories
	 */
	static void loadMovies(HashMap<Integer, HashMap<Integer, Movie>> mMovies, HashMap<Integer, Integer> movieCategoryList, Boolean category)
	{
		System.out.println("Loading Movie Info: ");

		ArrayList<ArrayList<String>> movieList = FileParser.mapFile("movie_titles.txt");
		int id;
		int year;
		int cat;
		String title;


		if(category)
		{
			//makes movie objects and adds them with their category to the movie list
			for(int i = 0; i < movieList.get(0).size(); i++)
			{
				id = Integer.parseInt(movieList.get(0).get(i));
				year = Integer.parseInt(movieList.get(1).get(i));
				title = movieList.get(2).get(i);
				cat = Integer.parseInt(movieList.get(3).get(i));

				Movie tempMovie = new Movie(id, year, title);       //Make movie
				tempMovie.categoryId = cat;
				
				if(!mMovies.containsKey(cat))
				{
					HashMap<Integer, Movie> tmpMovie = new HashMap<Integer, Movie>();
					tmpMovie.put(tempMovie.movieID, tempMovie);
					mMovies.put(cat, tmpMovie);
				}
				else{
					mMovies.get(cat).put(tempMovie.movieID, tempMovie);          //Add to mMovies
				}
				
				movieCategoryList.put(id, cat);
			}
		}
		else{
			//makes movie objects and adds them with same default category to the movie list
			HashMap<Integer, Movie> tmpMovie_ = new HashMap<Integer, Movie>();
			mMovies.put(0, tmpMovie_);
			
			for(int i = 0; i < movieList.get(0).size(); i++)
			{
				id = Integer.parseInt(movieList.get(0).get(i));
				year = Integer.parseInt(movieList.get(1).get(i));
				title = movieList.get(2).get(i);

				
				Movie tempMovie = new Movie(id, year, title);       //Make movie
				mMovies.get(0).put(tempMovie.movieID, tempMovie);          //Add to mMovies
			}
		}

		System.out.println(" - " + movieList.get(0).size() + " found");
	}

}