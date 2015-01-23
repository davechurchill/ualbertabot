package objects;

import java.io.*;

public class BWAPISettings implements Serializable, Cloneable 
{
	/**
	 * 
	 */
	private static final long serialVersionUID = -5086509309385709470L;
	/**
	 * Used only for tournaments Tournaments can only be run in RELEASE mode
	 */
	public String tournament;
	/**
	 * auto_menu = OFF | SINGLE_PLAYER | LAN | BATTLE_NET for replays, just set
	 * the map to the path of the replay file
	 */
	public String auto_menu;
	/**
	 * pause_dbg = ON | OFF This specifies if auto_menu will pause until a
	 * debugger is attached to the process. Only works in DEBUG mode.
	 */
	public String pause_dbg;
	/**
	 * lan_mode = Same as the text that appears in the multiplayer connection
	 * list -> Examples: Direct Cable Connection, Local Area Network (UDP),
	 * Local PC (UDP)
	 */
	public String lan_mode;
	/**
	 * auto_restart = ON | OFF if ON, BWAPI will automate through the end of
	 * match screen and start the next match if OFF, BWAPI will pause at the end
	 * of match screen until you manually click OK, and then BWAPI resume menu
	 * automation and start the next match
	 */
	public String auto_restart;
	/**
	 * map = path to map relative to Starcraft folder, i.e. map =
	 * maps\(2)Boxer.scm leaving this field blank will join a game instead of
	 * creating it The filename(NOT the path) can also contain wildcards,
	 * example: maps\(?)*.sc? A ? is a wildcard for a single character and * is
	 * a wildcard for a string of characters
	 */
	public String map;
	/**
	 * mapiteration = RANDOM | SEQUENCE type of iteration that will be done on a
	 * map name with a wildcard
	 */
	public String mapiteration;
	/**
	 * enemy_count = 1-7, for 1v1 games, set enemy_count = 1 only used in single
	 * player games
	 */
	public String enemy_count;
	/**
	 * enemy_race = Terran | Protoss | Zerg | Random | RandomTP | RandomTZ |
	 * RandomPZ only used in single player games
	 */
	public String enemy_race;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_1;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_2;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_3;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_4;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_5;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_6;
	/**
	 * enemy_race_# = Default -> Values for enemy_race are acceptable, Default
	 * will use the value specified in enemy_race
	 */
	public String enemy_race_7;
	/**
	 * game_type = TOP_VS_BOTTOM | MELEE | FREE_FOR_ALL | ONE_ON_ONE |
	 * USE_MAP_SETTINGS | CAPTURE_THE_FLAG | GREED | SLAUGHTER | SUDDEN_DEATH |
	 * TEAM_MELEE | TEAM_FREE_FOR_ALL | TEAM_CAPTURE_THE_FLAG
	 */
	public String game_type;
	/**
	 * save_replay = path to save replay to Accepts all environment variables
	 * including custom variables. See wiki for more info.
	 */
	public String save_replay;
	/**
	 * # of players to wait for in a network game before starting. This includes
	 * the BWAPI player. The game will start immediately when it is full.
	 */
	public int wait_for_min_players;
	/**
	 * Start immediately when the game has reached # players. This includes the
	 * BWAPI player. The game will start immediately when it is full.
	 */
	public int wait_for_max_players;
	/**
	 * The time in milliseconds (ms) to wait after the game has met the
	 * min_players requirement. The game will start immediately when it is full.
	 */
	public int wait_for_time;
	/**
	 * holiday = ON | OFF -> This will apply special easter eggs to the game
	 * when it comes time for a holiday.
	 */
	public String holiday;
	/** Control the display of startup warnings */
	public String show_warnings;
	/** Used to disable shared memory in the Windows Emulator */
	public String shared_memory;
	/**
	 * windowed = ON | OFF -> This causes BWAPI to enter windowed mode when it
	 * is injected.
	 */
	public String windowed;
	/** Determines the position of the window */
	public String left;
	/** Determines the position of the window */
	public String top;
	/** Determines the width of the client area and not the window itself */
	public String width;
	/** Determines the height of the client area and not the window itself */
	public String height;
	/** Game sound engine = ON | OFF */
	public String sound;
	/** Screenshot format = gif | pcx | tga | bmp */
	public String screenshots;
	/** The Path of the log file */
	public String log_path;

	public BWAPISettings() 
	{
		tournament = "";
		auto_menu = "OFF";
		pause_dbg = "OFF";
		lan_mode = "Local Area Network (UDP)";
		auto_restart = "OFF";
		map = null;
		mapiteration = "Random";
		enemy_count = "0";
		enemy_race = "Terran";
		enemy_race_1 = "Default";
		enemy_race_2 = "Default";
		enemy_race_3 = "Default";
		enemy_race_4 = "Default";
		enemy_race_5 = "Default";
		enemy_race_6 = "Default";
		enemy_race_7 = "Default";
		game_type = "MELEE";
		save_replay = "lol";
		wait_for_min_players = 2;
		wait_for_max_players = 8;
		wait_for_time = 60000;
		holiday = "OFF";
		windowed = "ON";
		left = "0";
		top = "0";
		width = "640";
		height = "480";
		sound = "OFF";
		screenshots = "bmp";
		log_path = "bwapi-data\\logs";
		show_warnings = "YES";
		shared_memory = "ON";
	}
	
	public void loadFromFile(String filename)
	{
		try
		{
			BufferedReader br = new BufferedReader(new FileReader(filename));
			String line;
			
			while ((line = br.readLine()) != null)
			{
				line = line.trim().toLowerCase();
				
				if (line.startsWith("#") || line.length() == 0)
				{
					continue;
				}
				
				parseLine(line);
			}
			
			br.close();
		}
		catch (Exception e)
		{
			System.err.println("Error loading BWAPI default settings file\n");
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	private void parseLine(String line) throws Exception
	{
		if (!(line.contains("="))) 
		{
			return;
		}
		
		String token = line.substring(0, line.indexOf('='));
		String value = line.substring(line.indexOf('=') + 1).trim();
		
		if (token.equals("tournament")) 
		{
			this.tournament = value;
		} 
		else if (token.equals("auto_menu")) 
		{
			this.auto_menu = value;
		} 
		else if (token.equals("pause_dbg")) 
		{
			this.pause_dbg = value;
		} 
		else if (token.equals("lan_mode")) 
		{
			this.lan_mode = value;
		} 
		else if (token.equals("auto_restart")) 
		{
			this.auto_restart = value;
		}  
		else if (token.equals("game_type")) 
		{
			this.game_type = value;
		} 
		else if (token.equals("holiday")) 
		{
			this.holiday = value;
		} 
		else if (token.equals("windowed")) 
		{
			this.windowed = value;
		} 
		else if (token.equals("left")) 
		{
			this.left = value;
		} 
		else if (token.equals("top")) 
		{
			this.top = value;
		} 
		else if (token.equals("width")) 
		{
			this.width = value;
		} 
		else if (token.equals("height")) 
		{
			this.height = value;
		} 
		else if (token.equals("sound")) 
		{
			this.sound = value;
		} 
		else if (token.equals("log_path")) 
		{
			this.log_path = value;
		} 
		else if (token.equals("wait_for_time")) 
		{
			this.wait_for_time = Integer.parseInt(value);
		} 
		else if (token.equals("holiday")) 
		{
			this.holiday = value;
		} 
		else if (token.equals("show_warnings")) 
		{
			this.show_warnings = value;
		} 
		else if (token.equals("shared_memory")) 
		{
			this.shared_memory = value;
		} 
		else if (token.equals("screenshots")) 
		{
			this.screenshots = value;
		}
	}

	public String toString()
	{
		return
	
		tournament + "\n" +
		auto_menu + "\n" +
		pause_dbg + "\n" +
		lan_mode + "\n" +
		auto_restart + "\n" +
		map + "\n" +
		mapiteration + "\n" +
		enemy_count + "\n" +
		enemy_race + "\n" +
		enemy_race_1 + "\n" +
		enemy_race_2 + "\n" +
		enemy_race_3 + "\n" +
		enemy_race_4 + "\n" +
		enemy_race_5 + "\n" +
		enemy_race_6 + "\n" +
		enemy_race_7 + "\n" +
		game_type + "\n" +
		save_replay + "\n" +
		wait_for_min_players + "\n" +
		wait_for_max_players + "\n" +
		wait_for_time + "\n" +
		holiday + "\n" +
		windowed + "\n" +
		left + "\n" +
		top + "\n" +
		width + "\n" +
		height + "\n" +
		sound + "\n" +
		screenshots + "\n" +
		log_path + "\n" +
		show_warnings + "\n" +
		shared_memory + "\n";
	}
		
	public BWAPISettings clone()
	{
		BWAPISettings bwapi = new BWAPISettings();
		bwapi.tournament = this.tournament;
		bwapi.auto_menu = this.auto_menu;
		bwapi.pause_dbg = this.pause_dbg;
		bwapi.lan_mode = this.lan_mode;
		bwapi.auto_restart = this.auto_restart;
		bwapi.map = this.map;
		bwapi.mapiteration = this.mapiteration;
		bwapi.enemy_count = this.enemy_count;
		bwapi.enemy_race = this.enemy_race;
		bwapi.enemy_race_1 = this.enemy_race_1;
		bwapi.enemy_race_2 = this.enemy_race_2;
		bwapi.enemy_race_3 = this.enemy_race_3;
		bwapi.enemy_race_4 = this.enemy_race_4;
		bwapi.enemy_race_5 = this.enemy_race_5;
		bwapi.enemy_race_6 = this.enemy_race_6;
		bwapi.enemy_race_7 = this.enemy_race_7;
		bwapi.game_type = this.game_type;
		bwapi.save_replay = this.save_replay;
		bwapi.wait_for_min_players = this.wait_for_min_players;
		bwapi.wait_for_max_players = this.wait_for_max_players;
		bwapi.wait_for_time = this.wait_for_time;
		bwapi.holiday = this.holiday;
		bwapi.windowed = this.windowed;
		bwapi.left = this.left;
		bwapi.top = this.top;
		bwapi.width = this.width;
		bwapi.height = this.height;
		bwapi.sound = this.sound;
		bwapi.screenshots = this.screenshots;
		bwapi.log_path = this.log_path;
		bwapi.shared_memory = this.shared_memory;
		bwapi.show_warnings = this.show_warnings;
		return bwapi;
	}
}
