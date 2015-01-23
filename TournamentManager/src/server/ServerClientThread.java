package server;

import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;

import javax.imageio.ImageIO;
import objects.*;
import server.Server;

public class ServerClientThread extends Thread implements Comparable<ServerClientThread>
{
	
	private InetAddress 		address;
	public  InstructionMessage 	lastInstructionSent = null;
	private Socket 				con;
	private ClientStatus 		status;
	private Server 				server;
	
	private ObjectInputStream 	ois = null;
	private ObjectOutputStream 	oos = null;
	private boolean	 			run = true;
	
	ImageWindow					imageWindow;

	public ServerClientThread(Socket con, Server man) throws SocketException 
	{
		address = con.getInetAddress();
		this.con = con;
		con.setKeepAlive(true);
		server = man;
		status = ClientStatus.READY;
	}

	public void run() 
	{
		setupConnectionStreams();

		while (run)
		{
			try 
			{
				Message m = (Message) ois.readObject();
				handleClientMessage(m);
			}
			catch (Exception e) 
			{
				server.log("Excpetion in ManagerClientThread, removing client\n");
				e.printStackTrace();
				server.removeClient(this);
				run = false;
			}
		}
	}
	
	private void setupConnectionStreams()
	{
		try 
		{
			oos = new ObjectOutputStream(con.getOutputStream());
			ois = new ObjectInputStream(con.getInputStream());
		} 
		catch (Exception e) 
		{
			server.log("ManagerClientThread Object Streams could not initialize\n");
			e.printStackTrace();
		}
	}
	
	private void handleClientMessage(Message m) throws Exception
	{
		if (m != null) 
		{
			if (m instanceof ClientStatusMessage)
			{
				updateClientStatus((ClientStatusMessage)m);
			}
			else if (m instanceof DataMessage)
			{
				DataMessage dm = (DataMessage)m;
				
				if (dm.type == DataType.REPLAY)
				{
					server.log("Message from Client " + server.getClientIndex(this) + " : " + m.toString() + "\n");
					dm.write(ServerSettings.Instance().ServerReplayDir);
				}
				else if (dm.type == DataType.WRITE_DIR)
				{
					dm.write(ServerSettings.Instance().ServerBotDir + dm.botName + "\\write");
				}
				else if (dm.type == DataType.SCREENSHOT)
				{
					InputStream in = new ByteArrayInputStream(dm.getRawData());
					BufferedImage screenshot = ImageIO.read(in);
					
					if (imageWindow == null)
					{
						imageWindow = new ImageWindow(this.getAddress().toString(), screenshot);					
					}
					else
					{
						imageWindow.refresh(screenshot);
					}
				
				}
			}
			
		}
	}
	
	private void updateClientStatus(ClientStatusMessage m)
	{
		status = m.status;
		server.updateClient(this);
		
		// if the status is sending, grab the replay
		if (status == ClientStatus.SENDING)
		{
			server.receiveGameResults(m.game); 
		}
		
		if (m.status == ClientStatus.RUNNING && m.gameState != null)
		{
			server.updateRunningStats(this.toString(), m.gameState, m.isHost);
		}
		
		if (m.status == ClientStatus.STARTING && m.startingTime > 0)
		{
			server.updateStartingStats(this.toString(), m.startingTime);
		}
	}

	public synchronized void sendMessage(Message m) throws Exception
	{
		server.log("Sending Message to Client " + server.getClientIndex(this) + " : " + m.toString() + "\n");
		
		oos.writeObject(m);
		oos.flush();
		oos.reset();
		
		if (m instanceof InstructionMessage)
		{
			lastInstructionSent = (InstructionMessage)m;
		}
	}
	
	public void sendChaoslauncherFiles()
	{
		try
		{
			Message m = new DataMessage(DataType.CHAOSLAUNCHER, ServerSettings.Instance().ServerRequiredDir + "Chaoslauncher.zip");
			sendMessage(m);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public void sendTournamentModuleSettings()
	{
		try
		{
			sendMessage(ServerSettings.Instance().tmSettings);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public void sendBotFiles(Bot bot)
	{
		try
		{
			Message m = new DataMessage(DataType.BOT_DIR, bot.getName(), ServerSettings.Instance().ServerBotDir + bot.getName());
			sendMessage(m);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public void sendRequiredFiles(Bot bot)
	{
		try
		{
			Message m = new DataMessage(DataType.REQUIRED_DIR, ServerSettings.Instance().ServerRequiredDir + "Required_" + bot.getBWAPIVersion() + ".zip");
			sendMessage(m);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public String toString()
	{
		return "" + this.getAddress().toString().replaceAll("/", "");
	}
	
	public synchronized InetAddress getAddress() 
	{
		return address;
	}

	public synchronized ClientStatus getStatus() 
	{
		return status;
	}

	public void stopThread() 
	{
		this.interrupt();
		try 
		{
			this.con.close();
		} 
		catch 
		(IOException e) 
		{
			e.printStackTrace();
		}
		
		run = false;
	}

	public synchronized void setStatus(ClientStatus status) 
	{
		this.status = status;
	}

	@Override
	public int compareTo(ServerClientThread arg0) 
	{
		return this.address.equals((arg0).getAddress()) ? 0 : 1;
	}
}
