package objects;

import java.util.Vector;

import server.ServerSettings;

public class GameResult implements Comparable<Object>
{
	public int gameID				= -1;
	public int roundID				= -1;
	
	public String hostName			= "Error";
	public String awayName			= "Error";
	public String mapName			= "Error";
	
	public String firstReport		= "Error";
	
	public String winName			= "Error";
	public String crashName			= "";
	public String timeOutName		= "";
	
	public boolean hostWon			= false;
	public boolean hostCrash		= false;
	public boolean awayCrash		= false;
	
	public Vector<Integer> hostTimers   = new Vector<Integer>();
	public Vector<Integer> awayTimers   = new Vector<Integer>();

	public boolean hourTimeout		= false;
	
	public int hostScore			= 0;
	public int awayScore			= 0;
	public int finalFrame			= -2;
	public int hostTime				= 0;
	public int awayTime				= 0;

	public GameResult() {}

	public GameResult (String data) 
	{
		setResult(data);

		int numTimers = ServerSettings.Instance().tmSettings.TimeoutLimits.size();
	
		for (int i=0; i<numTimers; ++i)
		{
			hostTimers.add(0);
			awayTimers.add(0);
		}
	}
	
	public void setResult (String dataLine)
	{
		String[] data = dataLine.split(" +");
		
		gameID 				= Integer.parseInt(data[1]);
		roundID 			= Integer.parseInt(data[2]);
		
		hostName			= data[3];
		awayName			= data[4];
		mapName				= data[5];
		
		if (data[6].equals("true"))
		{
			hostWon = true;
		}
		
		if (data[7].equals("true"))
		{
			hostCrash = true;
		}
		
		if (data[8].equals("true"))
		{
			awayCrash = true;
		}
		
		hourTimeout			= Boolean.getBoolean(data[9]);
		
		hostScore			= Integer.parseInt(data[10]) != 0 ? Integer.parseInt(data[10]) : hostScore;
		awayScore			= Integer.parseInt(data[11]) != 0 ? Integer.parseInt(data[11]) : awayScore;
		
		finalFrame			= Integer.parseInt(data[12]) > finalFrame ? Integer.parseInt(data[12]) : finalFrame;
		
		hostTime			= Integer.parseInt(data[13]) != 0 ? Integer.parseInt(data[13]) : hostTime;
		awayTime			= Integer.parseInt(data[14]) != 0 ? Integer.parseInt(data[14]) : awayTime;
		
		int numTimers = ServerSettings.Instance().tmSettings.TimeoutLimits.size();
		for (int i=0; i<numTimers; ++i)
		{
			hostTimers.add(Integer.parseInt(data[15 + i]));
			awayTimers.add(Integer.parseInt(data[15 + numTimers + i]));
		}
		
		if (finalFrame > 0 && hostCrash && !awayCrash)
		{
			crashName = hostName;
			hostWon = false;
		}
		else if (finalFrame > 0 && awayCrash && !hostCrash)
		{
			crashName = awayName;
			hostWon = true;
		}
			
		int tempFinalFrame 	= Integer.parseInt(data[12]);
		
		// if this is the first bot to report
		if (firstReport.equals("Error"))
		{
			// if host time is zero then this is the away bot reporting
			if (hostTime == 0)
			{
				firstReport = awayName;
				// if we have no final frame then the bot must have crashed
				if (tempFinalFrame == -1)
				{
					awayCrash = true;
					hostCrash = false;
					crashName = awayName;
					hostWon = true;
				}
			}
			// this is the host bot reporting
			else
			{
				firstReport = hostName;
				// if we have no final frame then the bot must have crashed
				if (tempFinalFrame == -1)
				{
					hostCrash = true;
					awayCrash = false;
					crashName = hostName;
					hostWon = false;
				}
			}
		}
		// otherwise this is the 2nd report
		// this checks to see if the 2nd report bot crashed
		else if (tempFinalFrame == -1)
		{
			// if the first bot to report was host
			if (firstReport.equals(hostName))
			{
				hostCrash = false;
				awayCrash = true;
				crashName = awayName;
				hostWon = true;
			}
			// otherwise the first bot to report was the away bot
			else
			{
				awayCrash = false;
				hostCrash = true;
				crashName = hostName;
				hostWon = false;
			}
		}
		
		// check bot time-outs
		for (int i=0; i<numTimers; ++i)
		{
			// check if the host timed out
			if (hostTimers.get(i) >= ServerSettings.Instance().tmSettings.TimeoutBounds.get(i))
			{
				timeOutName = hostName;
				hostWon = false;
				break;
			}
			
			// check if the away bot timed out
			if (awayTimers.get(i) >= ServerSettings.Instance().tmSettings.TimeoutBounds.get(i))
			{
				timeOutName = awayName;
				hostWon = true;
				break;
			}
		}
		
		// if the bots reached the hour time limit
		if (finalFrame >= ServerSettings.Instance().tmSettings.GameFrameLimit)
		{
			hourTimeout = true;
			
			// the winner is the bot with the highest score
			if (hostScore >= awayScore)
			{
				hostWon = true;
			}
			else
			{
				hostWon = false;
			}
		}
		
		winName = hostWon ? hostName : awayName;
	}
	
	public String toString()
	{
		String s = String.format("%7d %5d %15s %15s %15s %8d %15s %15s %25s %6b %6b%6b %8d %8d %10d %10d\n",
				this.gameID, this.roundID, this.hostName, 
				this.awayName, this.winName, this.finalFrame, this.crashName, this.timeOutName, this.mapName, 
				this.hostCrash, this.awayCrash, 
				this.hourTimeout, this.hostScore, this.awayScore, 
				this.hostTime, this.awayTime);
				
		for (int i=0; i<hostTimers.size(); ++i)
		{
			String t = String.format(" %5d", hostTimers.get(i));
			s += t;
		}
		
		for (int i=0; i<awayTimers.size(); ++i)
		{
			String t = String.format(" %5d", awayTimers.get(i));
			s += t;
		}
		
		return s;
	}
	
	public int compareTo(Object other)
	{
		return this.gameID - ((GameResult)other).gameID;
	}

	public String getHostReplayName()
	{
		String replayName = "";
		
		replayName += hostName.toUpperCase() + "\\";
		
		String idString = "" + gameID;
		while(idString.length() < 5) { idString = "0" + idString; }
		
		replayName += idString + "-";
		replayName += hostName.substring(0, Math.min(hostName.length(), 4)).toUpperCase();
		replayName += "_";
		replayName += awayName.substring(0, Math.min(awayName.length(), 4)).toUpperCase();
		replayName += ".REP";
		
		return replayName;
	}
	
	public String getAwayReplayName()
	{
		String replayName = "";
		
		replayName += awayName.toUpperCase() + "\\";
		String idString = "" + gameID;
		while(idString.length() < 5) { idString = "0" + idString; }
		
		replayName += idString + "-";
		replayName += awayName.substring(0, Math.min(awayName.length(), 4)).toUpperCase();
		replayName += "_";
		replayName += hostName.substring(0, Math.min(hostName.length(), 4)).toUpperCase();
		replayName += ".REP";
		
		return replayName;
	}
}
