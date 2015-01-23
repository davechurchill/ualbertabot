package objects;

import java.io.File;

public class Map{

	private String mapName;

	private String mapLocation;

	public Map(String mapLocation) 
	{
		this.mapLocation = mapLocation;
		
		File f = new File(mapLocation);
		mapName = f.getName();
	}

	public String getMapName() 
	{
		return mapName;
	}

	public String getMapLocation() 
	{
		return mapLocation;
	}

	public void print() 
	{
		System.out.println(this.mapName + " -> " + this.mapLocation);
	}
}
