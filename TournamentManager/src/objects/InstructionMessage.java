package objects;

public class InstructionMessage implements Message 
{
	private static final long serialVersionUID = 9062722137863042773L;
	
	public Bot 					hostBot;
	public Bot 					awayBot;
	
	public boolean 				isHost;
	
	public BWAPISettings 		bwapi;
	
	public int 					game_id;
	public int 					round_id;
	
	public String toString()
	{
		return hostBot.getName() + " " + awayBot.getName() + " " + isHost + " (" + game_id + "/" + round_id + ")";
	}
	
	public InstructionMessage()
	{
	
	}
	
	public InstructionMessage(BWAPISettings defaultBWAPI, boolean host, Game game)
	{
		hostBot = game.getHomebot();
        awayBot = game.getAwaybot();
        game_id = game.getGameID();
        round_id = game.getRound();
        isHost = host;
        bwapi = defaultBWAPI.clone();
		
		bwapi.enemy_count = "1";
        bwapi.wait_for_max_players = 2;
        bwapi.map = host ? game.getMap().getMapLocation() : "";
        bwapi.auto_menu = "LAN";
	}
}
