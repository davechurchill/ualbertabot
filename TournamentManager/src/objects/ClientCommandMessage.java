package objects;

public class ClientCommandMessage implements Message
{
	private static final long serialVersionUID = -347052767995407052L;

	private String command = "";
	
	public ClientCommandMessage(String command)
	{
		this.command = command;
	}
	
	public String getCommand()
	{
		return command;		
	}
	
	public String toString()
	{
		return getCommand();
	}
}