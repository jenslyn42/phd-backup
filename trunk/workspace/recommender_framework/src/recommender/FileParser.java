package recommender;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

public class FileParser {

//	public static ArrayList<ArrayList<String>> mapFileMsg(String filename, String msg, Boolean category)
//	{
//		System.out.println("Opening "+filename+"\n");
//		System.out.println(msg+"\n");
//		fileMap = mapFile(filename, category);
//		System.out.println(filename+" closed");
//		return fileMap;
//	}

	public static ArrayList<ArrayList<String>> mapFile(String filename)
	{
		String lineRead = null;
		String[] splitString = null;
		int lineCardinality = 0;
		ArrayList<ArrayList<String>> fileTokenHolder = new ArrayList<ArrayList<String>>();

		try{
			BufferedReader file = new BufferedReader(new FileReader(filename));

			if((lineRead = file.readLine()) != null)
			{
				while(lineRead.startsWith("@"))
					lineRead = file.readLine();
				splitString = lineRead.split(";");
				lineCardinality = splitString.length;

				//Set the cardinality of fileTokenHolder and add one element to each column 
				for(int i=0; i<lineCardinality; i++)
				{
					fileTokenHolder.add(new ArrayList<String>());
					fileTokenHolder.get(i).add(splitString[i]);
				}

				//Add all remaining elements in the file to fileTokenHolder
				while ((lineRead = file.readLine()) != null)
				{
					splitString = lineRead.split(";");         //Split line delimited by simi-colon

					for(int i=0; i<lineCardinality; i++)
						fileTokenHolder.get(i).add(splitString[i]);
				}
			}
			file.close();
			return fileTokenHolder;

		}catch(IOException e){
			System.err.println("IOException while reading file \""+filename+"\": "+e.getMessage()+"\n");	
		}
		return null;
	}
}
