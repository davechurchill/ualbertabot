package objects;

public class ClientStatusMessage implements Message
{
	private static final long serialVersionUID = 3747113650323312712L;
	
	public ClientStatus 				status 			= null;
	public Game 						game 			= null;
	public boolean 						isHost 			= false;
	public TournamentModuleState 		gameState 		= null;
	public int 							startingTime 	= 0;

	public ClientStatusMessage(ClientStatus status, Game game) 
	{
		this.status = status;
		this.game = game;
	}
	
	public ClientStatusMessage(ClientStatus status, Game game, TournamentModuleState gs, boolean isHost, int startingTime) 
	{
		this.status = status;
		this.game = game;
		this.gameState = gs;
		this.isHost = isHost;
		this.startingTime = startingTime;
	}
}
