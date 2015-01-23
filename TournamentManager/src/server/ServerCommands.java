package server;

import objects.*;
import utility.FileUtils;
import utility.WindowsCommandTools;

public class ServerCommands
{
	public static void Server_InitialSetup()
	{
		if (System.getProperty("os.name").contains("Windows"))
		{
			WindowsCommandTools.RunWindowsCommand("netsh firewall add allowedprogram program = " + ServerSettings.Instance().ServerDir + "server.jar name = AIIDEServer mode = ENABLE scope = ALL", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening TCP 12345 \"Open Port 12345TCP\"", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening UDP 12345 \"Open Port 12345UDP\"", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening TCP 1337 \"Open Port 1337TCP\"", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening UDP 1337 \"Open Port 1337UDP\"", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening TCP 11337 \"Open Port 11337TCP\"", true, false);
			WindowsCommandTools.RunWindowsCommand("netsh firewall add portopening UDP 11337 \"Open Port 11337UDP\"", true, false);
		}
	}
	
	public static void Server_MoveWriteToRead() 
	{
    	for (Bot bot : ServerSettings.Instance().BotVector)
		{
			FileUtils.CopyDirectory(ServerSettings.Instance().ServerBotDir + bot.getName() + "/write/", ServerSettings.Instance().ServerBotDir + bot.getName() + "/read/");
			    		
			//String copy = "xcopy " + ServerSettings.Instance().ServerBotDir + bot.getName() + "/write/*.* " + ServerSettings.Instance().ServerBotDir + bot.getName() + "/read/ /E /V /Y";
			//WindowsCommandTools.RunWindowsCommand(copy, true, false);
			
			//WindowsCommandTools.CopyDirectory(ServerSettings.Instance().ServerBotDir + bot.getName() + "/write/", ServerSettings.Instance().ServerBotDir + bot.getName() + "/read/");
    	}
	}
}