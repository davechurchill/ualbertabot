package client;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

import objects.*;

public class ClientListenerThread extends Thread
{
	private Client client;
	private Socket socket = null;
	private ObjectInputStream ois = null;
	private ObjectOutputStream oos = null;
	
	public boolean connected = false;

	public ClientListenerThread(Client client) 
	{
		this.client = client;
	}

	private void setupSocket()
	{
		String temp = client.getServer();
		int port = Integer.parseInt(temp.substring(temp.indexOf(':') + 1));
		String address = temp.substring(0, temp.indexOf(':'));
		
		Client.log("Connecting to Server @ " + address + ":" + port + "\n");
		
		while(true)
		{
			try 
			{
				Client.updateFunc("Connecting");
				socket = new Socket(address, port);
				socket.setKeepAlive(true);
				connected = true;
				break;
			} 
			catch (Exception e) 
			{
				Client.log("Couldn't connect to server, trying again in 5 seconds.\n");
				connected = false;
			}
			
			try { Thread.sleep(5000); } catch (Exception e) { };
		}
	}
	
	private void setupStreams()
	{
		try 
		{
			ois = new ObjectInputStream(socket.getInputStream());
			oos = new ObjectOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			System.out.println("in or out failed");
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	public String getAddress()
	{
		return "" + socket.getInetAddress();
	}
	
	public void run()
	{
		setupSocket();
		setupStreams();

		client.setStatus(ClientStatus.READY);
		
		while (true) 
		{
			try 
			{
				//Client.log("CListner: Waiting for Message...\n");
				Message m = (Message) ois.readObject();
				Client.log("CListner: Message recieved: " + m.toString() + "\n");
				client.receiveMessage(m);
			} 
			catch (Exception e1) 
			{
				connected = false;
				Client.log("Server disconnected, shutting down...\n");
				System.err.println("Cannot contact server, shutting down");
				e1.printStackTrace();
				client.shutDown();
			}
		}	
	}

	public void sendMessageToServer(Message m)
	{
		try 
		{
			oos.writeObject(m);
			oos.flush();
			oos.reset();
		}
		catch (IOException e) 
		{
			System.err.println("sendMessageToServer() exception");
			e.printStackTrace();
			System.out.flush();
			System.exit(-1);
		}
	}
}

