package utility;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.File;
import java.util.Vector;

import objects.*;
import server.ServerSettings;

public class GameParser 
{
	private static GameStorage games;
	private static Vector<Bot> bots;
	private static Vector<Map> maps;

	public static GameStorage getGames(Vector<Bot> p_bots, Vector<Map> p_maps)
	{
		try
		{
			maps = p_maps;
			bots = p_bots;
			parse();
		}
		catch (Exception e)
		{
			System.err.println("Couldn't load games file list\n");
			e.printStackTrace();
			System.exit(-1);
		}
		
		return games;
	}

	private static void parse() throws NumberFormatException, Exception 
	{
		games = new GameStorage();
		try 
		{
		
			if (!new File(ServerSettings.Instance().GamesListFile).exists())
			{
				return;
			}
		
			BufferedReader br = new BufferedReader(new FileReader(ServerSettings.Instance().GamesListFile));
			parseGames(br);
			br.close();
		} 
		catch (FileNotFoundException e) 
		{
			System.out.println("Could not read settings file");
			e.printStackTrace();
		} 
		catch (IOException e) 
		{
			System.out.println("IOException while reading settings.ini");
			e.printStackTrace();
		}
	}
	
	private static void parseGames(BufferedReader br) throws NumberFormatException, Exception 
	{
		String line = br.readLine();
		while (line.startsWith("#")) 
		{
			line = br.readLine();
		}
		
		line = line.trim();
		while (line != null) 
		{
			if(!line.startsWith("#") && line.length() > 0)
			{
				line = line.trim();
				String[] args = line.split("\\s+");
				Game newGame = new Game(Integer.parseInt(args[0]), Integer.parseInt(args[1]), findBot(args[2]), findBot(args[3]), findMap(args[4])); 
				games.addGame(newGame, newGame.getRound());
			}
			line = br.readLine();
		}
	}
	
	private static Bot findBot(String name) throws Exception
	{
		for(Bot b : bots)
		{
			if(b.getName().equals(name))
			{
				return b;
			}
		}
		
		throw new Exception("Bot not found!!\n Was looking for \"" + name + "\"");
	}
	
	private static Map findMap(String name) throws Exception
	{
		for (Map m : maps)
		{
			if (m.getMapName().equals(name))
			{
				return m;
			}
		}
		
		throw new Exception("Map not found!!\n Was looking for\"" + name + "\"");
	}
}
