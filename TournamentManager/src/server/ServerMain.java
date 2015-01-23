package server;


public class ServerMain 
{

	public static String serverSettingsFile;
	
	public static void main(String[] args) throws Exception
	{
		if (args.length == 1)
		{
			ServerSettings.Instance().parseSettingsFile(args[0]);
		}
		else
		{
			System.err.println("\n\nPlease provide server settings file as command line argument.\n");
			System.exit(-1);
		}
		
		Server.Instance().start();
		
		while (true)
		{
			Thread.sleep(1000);
		}
	}

}
