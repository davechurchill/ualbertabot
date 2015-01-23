package utility;

import java.util.*;
import java.io.*;

public class ResultsGraph
{

	static HashMap<String, Integer> gamesPlayed = new HashMap<String, Integer>();
	static HashMap<String, Integer> gamesWon = new HashMap<String, Integer>();
	static HashMap<String, Integer> elo = new HashMap<String, Integer>();

	static HashMap<String, Vector<Double>> winHistory = new HashMap<String, Vector<Double>>();
	static HashMap<String, Vector<Integer>> eloHistory = new HashMap<String, Vector<Integer>>();
	
	static String[] botNames = {"Aiur", "BTHAI", "ICEStarCraft", "Nova", "Skynet", "UAlbertaBot", "Xelnaga", "Ximp"};
	
	static int numBots = botNames.length;
	
	
	public static void main(String[] args) throws Exception
	{
		for (int i=0; i<numBots; ++i)
		{
			gamesPlayed.put(botNames[i], 0);
			gamesWon.put(botNames[i], 0);
			elo.put(botNames[i], 1600);
			winHistory.put(botNames[i], new Vector<Double>() );
			eloHistory.put(botNames[i], new Vector<Integer>() );
		}
	
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		String line = br.readLine();

		int previousRound = 1;
		
		while ((line = br.readLine()) != null)
		{
			System.err.println(line);
			String[] tokens = line.split(" +");
			
			int round = 0;
			
			try
			{
				round = Integer.parseInt(tokens[2]);
			}
			catch (Exception e)
			{
				break;
			}
			
			if (round != previousRound)
			{
				updateHistory();
				previousRound = round;
			}
			
			parseLine(line);
		}
		
		for (int i=0; i<numBots; ++i)
		{
			Vector<Double> w = winHistory.get(botNames[i]);
			System.out.print(botNames[i] + " ");
		
			for (int v=0; v<w.size(); ++v)
			{
				System.out.print(w.get(v) + " ");
			}
			
			System.out.print("\n");
		}
		
		System.out.print("\n\n");
		
		for (int i=0; i<numBots; ++i)
		{
			Vector<Integer> w = eloHistory.get(botNames[i]);
			System.out.print(botNames[i] + " ");
		
			for (int v=0; v<w.size(); ++v)
			{
				System.out.print(w.get(v) + " ");
			}
			
			System.out.print("\n");
		}
	}
	
	public static void updateHistory()
	{
		for (int i=0; i<numBots; ++i)
		{
			double wins = gamesWon.get(botNames[i]);
			double played = gamesPlayed.get(botNames[i]);
			
			winHistory.get(botNames[i]).add(wins / played);
			eloHistory.get(botNames[i]).add(elo.get(botNames[i]));
		}
	}
	
	public static void parseLine(String line)
	{
		String[] result = line.split(" +");
		
		String bot1 = result[3];
		String bot2 = result[4];
		String botW = result[5];
		
		gamesPlayed.put(bot1, gamesPlayed.get(bot1) + 1);
		gamesPlayed.put(bot2, gamesPlayed.get(bot2) + 1);
		gamesWon.put(botW, gamesWon.get(botW) + 1);
		
		if (bot1.equals(botW))
		{
			updateELO(bot1, bot2);
		}
		else
		{
			updateELO(bot2, bot1);
		}
		
	}
	
	public static int k(int tr)
	{
		return 4;
	}
	
	public static void updateELO(String winner, String loser)
	{
		int wELO = elo.get(winner);
		int lELO = elo.get(loser);
		
		double wExp = Math.pow(10,((lELO - wELO)/400));
		double lExp = Math.pow(10,((wELO - lELO)/400));
	
		int wD = (int)((double)k(wELO) * (1 - 1/(1+wExp)));
		int lD = (int)((double)k(lELO) * (1 - 1/(1+lExp)));
		
		elo.put(winner, Math.max(wELO + wD, 400));
		elo.put(loser, Math.max(lELO - lD, 400));
	}
}
