package objects;

public class ServerShutdownMessage implements Message
{
	private static final long serialVersionUID = -347052767995407052L;

	public ServerShutdownMessage()
	{
	
	}
	
	public String toString()
	{
		return "Server shutting down!";
	}
}