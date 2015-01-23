package utility;

import java.io.*;
import java.util.Vector;

import objects.Bot;
import objects.Map;

public class GameListGenerator 
{

	public static void GenerateGames(int rounds, Vector<Map> maps, Vector<Bot> bots) 
	{
		try 
		{
			FileWriter fstream = new FileWriter("games.txt");
			
			BufferedWriter out = new BufferedWriter(fstream);
			
			generateRoundRobin(rounds, maps, bots, out);
			
			out.write("");
			out.flush();
			out.close();
			
			System.out.println("Generation Complete");
			
		} 
		catch (Exception e) 
		{
			System.err.println("Error: " + e.getMessage());
		}
	}

	public static void generateRoundRobin(int rounds, Vector<Map> maps, Vector<Bot> bots, BufferedWriter out) throws IOException 
	{
		int gameID = 0;
		int roundNum = 0;
		
		for (int i = 0; i < rounds; i++) 
		{
			for(Map m : maps)
			{
				for (int j = 0; j < bots.size(); j++) 
				{
					for (int k = 0; k < bots.size(); k++) 
					{
						if (roundNum % 2 == 0) 
						{
							if (k > j) 
							{
								out.write(String.format("%7d %5d %20s %20s %35s", gameID, roundNum, bots.get(j).getName(), bots.get(k).getName(), m.getMapName()) + System.getProperty("line.separator"));
								gameID++;
							}
						} 
						else 
						{
							if (k < j) 
							{
								out.write(String.format("%7d %5d %20s %20s %35s", gameID, roundNum, bots.get(j).getName(), bots.get(k).getName(), m.getMapName()) + System.getProperty("line.separator"));
								gameID++;
							}
						}
					}
				}
				roundNum++;
			}
		}
	}

}
