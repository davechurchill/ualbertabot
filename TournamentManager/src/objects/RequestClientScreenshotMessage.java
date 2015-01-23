package objects;

public class RequestClientScreenshotMessage implements Message
{
	private static final long serialVersionUID = -347052767995307052L;

	public int WIDTH = 1024;
	public int HEIGHT = 768;
	
	public RequestClientScreenshotMessage()
	{
	
	}
	
	public String toString()
	{
		return "Request Client Screenshot";
	}
}