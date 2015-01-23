package server;

import java.io.*;
import objects.TournamentModuleSettingsMessage;
import java.util.Vector;
import java.util.StringTokenizer;

import objects.BWAPISettings;
import objects.Bot;
import objects.Map;

public class ServerSettings
{
	public Vector<Bot> 		BotVector 			= new Vector<Bot>();
	public Vector<Map> 		MapVector 			= new Vector<Map>();
	
	public String			ServerDir			= "./";
	public String			ServerReplayDir		= "replays/";
	public String			ServerRequiredDir	= "required/";
	public String			ServerBotDir		= "bots/";
	public int				ServerPort			= -1;
	
	public String			GamesListFile		= null;
	public String			ResultsFile			= null;
	public String 			ClearResults	 	= "ask";
	public String 			ResumeTournament 	= "ask";
	public String			DetailedResults		= "no";
	
	public BWAPISettings	bwapi = new BWAPISettings();
	
	public TournamentModuleSettingsMessage tmSettings = new TournamentModuleSettingsMessage();

	private static final ServerSettings INSTANCE = new ServerSettings();
	
	private ServerSettings()
	{
		
	}
	
	public static ServerSettings Instance() 
	{
        return INSTANCE;
    }
	
	public void parseSettingsFile(String filename)
	{
		try
		{
			boolean error = false;
			BufferedReader br = new BufferedReader(new FileReader(filename));
			String line;
			
			while ((line = br.readLine()) != null)
			{
				line = line.trim();
				
				if (line.startsWith("#") || line.length() == 0)
				{
					continue;
				}
				
				// if parseLine is false there was an error
				if (!parseLine(line))
				{
					error = true;
				}
			}
			
			br.close();
			
			if (error)
			{
				System.exit(-1);
			}
		}
		catch (Exception e)
		{
			System.err.println("Error parsing settings file, exiting\n");
			e.printStackTrace();
			System.exit(-1);
		}
		
		if (!checkValidSettings())
		{
			System.err.println("\n\nError in server set-up, please check documentation: http://webdocs.cs.ualberta.ca/~cdavid/starcraftaicomp/tm.shtml#ss");
			System.exit(0);
		}
	}
	
	private boolean checkValidSettings()
	{
		boolean valid = true;
		
		// check if all setting variables are valid
		if (BotVector.size() <= 1) 		{ System.err.println("ServerSettings: Must have at least 2 bots in settings file"); valid = false; }
		if (MapVector.size() <= 0)		{ System.err.println("ServerSettings: Must have at least 1 map in settings file"); valid = false; }
		if (ServerDir == null)			{ System.err.println("ServerSettings: ServerDir not specified in settings file"); valid = false; }
		if (GamesListFile == null)		{ System.err.println("ServerSettings: GamesListFile not specified in settings file"); valid = false; }
		if (ResultsFile == null)		{ System.err.println("ServerSettings: ResultsFile must be specified in settings file"); valid = false; }
		if (ServerPort == -1)			{ System.err.println("ServerSettings: ServerPort must be specified as an integer in settings file"); valid = false; }
		
		// check if all required files are present
		if (!new File(ServerReplayDir).exists()) 	{ System.err.println("ServerSettings: Replay Dir (" + ServerReplayDir + ") does not exist"); valid = false; }
		if (!new File(ServerBotDir).exists()) 		{ System.err.println("ServerSettings: Bot Dir (" + ServerBotDir + ") does not exist"); valid = false; }
		if (!new File(ServerRequiredDir).exists()) 	{ System.err.println("ServerSettings: Required Files Dir (" + ServerRequiredDir + ") does not exist"); valid = false; }
		
		// check all bot directories
		for (Bot b : BotVector)
		{
			String botDir 		= ServerBotDir + b.getName() + "/";
			String botAIDir 	= botDir + "AI/";
			String botDLLFile	= botAIDir + b.getName() + ".dll";
			String botWriteDir 	= botDir + "write/";
			String botReadDir 	= botDir + "read/";
			String proxyScript	= botAIDir + "run_proxy.bat";
			String botBWAPIReq  = ServerRequiredDir + "Required_" + b.getBWAPIVersion() + ".zip";
			
			// Check if all the bot files exist
			if (!new File(botDir).exists()) 		{ System.err.println("Bot Error: " + b.getName() + " bot directory " + botDir + " does not exist."); valid = false; }
			if (!new File(botAIDir).exists()) 		{ System.err.println("Bot Error: " + b.getName() + " bot AI directory " + botAIDir + " does not exist."); valid = false; }
			if (!new File(botDLLFile).exists()) 	{ System.err.println("Bot Error: " + b.getName() + " bot dll file " + botDLLFile + " does not exist."); valid = false; }
			if (!new File(botWriteDir).exists()) 	{ System.err.println("Bot Error: " + b.getName() + " bot write directory " + botWriteDir + " does not exist."); valid = false; }
			if (!new File(botReadDir).exists()) 	{ System.err.println("Bot Error: " + b.getName() + " bot read directory " + botReadDir + " does not exist."); valid = false; }
			if (!new File(botBWAPIReq).exists()) 	{ System.err.println("Bot Error: " + b.getName() + " bot required BWAPI files " + botBWAPIReq + " does not exist."); valid = false; }
			
			// Check if the bot is proxy and the proxy bot exists
			if (b.isProxyBot() && !new File(proxyScript).exists()) 
			{ 
				System.err.println("Bot Error: " + b.getName() + " listed as proxy but " + proxyScript + " does not exist."); 
				valid = false; 
			}
		}
		
		// Check if all the maps exist
		/*for (Map m : MapVector)
		{
			String mapLocation = ServerRequiredDir + "Starcraft/" + m.getMapLocation();
			if (!new File(mapLocation).exists())
			{
				System.err.println("Map Error: " + m.getMapName() + " file does not exist at specified location: " + mapLocation); valid = false;
			}
		}*/
		
		return valid;
	}
	
	private boolean parseLine(String line) throws Exception
	{
		boolean valid = true;
		StringTokenizer st = new StringTokenizer(line, " +");	
		String type = st.nextToken();
		
		if (type.equalsIgnoreCase("Bot"))
		{
			BotVector.add(new Bot(st.nextToken(), st.nextToken(), st.nextToken(), st.nextToken()));
		}
		else if (type.equalsIgnoreCase("Map"))
		{
			MapVector.add(new Map(st.nextToken()));
		}
		else if (type.equalsIgnoreCase("GamesListFile"))
		{
			GamesListFile = st.nextToken();
		}
		else if (type.equalsIgnoreCase("ResultsFile"))
		{
			ResultsFile = st.nextToken();
		}
		else if (type.equalsIgnoreCase("DetailedResults"))
		{
			DetailedResults = st.nextToken();
		}
		else if (type.equalsIgnoreCase("ResumeTournament"))
		{
			String resume = st.nextToken();
			
			if (!resume.equalsIgnoreCase("yes") && !resume.equalsIgnoreCase("no") && !resume.equalsIgnoreCase("ask"))
			{
				System.err.println("ServerSettings: ClearResultsFile invalid option: " + resume);
				valid = false;
			}
			
			ResumeTournament = resume;
		}
		else if (type.equalsIgnoreCase("ClearResults"))
		{
			String clear = st.nextToken();
			
			if (!clear.equalsIgnoreCase("yes") && !clear.equalsIgnoreCase("no") && !clear.equalsIgnoreCase("ask"))
			{
				System.err.println("ServerSettings: ClearResultsFile invalid option: " + clear);
				valid = false;
			}
			
			ClearResults = clear;
		}
		else if (type.equalsIgnoreCase("ServerPort"))
		{
			try
			{
				ServerPort = Integer.parseInt(st.nextToken());
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: ServerPort option must be a valid integer port on your system");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMLocalSpeed"))
		{
			try
			{
				tmSettings.LocalSpeed = Integer.parseInt(st.nextToken());
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMLocalSpeed must be an integer");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMFrameSkip"))
		{
			try
			{
				tmSettings.FrameSkip = Integer.parseInt(st.nextToken());
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMFrameSkip must be an integer");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMGameFrameLimit"))
		{
			try
			{
				tmSettings.GameFrameLimit = Integer.parseInt(st.nextToken());
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMGameFrameLimit must be an integer");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMTimeout"))
		{
			try
			{
				int limit = Integer.parseInt(st.nextToken());
				int bound = Integer.parseInt(st.nextToken());
				
				tmSettings.TimeoutLimits.add(limit);
				tmSettings.TimeoutBounds.add(bound);
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMTimeout must be two integers");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMDrawBotNames"))
		{
			try
			{
				tmSettings.DrawBotNames = st.nextToken();
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMDrawBotNames must true or false");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMDrawTournamentInfo"))
		{
			try
			{
				tmSettings.DrawTournamentInfo = st.nextToken();
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMDrawTournamentInfo must true or false");
				valid = false;
			}
		}
		else if (type.equalsIgnoreCase("TMDrawUnitInfo"))
		{
			try
			{
				tmSettings.DrawUnitInfo = st.nextToken();
			}
			catch (Exception e)
			{
				System.err.println("ServerSettings: TMDrawUnitInfo must true or false");
				valid = false;
			}
		}
		else
		{
			System.err.println("Incorrect setting type in settings file:    " + type);
			valid = false;
		}
		
		return valid;
	}
}