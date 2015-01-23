package objects;

import java.util.Iterator;
import java.util.Vector;

import utility.ResultsParser;

public class GameStorage 
{
	private Vector<Game> allGames;
	
	private int currentGameIndex = 0;
	
	public GameStorage()
	{
		allGames = new Vector<Game>();
	}
	
	public void addGame(Game game, int round)
	{
		allGames.add(game);
	}
	
	public void removePlayedGames(ResultsParser rp)
	{
		Iterator<Game> it = allGames.iterator();
		while (it.hasNext()) 
		{
		    if (rp.hasGameResult(it.next().getGameID())) 
		    {
		        it.remove();
		    }
		}
	}
	
	public boolean hasMoreGames()
	{
		return currentGameIndex < allGames.size() - 1;
	}
	
	public Game getNextGame()
	{
		Game g = allGames.get(currentGameIndex);
		currentGameIndex++;
		return g;
	}

	public Game lookupGame(int gameID, int round) 
	{
		for(Game g : allGames)
		{
			if(g.getGameID() == gameID)
			{
				return g;
			}
		}
		
		return null;
	}
}
