package objects;

import java.io.Serializable;

import server.ServerSettings;

public class Bot implements Serializable
{
	private static final long serialVersionUID = 2690734629985126222L;
	private String name;
	private String race;
	private String type;
	private String bwapiVersion;

	public Bot(String name, String race, String type, String bwapiVersion)
	{
		this.name = name;
		this.race = race;
		this.type = type;
		this.bwapiVersion = bwapiVersion;
	}
	
	public String getName() 
	{
		return name;
	}

	public String getRace() 
	{
		return race;
	}
	
	public String getType()
	{
		return type;
	}
	
	public String getServerDir()
	{
		return ServerSettings.Instance().ServerBotDir + getName() + "/";
	}
	
	public String getBWAPIVersion()
	{
		return bwapiVersion;
	}
	
	public boolean isProxyBot()
	{
		return type.equalsIgnoreCase("proxy");
	}
}
