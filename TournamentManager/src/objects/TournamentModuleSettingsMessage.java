package objects;
import java.util.Vector;

public class TournamentModuleSettingsMessage implements Message 
{
	private static final long serialVersionUID = 9062722131163042773L;
	
	public int 					LocalSpeed				= 0;
	public int 					FrameSkip				= 0;
	public int					GameFrameLimit			= 85714;
	
	public Vector<Integer>		TimeoutLimits			= new Vector<Integer>();
	public Vector<Integer>		TimeoutBounds			= new Vector<Integer>();
	
	public String				DrawBotNames			= "true";
	public String				DrawTournamentInfo		= "true";
	public String				DrawUnitInfo			= "true";
	
	public TournamentModuleSettingsMessage()
	{
		
	}
	
	public String toString()
	{
		return "(" + LocalSpeed + "," + FrameSkip + "," + GameFrameLimit + ")";
	}
	
	public String getSettingsFileString()
	{
		String newLine = System.getProperty("line.separator");
		
		String tm = "";
		
		tm += "LocalSpeed "         + LocalSpeed     + newLine;
		tm += "FrameSkip "          + FrameSkip      + newLine;
		tm += "GameFrameLimit "     + GameFrameLimit + newLine;
		tm += "DrawBotNames "       + DrawBotNames     + newLine;
		tm += "DrawTournamentInfo " + DrawTournamentInfo     + newLine;
		tm += "DrawUnitInfo "       + DrawUnitInfo     + newLine;
		
		for (int i = 0; i < TimeoutLimits.size(); ++i)
		{
			tm += "Timeout " + TimeoutLimits.get(i) + " " + TimeoutBounds.get(i) + newLine;	
		}
		
		return tm;
	}
}
