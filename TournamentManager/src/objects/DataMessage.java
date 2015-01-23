package objects;

import java.io.File;

import utility.*;

public class DataMessage implements Message
{
	/**
	 * 
	 */
	private static final long serialVersionUID = -8193083217816970522L;
	public DataType type;		// the type of data
	public byte[] data;			// the data
	public String botName;		// associated bot name (if necessary)
	public String zipFileName = "";
	
	public DataMessage(DataType type, String src)
	{
		this.type = type;
		
		// Chaoslauncher and Required Files are in zip files already, so load from a zip
		if (type == DataType.CHAOSLAUNCHER || type == DataType.REQUIRED_DIR)
		{
			zipFileName = src;
			readZipFile(src);
		}
		else
		{
			// otherwise we need to zip the directory
			read(src);
		}
		
	}
	
	public DataMessage(DataType type, byte [] data)
	{
		this.type = type;
		
		this.data = data;
		
	}
	
	public DataMessage(DataType type, String botName, String src)
	{
		this.type = type;
		this.botName = botName;
		read(src);
	}
	
	public byte[] getRawData()
	{
		return data;
	}
	
	public String toString()
	{
		String extra = " " + (botName == null ? new File(zipFileName).getName() : botName);		
		
		return "" + type + extra + " " + (data.length/1000) + " kb";
	}
	
	public void read(String src)
	{
		try
		{
			data = ZipTools.ZipDirToByteArray(src);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public void readZipFile(String src)
	{
		try
		{
			data = ZipTools.LoadZipFileToByteArray(src);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public void write(String dest)
	{
		try
		{
			ZipTools.UnzipByteArrayToDir(data, dest);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
}