package utility;

import java.util.*;
import java.io.*;
import java.text.*;

import objects.GameResult;
import server.ServerSettings;

public class ResultsParser
{
	private HashMap<Integer, GameResult> gameResults = new HashMap<Integer, GameResult>();
	
	Vector<GameResult> results 		= new Vector<GameResult>();
	Set<Integer> gameIDs 			= new HashSet<Integer>();
	
	private int numBots 			= ServerSettings.Instance().BotVector.size();
	private int numMaps 			= ServerSettings.Instance().MapVector.size();
	
	private String[] botNames 		= new String[numBots];;
	private String[] shortBotNames 	= new String[numBots];;
	private String[] mapNames 		= new String[numMaps];;
	
	private int[][] wins 			= new int[numBots][numBots];
	private int[][] mapWins 		= new int[numBots][numMaps];
	private int[][] mapGames 		= new int[numBots][numMaps];
	
	private int[] timeout 			= new int[numBots];
	private int[] games 			= new int[numBots];
	private int[] crash 			= new int[numBots];
	private int[] frames 			= new int[numBots];
	private int[] mapUsage 			= new int[numMaps];
	private int[] hour 		        = new int[numBots];
	
	private Vector<Vector<Integer>> winsAfterRound = new Vector<Vector<Integer>>();
	private Vector<Vector<Integer>> gamesAfterRound = new Vector<Vector<Integer>>();
	
	public ResultsParser(String filename)
	{
		// set the bot names and map names
		for (int i=0; i<botNames.length; ++i)
		{
			botNames[i] = ServerSettings.Instance().BotVector.get(i).getName();
			shortBotNames[i] = botNames[i].substring(0, Math.min(5, botNames[i].length()));
		}
		
		for (int i=0; i<mapNames.length; ++i)
		{
			mapNames[i] = ServerSettings.Instance().MapVector.get(i).getMapName();
		}
		
		try
		{
			if (!new File(filename).exists())
			{
				return;
			}
		
			BufferedReader br = new BufferedReader(new FileReader(filename));
		
			String line;
			
			while ((line = br.readLine()) != null)
			{
				parseLine(line);
			}
			
			results = new Vector<GameResult>(gameResults.values());
			Collections.sort(results, new GameResultIDComparator());
			
			parseResults(results);
			br.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
		
	public int numResults()
	{
		return results.size();
	}
		
	public void parseResults(Vector<GameResult> results)
	{
		for (int i=0; i<numBots; ++i)
		{
			gamesAfterRound.add(new Vector<Integer>());
			winsAfterRound.add(new Vector<Integer>());
			
			gamesAfterRound.get(i).add(0);
			winsAfterRound.get(i).add(0);
		}
		
		for (int i=0; i<results.size(); i++)
		{
			GameResult result = results.get(i);
			
			int b1 = getIndex(result.hostName);
			int b2 = getIndex(result.awayName);
			
			if (getIndex(result.timeOutName) != -1)
			{
				timeout[getIndex(result.timeOutName)]++;
			}
			
			if (getIndex(result.crashName) != -1)
			{
				crash[getIndex(result.crashName)]++;
			}
			
			hour[b1] += (result.hourTimeout) ? 1 : 0;
			hour[b2] += (result.hourTimeout) ? 1 : 0;
			
			games[b1]++;
			games[b2]++;
			
			frames[b1] += result.finalFrame;
			frames[b2] += result.finalFrame;
			
			int winner = getIndex(result.winName);
			int map = getMapIndex(result.mapName);
			
			wins[b1][b2] += (winner == b1) ? 1 : 0;
			wins[b2][b1] += (winner == b2) ? 1 : 0;
			
			mapUsage[map]++;
			mapWins[winner][map]++;
			mapGames[b1][map]++;
			mapGames[b2][map]++;
						
			// update win percentage arrays
			if (result.roundID >= gamesAfterRound.get(b1).size())
			{
				gamesAfterRound.get(b1).add(gamesAfterRound.get(b1).lastElement() + 1);
			}
			else
			{
				gamesAfterRound.get(b1).set(result.roundID, gamesAfterRound.get(b1).get(result.roundID) + 1);
			}
			
			// if it's a new round, add it to the end of the vector
			if (result.roundID >= gamesAfterRound.get(b2).size())
			{
				gamesAfterRound.get(b2).add(gamesAfterRound.get(b2).lastElement() + 1);
			}
			// otherwise just add 1 to the back
			else
			{
				gamesAfterRound.get(b2).set(result.roundID, gamesAfterRound.get(b2).get(result.roundID) + 1);
			}
			
			if (result.roundID >= winsAfterRound.get(b1).size())
			{
				winsAfterRound.get(b1).add(winsAfterRound.get(b1).lastElement());		
			}
			
			if (result.roundID >= winsAfterRound.get(b2).size())
			{
				winsAfterRound.get(b2).add(winsAfterRound.get(b2).lastElement());		
			}
			
			winsAfterRound.get(winner).set(result.roundID, winsAfterRound.get(winner).get(result.roundID) + 1);	
		}
		
		printWinPercentageGraph();
	}
	
	public String getRawDataHTML()
	{
		String data = "";
		for (int i=0; i<results.size(); i++)
		{
			data += results.get(i).toString();
		}
		
		return data;
	}
	
	public void printWinPercentageGraph()
	{
		String s = "";
		for (int i=0; i<numBots; ++i)
		{
			s = s + botNames[i] + " ";
			for (int j=0; j<gamesAfterRound.get(i).size(); ++j)
			{
				s = s + "" + ((double)winsAfterRound.get(i).get(j) / (double)gamesAfterRound.get(i).get(j)) + " ";
			}
			s = s + "\n";
		}
		
		try
		{
			File file = new File("winpercentage.txt");
			if (!file.exists()) {
				file.createNewFile();
			}
			FileWriter fw = new FileWriter(file.getAbsoluteFile());
			BufferedWriter bw = new BufferedWriter(fw);
			bw.write(s);
			bw.close();
			fw.close();
		}
		catch (Exception e)
		{
			
		}
	}
	
	public boolean hasGameResult(int gameID)
	{
		return gameIDs.contains(gameID);
	}
	
	public String getAllResultsHTML()
	{
		int numTimers = ServerSettings.Instance().tmSettings.TimeoutLimits.size();
		int width = 89;
		String html = "<html><head>\n";
		html += "<script type=\"text/javascript\" src=\"javascript/jquery-1.10.2.min.js\"></script>	<script type=\"text/javascript\" src=\"javascript/jquery.tablesorter.js\"></script> <style> td { text-align:center; } </style>\n";
		html += "<link rel=\"stylesheet\" href=\"javascript/themes/blue/style.css\" type=\"text/css\" media=\"print, projection, screen\" />\n";
		html += "<script type=\"text/javascript\"> $(function() { $(\"#resultsTable\").tablesorter({widgets: ['zebra']}); });</script>\n";
		html += "</head>\n"; 
		html += "<p style=\"font: 16px/1.5em Verdana\">Go To: <a href=\"index.html\">Results Overview</a>. Click column header to sort. Sort multiple columns by holding shift.</p>\n";
		html += "<table cellpadding=2 rules=all style=\"font: 10px/1.5em Verdana\" id=\"resultsTable\" class=\"tablesorter\">\n";
		html += "  <thead><tr>\n";
		html += "    <th width=" + 130 + ">Round/Game</td>\n";
		html += "    <th width=" + 100 + ">Winner</td>\n";
		html += "    <th width=" + 100 + ">Loser</td>\n";
		html += "    <th width=" + 100 + ">Crash</td>\n";
		html += "    <th width=" + 100 + ">Timeout</td>\n";
		html += "    <th width=" + 100 + ">Map</td>\n";
		html += "    <th width=" + 100 + ">Duration</td>\n";
		html += "    <th width=" + 100 + ">W Score</td>\n";
		html += "    <th width=" + 100 + ">L Score</td>\n";
		html += "    <th width=" + 100 + ">(W-L)/Max</td>\n";
		for (int t=0; t<numTimers; ++t)
		{
			html += "    <th width=" + width + ">W " + ServerSettings.Instance().tmSettings.TimeoutLimits.get(t) + "</td>\n";
		}
		for (int t=0; t<numTimers; ++t)
		{
			html += "    <th width=" + width + ">L " + ServerSettings.Instance().tmSettings.TimeoutLimits.get(t) + "</td>\n";
		}
		html += "  </tr></thead><tbody>\n";
				
		Collections.sort(results, new GameResultIDComparator());
		
		for (int i=0; i<results.size(); i++)
		{
			GameResult r = results.get(i);
			html += "  <tr>\n";
			
			String idString = "" + r.gameID;
			while (idString.length() < 5) { idString = "0" + idString; }
			
			html += "    <td>" + ( r.roundID + " / " + idString) + "</td>\n";
			
			String winnerReplayName = r.hostWon ? r.getHostReplayName() : r.getAwayReplayName();
			String loserReplayName = r.hostWon ? r.getAwayReplayName() : r.getHostReplayName();
			String winnerName = r.hostWon ? r.hostName : r.awayName;
			String loserName = r.hostWon ? r.awayName : r.hostName;
			
			if (new File("replays\\" + winnerReplayName).exists())
			{
				html += "    <td>" + "<a href=\"replays/" + winnerReplayName + "\">" + winnerName + "</a></td>\n";
			}
			else
			{
				html += "    <td>" + winnerName + "</td>\n";
			}
			
			if (new File("replays\\" + loserReplayName).exists())
			{
				html += "    <td>" + "<a href=\"replays/" + loserReplayName + "\">" + loserName + "</a></td>\n";
			}
			else
			{
				html += "    <td>" + loserName + "</td>\n";
			}
			
			html += "    <td>" + r.crashName + "</td>\n";
			html += "    <td>" + r.timeOutName + "</td>\n";
			html += "    <td>" + r.mapName.substring(r.mapName.indexOf(')') + 1, r.mapName.indexOf('.')) + "</td>\n";
			
			String hours   = "" + r.finalFrame/(24*60*60); while (hours.length() < 2) { hours = "0" + hours; }
			String minutes = "" + (r.finalFrame % (24*60*60))/(24*60); while (minutes.length() < 2) { minutes = "0" + minutes; }
			String seconds = "" + (r.finalFrame % (24*60))/(24); while (seconds.length() < 2) { seconds = "0" + seconds; }
			
			html += "    <td>00:" + minutes + ":" + seconds + "</td>\n";
			html += "    <td>" + (r.hostWon ? r.hostScore : r.awayScore) + "</td>\n";
			html += "    <td>" + (r.hostWon ? r.awayScore : r.hostScore) + "</td>\n";
			
			double maxScore = Math.max(r.hostScore, r.awayScore) + 1;
			double maxSq = (double)maxScore;// * (double)maxScore;
			double hostSq = (double)r.hostScore;// * (double)r.hostScore;
			double awaySq = (double)r.awayScore;// * (double)r.awayScore;
			double closeNess = (r.hostWon ?  (hostSq-awaySq)/(maxSq) : (awaySq-hostSq)/(maxSq));
			closeNess = (double)Math.round(closeNess * 100000) / 100000;
			html += "    <td>" + closeNess + "</td>\n";
			
			for (int t=0; t<numTimers; ++t)
			{
				html += "    <td>" + (r.hostWon ? r.hostTimers.get(t) : r.awayTimers.get(t)) + "</td>\n";
			}
			
			for (int t=0; t<numTimers; ++t)
			{
				html += "    <td>" + (r.hostWon ? r.awayTimers.get(t) : r.hostTimers.get(t)) + "</td>\n";
			}
			
			html += "  </tr>\n";
		}
		
		html += "</tbody></table>\n";
		html += "<br></html>\n";
		
		return html;
	}
	
	public String getHeaderHTML()
	{
		String html = "<html>\n";
		html += "<head>\n";
		html += "<title>StarCraft AI Competition Results</title>\n";
		html += "</head>\n";
		html += "<body alink=\"#0000FF\" vlink=\"#0000FF\" link=\"#0000FF\">\n";
		
		return html;
	}
	
	public String getEntrantsHTML()
	{
		String html = "";
	
		try
		{
			BufferedReader br = new BufferedReader(new FileReader("..\\html\\entrants.html"));
			String line;
			
			while ((line = br.readLine()) != null)
			{
				html += line + "\n";
			}
			html += "<br>\n";
			br.close();
			return html;
		}
		catch (Exception e)
		{
			return "";
		}
	}
	
	public String getFooterHTML()
	{
		String timeStamp = "<p style=\"font: 10px/1.5em Verdana\">Last Updated: " + new SimpleDateFormat("yyyy-MM-dd [HH:mm:ss]").format(Calendar.getInstance().getTime()) + "</p>\n";
		String html = timeStamp;
		html += "</body>\n";
		html += "</html>\n";
		return html;
	}
	
	
	public String getResultsHTML()
	{	
		
		String html = "";
		int[] allgames = new int[botNames.length];
		int[] allwins = new int[botNames.length];
		
		for (int i=0; i<numBots; i++)
		{
			for (int j=0; j<numBots; j++)
			{
				allwins[i] += wins[i][j];
				allgames[i] += wins[i][j] + wins[j][i];
			}
		}
		
		Vector<ResultPair> allPairs = new Vector<ResultPair>();
		for (int i=0; i<numBots; i++)
		{
			double winPercentage = (allgames[i] > 0 ? ((double)allwins[i]/allgames[i]) : 0);
			allPairs.add(new ResultPair(botNames[i], i, winPercentage));
		}
		Collections.sort(allPairs, new ResultPairComparator());
		
		/////////////////////////////////////////
		// EXTRA STATS
		/////////////////////////////////////////
		int width = 80;
		File resultsHTML = new File("html/results.html");
		long resultsHTMLSize = resultsHTML.exists() ? resultsHTML.length() : 0;
		
		html += "<p style=\"font: 16px/1.5em Verdana\">Go To: <a href=\"results.html\">Detailed Game Results</a> ( " + resultsHTMLSize/1000  + " kB )</p>\n";
		html += "<table cellpadding=2 rules=all style=\"font: 14px/1.5em Verdana\">\n";
		html += "  <tr>\n";
		html += "    <td width=100 rowspan=2> </td>\n";
		html += "    <td colspan=8 bgcolor=#CCCCCC align=center style=\"font: 16px/1.5em Verdana\">Overall Tournament Statistics</td>\n";
		html += "  </tr>\n";
		html += "  <tr>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Games</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Win</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Loss</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Win %</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">AvgTime</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Hour</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Crash</td>\n";
		html += "    <td bgcolor=#CCCCCC align=center width=" + width + ">Timeout</td>\n";
		html += "  </tr>\n";
		
		int[] dataTotals = {0, 0, 0, 0, 0, 0, 0, 0};
		
		for (int i=0; i<numBots; ++i)
		{
			int ii = allPairs.get(i).botIndex;
			String color = ((i%2) == 1 ? "#ffffff" : "#E8E8E8");
			html += "  <tr>\n";
			html += "    <td align=center bgcolor=#CCCCCC>" + botNames[ii] + "</td>\n"; 
			html += "    <td align=center bgcolor=" + color + ">" + allgames[ii] + "</td>\n";
			dataTotals[0] += allgames[ii];			
			html += "    <td align=center bgcolor=" + color + ">" + allwins[ii] + "</td>\n";
			dataTotals[1] += allwins[ii];
			html += "    <td align=center bgcolor=" + color + ">" + (allgames[ii] - allwins[ii]) + "</td>\n";
			dataTotals[2] += (allgames[ii] - allwins[ii]);			
			html += "    <td align=center bgcolor=" + color + ">" + new DecimalFormat("##.##").format(allPairs.get(i).win*100) + "</td>\n";
			html += "    <td align=center bgcolor=" + color + ">" + (allgames[ii] > 0 ? getTime(frames[ii]/games[ii]) : "0") + "</td>\n"; 	;
			dataTotals[4] += (allgames[ii] > 0 ? frames[ii]/games[ii] : 0);
			html += "    <td align=center bgcolor=" + color + ">" + hour[ii] + "</td>\n";
			dataTotals[5] += hour[ii];
			html += "    <td align=center bgcolor=" + color + ">" + crash[ii] + "</td>\n"; 
			dataTotals[6] += crash[ii];			
			html += "    <td align=center bgcolor=" + color + ">" + timeout[ii] + "</td>\n";	
			dataTotals[7] += timeout[ii];
			html += "  </tr>\n";
		}
		
		html += "  <tr>\n";
		html += "    <td align=center bgcolor=#CCCCCC><b>Total</b></td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[0]/2) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[1]) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[2]) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + "N/A" + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + getTime((dataTotals[4]/botNames.length)) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[5]/2) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[6]) + "</td>\n";
		html += "    <td align=center bgcolor=#CCCCCC>" + (dataTotals[7]) + "</td>\n";
		html += "  </tr>\n";
		
		html += "</table>\n";
		html += "<br>\n";
		
		html += "<table cellpadding=2 rules=all style=\"font: 14px/1.5em Verdana\">\n";
		html += "  <tr>\n";
		html += "    <td width=100 rowspan=2> </td>\n";
		html += "    <td colspan=" + (numBots+1) + " bgcolor=#CCCCCC align=center style=\"font: 16px/1.5em Verdana\">Bot vs. Bot Results - (Row,Col) = Row Wins vs. Col</td>\n";
		html += "  </tr>\n";
		html += "  <tr>\n";
		html += "    <td width=71 align=center bgcolor=#CCCCCC>Win %</td>\n";
		
		for (int i=0; i<numBots; ++i)
		{
			int ii = allPairs.get(i).botIndex;
			html += "    <td width=71 align=center bgcolor=#CCCCCC>" + shortBotNames[ii] + "</td>\n";
		}
		html += "  </tr>\n";
		
		for (int i=0; i<numBots; i++)
		{
			int ii = allPairs.get(i).botIndex;
			html += "  <tr>\n";
			
			String color = ((i%2) == 1 ? "#ffffff" : "#E8E8E8");
			html += "    <td width=100 align=center  bgcolor=#CCCCCC>" + botNames[ii] + "</td>\n";
			html += "    <td width=71 align=center bgcolor=" + color + ">" + new DecimalFormat("##.##").format(allPairs.get(i).win*100) + "</td>\n";
			
			
			for (int j=0; j<numBots; j++)
			{
				int jj = allPairs.get(j).botIndex;
				if (ii == jj)
				{
					html += "    <td align=center bgcolor=#ffffff>" + "-" + "</td>\n";
				}
				else
				{
					double w = wins[ii][jj];
					double g = wins[ii][jj] + wins[jj][ii];
					double p = g > 0 ? w / g : 0;
					int c = (int)(p * 255);
					String cellColor = "rgb(" + (255-c) + "," + 255 + "," + (255-c) + ")";
					html += "    <td align=center style=\"background-color:" + cellColor + "\">" + wins[ii][jj] + "/" + (int)g + "</td>\n";
				}
			}
			html += "  </tr>\n";
		}
		
		html += "</table>\n";		
		html += "<br>\n";
			
		/////////////////////////////////////////
		// MAP WINS TABLE
		/////////////////////////////////////////
		html += "<table cellpadding=2 rules=all style=\"font: 12px/1.5em Verdana\">\n";
		html += "  <tr>\n";
		html += "    <td width=100 rowspan=2> </td>\n";
		html += "    <td colspan=" + (numMaps) + " bgcolor=#CCCCCC align=center style=\"font: 16px/1.5em Verdana\">Bot Win Percentage By Map</td>\n";
		html += "  </tr>\n";
		html += "  <tr>\n";
		
		for (int i=0; i<numMaps; ++i)
		{
			html += "    <td width=63 align=center bgcolor=#CCCCCC style=\"font: 11px/1.5em Verdana\">" + mapNames[i].substring(3, Math.min(10, mapNames[i].length()-4)) + "</td>\n";
		}
		
		html += "  </tr>\n";
		
		for (int i=0; i<numBots; i++)
		{
			int ii = allPairs.get(i).botIndex;
			html += "  <tr>\n";
			
			html += "    <td width=100 align=center  bgcolor=#CCCCCC>" + botNames[ii] + "</td>\n";
			
			
			for (int j=0; j<numMaps; j++)
			{
				double w = mapWins[ii][j];
				double g = mapGames[ii][j];
				double p = g > 0 ? w / g : 0;
				int c = (int)(p * 255);
				String cellColor = "rgb(" + (255-c) + "," + 255 + "," + (255-c) + ")";
				html += "    <td align=center style=\"background-color:" + cellColor + "\">" + (int)(p*100) + " %</td>\n";
				
			}
			html += "  </tr>\n";
		}
		
		
		html += "</table>\n";
		html += "<br>\n";
		
		
		
		return html;
	}
	
	public void writeToFile(String s, String filename) throws Exception
	{
		File file = new File(filename);
		if (!file.exists()) {
			file.createNewFile();
		}

		FileWriter fw = new FileWriter(file.getAbsoluteFile());
		BufferedWriter bw = new BufferedWriter(fw);
		bw.write(s);
		bw.close();
		fw.close();
	}
	
	public String getTime(int frames)
	{
		int fpm = 24*60;
		int fps = 24;
		int minutes = frames / fpm;
		int seconds = (frames / fps) % 60;
		
		return "" + minutes + ":" + (seconds < 10 ? "0" + seconds : seconds);
	}
		
	public int getIndex(String botName)
	{
		for (int i=0; i<botNames.length; i++)
		{
			if (botNames[i].equals(botName))
			{
				return i;
			}
		}
		
		return -1;
	}
	
	public int getMapIndex(String mapName)
	{
		for (int i=0; i<mapNames.length; i++)
		{
			if (mapNames[i].equals(mapName))
			{
				return i;
			}
		}
		
		return -1;
	}
	
	public void parseLine(String line)
	{
		if (line.trim().length() > 0)
		{
			Vector<String> data = new Vector<String>(Arrays.asList(line.split(" +")));
			data.remove(0);
		
			int gameID = Integer.parseInt(data.get(0));
			gameIDs.add(gameID);
			
			if (gameResults.containsKey(gameID))
			{
				gameResults.get(gameID).setResult(line);
			}
			else
			{
				gameResults.put(gameID, new GameResult(line));
			}
		}
	}
}

class ResultPair
{
	public String botName;
	public int botIndex;
	public double win;

	public ResultPair(String botName, int botIndex, double win)
	{
		this.botName = botName;
		this.botIndex = botIndex;
		this.win = win;
	}
	
}

class GameResultIDComparator implements Comparator<GameResult>
{
    public int compare(GameResult o1, GameResult o2)
    {
		return new Integer(o1.gameID).compareTo(new Integer(o2.gameID));
	}
}

class ResultPairComparator implements Comparator<ResultPair>
{
    public int compare(ResultPair o1, ResultPair o2)
    {
		if (o1.win == o2.win) return 0;
		if (o1.win < o2.win) return 1;
		return -1;
	}
}

class GameResultCrashComparator implements Comparator<GameResult>
{
    public int compare(GameResult o1, GameResult o2)
    {
		return o1.crashName.compareTo(o2.crashName);
	}
}

class GameResultTimeOutComparator implements Comparator<GameResult>
{
    public int compare(GameResult o1, GameResult o2)
    {
		return o1.timeOutName.compareTo(o2.timeOutName);
	}
}

class GameWinnerNameComparator implements Comparator<GameResult>
{
    public int compare(GameResult o1, GameResult o2)
    {
		return o1.winName.compareTo(o2.winName);
	}
}