package objects;

import java.io.*;
import java.util.Vector;

public class TournamentModuleState implements Serializable
{
	/**
	 * 
	 */
	private static final long serialVersionUID = -3613367579032527013L;
	public String selfName 		= "Error";
	public String enemyName 	= "Error";
	public String mapName 		= "Error";
	
	public int frameCount 		= -1;
	public int selfScore 		= -1;
	public int enemyScore 		= -1;
	public int gameElapsedTime 	= -1;
	public int selfSurrender 	= -1;
	public int selfWin 			= -1;
	public int gameEnded 		= -1;
	public int gameHourUp		= -1;
	public Vector<Integer> timeOutExceeded = new Vector<Integer>();

	public TournamentModuleState() 
	{ 
	}

	public boolean readData(String filename)
	{
		BufferedReader br = null;
		boolean success = false;
		
		try 
		{
			br = new BufferedReader(new FileReader(filename));
			
			selfName 		= br.readLine();
			enemyName 		= br.readLine();
			mapName 		= br.readLine();
			frameCount 		= Integer.parseInt(br.readLine());
			selfScore 		= Integer.parseInt(br.readLine());
			enemyScore 		= Integer.parseInt(br.readLine());
			gameElapsedTime	= Integer.parseInt(br.readLine());
			selfSurrender   = Integer.parseInt(br.readLine());
			selfWin 		= Integer.parseInt(br.readLine());
			gameEnded 		= Integer.parseInt(br.readLine());
			gameHourUp		= Integer.parseInt(br.readLine());
		
			timeOutExceeded.clear();
			String line;
			while ((line = br.readLine()) != null)
			{
				if (line.length() == 0)
				{
					break;
				}
				
				timeOutExceeded.add(Integer.parseInt(line));
			}
			
			br.close();
			
			success = true;
		}
		catch (Exception e)
		{
			success = false;
		} 
		finally
		{
			try 
			{
				br.close();
			} 
			catch (Exception e1) 
			{
			}
		}
		
		return success;
	}
}
