
package server;

import java.awt.*;

import javax.swing.*;

import java.awt.Color;

import javax.swing.table.*;

import objects.Bot;
import utility.FileUtils;
import utility.GameListGenerator;
import utility.ResultsParser;

import java.awt.event.*;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class ServerGUI 
{
	Server		server;
	
    private 	JFrame		mainFrame;
    private 	JTable		mainTable;
    private 	JTextArea	bottomText;	
    private 	JPanel		bottomPanel;
    private 	JMenuBar	menuBar;
    private 	JMenu		fileMenu;
    private 	JMenu		actionsMenu;
    private		JMenuItem	exitMenuItem;
    private 	JMenuItem	generateResultsMenuItem;
    private 	JMenuItem	sendClientCommandMenuItem;
    private		JMenuItem	viewClientScreenMenuItem;
    
    private String [] 		columnNames = {"Client", "Status", "Game #", "Self", "Enemy", "Map", "Duration", "Win"};	
	private Object [][] 	data = 	{ };

	private boolean resumedTournament = false;
	
	public ServerGUI(Server server)
	{
		this.server = server;
		CreateGUI();
	}
	
	public void CreateGUI()
	{
		mainTable = new JTable(new DefaultTableModel(data, columnNames));
		mainTable.setDefaultRenderer(Object.class, new MyRenderer());
		bottomText = new JTextArea();
		mainFrame = new JFrame("StarCraft AI Tournament Manager - Server");
		mainFrame.setLayout(new GridLayout(2,0));
		
		bottomPanel = new JPanel();
		bottomPanel.setLayout(new GridLayout(1,0));
		bottomPanel.add(new JScrollPane(bottomText));
	
        mainTable.setFillsViewportHeight(true);
	
        menuBar = new JMenuBar();
        fileMenu = new JMenu("File");
        fileMenu.setMnemonic(KeyEvent.VK_F);
        menuBar.add(fileMenu);
        actionsMenu = new JMenu("Actions");
        actionsMenu.setMnemonic(KeyEvent.VK_A);
        menuBar.add(actionsMenu);
        
        generateResultsMenuItem = new JMenuItem("Generate Detailed Results HTML", KeyEvent.VK_G);
        generateResultsMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	int confirmed = JOptionPane.showConfirmDialog(mainFrame, "Generate Detailed Results? This may take a while for large files.", "Detailed Results Confirmation", JOptionPane.YES_NO_OPTION);
    			if (confirmed == JOptionPane.YES_OPTION)
    			{
    				try
    				{
    					ResultsParser rp = new ResultsParser(ServerSettings.Instance().ResultsFile);
    					logText(getTimeStamp() + " Generating All Results File...\n");
    					writeHTMLFile(rp.getAllResultsHTML(), "html/results.html");
    					logText(getTimeStamp() + " Generating All Results File Complete!\n");
    				}
    				catch (Exception ex)
    				{
    					logText(getTimeStamp() + " Generating results failed :(\n");
    				}
    			}   
            }
        });
        actionsMenu.add(generateResultsMenuItem);

        exitMenuItem = new JMenuItem("Quit Server", KeyEvent.VK_Q);
        exitMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	int confirmed = JOptionPane.showConfirmDialog(mainFrame, "Shutdown Server: This will stop the entire tournament.", "Shutdown Confirmation", JOptionPane.YES_NO_OPTION);
    			if (confirmed == JOptionPane.YES_OPTION)
    			{
    				server.shutDown();
    			}   
            }
        });
        fileMenu.add(exitMenuItem);
        
        sendClientCommandMenuItem = new JMenuItem("Send Command to all Clients", KeyEvent.VK_C);
        sendClientCommandMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	String command = (String)JOptionPane.showInputDialog(mainFrame, 
            				"Enter Windows command to be executed on all Client machines.\n\n"
            				+ "Will run as if typed into the client's Windows command line.\n\n"
            				+ "Execution is asynchronous to client, no error on failure.\n\n"
            				+ "Example:     notepad.exe\n"
            				+ "Example:     taskkill /im notepad.exe\n\n", 
            				"Send Command to Clients", JOptionPane.PLAIN_MESSAGE, null, null, "");
            	
            	if (command != null && command.trim().length() > 0)
            	{
            		server.sendCommandToAllClients(command);
            	}
            }
        });
        actionsMenu.add(sendClientCommandMenuItem);
        
        viewClientScreenMenuItem = new JMenuItem("Take Client Screenshot", KeyEvent.VK_S);
        viewClientScreenMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	String client = (String)JOptionPane.showInputDialog(mainFrame, 
        				"Enter Address of Client\n\n"
        				+ "Will match if substring of address in client window\n\n"
        				+ "If multiple match, multiple will display\n\n"
        				+ "If Client screenshot already open, it will refresh\n\n",
        				"Take Screenshot of Client", JOptionPane.PLAIN_MESSAGE, null, null, "");
            	
            	if (client != null && client.trim().length() > 0)
            	{
            		server.sendScreenshotRequestToClient(client);
            	} 
            }
        });
        actionsMenu.add(viewClientScreenMenuItem);
        
        
		mainFrame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
	    mainFrame.setSize(800,600);
	    mainFrame.setJMenuBar(menuBar);
	    mainFrame.add(new JScrollPane(mainTable));
	    
		mainFrame.add(bottomPanel);
	    mainFrame.setVisible(true);
	    
	    
	    
	    mainFrame.addWindowListener(new WindowAdapter() 
	    {
	    	public void windowClosing(WindowEvent e) 
	    	{
    			int confirmed = JOptionPane.showConfirmDialog(mainFrame, "Shutdown Server: Are you sure?", "Shutdown Confirmation", JOptionPane.YES_NO_OPTION);
    			if (confirmed == JOptionPane.YES_OPTION)
    			{
    				server.shutDown();
    			}     
            }
	    });
	}
	
	public void writeHTMLFile(String html, String filename) throws Exception
	{
		File file = new File(filename);
		if (!file.exists()) {
			file.createNewFile();
		}
		FileWriter fw = new FileWriter(file.getAbsoluteFile());
		BufferedWriter bw = new BufferedWriter(fw);
		bw.write(html);
		bw.close();
		fw.close();
	}
	
	public void handleFileDialogues()
	{
		// if we resumed a tournament, don't delete anything!
		if (resumedTournament)
		{
			return;
		}
		
		handleTournamentData();
		handleNoGamesFile();
	}
	
	public boolean handleTournamentResume()
	{
		int resumeTournament = JOptionPane.NO_OPTION;
		ResultsParser rp = new ResultsParser(ServerSettings.Instance().ResultsFile);
		
		if (rp.numResults() > 0)
		{
			resumeTournament = JOptionPane.showConfirmDialog(mainFrame, "Results found in " + ServerSettings.Instance().ResultsFile + ", resume tournament from games list in " + ServerSettings.Instance().GamesListFile + " ?" , "Resume Tournament Confirmation", JOptionPane.YES_NO_OPTION);
		}
			
		if (resumeTournament == JOptionPane.YES_OPTION)
		{
			resumedTournament = true;
		}
		
		return resumedTournament;
	}
	
	private void handleTournamentData()
	{
		try
		{
			int resClear = JOptionPane.NO_OPTION;
			if (ServerSettings.Instance().ClearResults.equalsIgnoreCase("ask"))
			{
				resClear = JOptionPane.showConfirmDialog(mainFrame, "Clear existing tournament data?\nThis will clear all existing results, replays and bot read/write folders.", "Clear Tournament Data", JOptionPane.YES_NO_OPTION);
			}
			else if (ServerSettings.Instance().ClearResults.equalsIgnoreCase("yes"))
			{
				resClear = JOptionPane.YES_OPTION;
			}
			else
			{
				resClear = JOptionPane.NO_OPTION;
			}
			 
			if (resClear == JOptionPane.YES_OPTION)
			{
				logText(getTimeStamp() + " Clearing Results File\n");
				File resultsFile = new File(ServerSettings.Instance().ResultsFile);
				if (resultsFile.exists())
				{
					resultsFile.delete();
				}
				
				//FileOutputStream fos = new FileOutputStream(ServerSettings.Instance().ResultsFile);
				//fos.write((new String()).getBytes());
				//fos.close();
				
				logText(getTimeStamp() + " Clearing Bot Read / Write Directories\n");
    			for (Bot b : ServerSettings.Instance().BotVector)
    			{
    				String botRead = b.getServerDir() + "read/";
    				String botWrite = b.getServerDir() + "write/";
    				
    				FileUtils.CleanDirectory(new File(botRead)); 
    				FileUtils.CleanDirectory(new File(botWrite)); 
    			}
    			
    			logText(getTimeStamp() + " Clearing Replay Directory\n");
    			FileUtils.CleanDirectory(new File(ServerSettings.Instance().ServerReplayDir)); 
			}
		}
		catch (Exception e)
		{
			
		}
	}
	
	private void handleNoGamesFile()
	{
		// if the games list file doesn't exist
		if (!new File(ServerSettings.Instance().GamesListFile).exists())
		{
			int generate = JOptionPane.showConfirmDialog(mainFrame, "No games list was found.\nGenerate a new round robin games list file?", "Generate Games List?", JOptionPane.YES_NO_OPTION);
			
			if (generate == JOptionPane.YES_OPTION)
			{
				SpinnerNumberModel sModel = new SpinnerNumberModel(1, 1, 1000, 1);
				JSpinner spinner = new JSpinner(sModel);
	
				JOptionPane.showOptionDialog(mainFrame, spinner, "Enter Number of Rounds Per Map:", JOptionPane.PLAIN_MESSAGE, JOptionPane.QUESTION_MESSAGE, null, null, null);
				GameListGenerator.GenerateGames(Integer.parseInt("" + spinner.getValue()), ServerSettings.Instance().MapVector, ServerSettings.Instance().BotVector);
			
				logText(getTimeStamp() + " " + "Generating Round Robin Tournament With " + spinner.getValue() + " Rounds.\n");
			}

			if (!new File(ServerSettings.Instance().GamesListFile).exists()) { System.err.println("ServerSettings: GamesListFile (" + ServerSettings.Instance().GamesListFile + ") does not exist"); System.exit(-1); }
		}
	}
	
	public static String getTimeStamp()
	{
		return new SimpleDateFormat("[HH:mm:ss]").format(Calendar.getInstance().getTime());
	}
	
	public synchronized String getHTML() throws Exception
	{
		String table = "<table cellpadding=2 rules=all style=\"font: 12px/1.5em Verdana\">\n";
		table += "  <tr>\n";
		table += "    <td colspan=11 bgcolor=#CCCCCC align=center style=\"font: 16px/1.5em Verdana\">Current Real-Time Client Scheduler / Status</td>\n";
		table += "  </tr>\n";
		table += "  <tr>\n";
		for (int c=0; c<columnNames.length; ++c)
		{
			table += "    <td bgcolor=#cccccc width=67><center>";
			table += columnNames[c];
			table += "    </center></td>\n";
		}
		table += "  </tr>\n";
		
		for (int r=0; r<mainTable.getRowCount(); ++r)
		{
			String colour = "#FFFFFF";
			if (mainTable.getValueAt(r,1).equals("RUNNING")) colour = "#00FF00";
			if (mainTable.getValueAt(r,1).equals("STARTING")) colour = "#FFFF00";
			if (mainTable.getValueAt(r,1).equals("SENDING")) colour = "#FFA500";
			
			table += "  <tr bgcolor=" + colour + ">\n";
			for (int c=0; c<mainTable.getColumnCount(); ++c)
			{
				table += "    <td><center>";
				String s = (String)mainTable.getValueAt(r,c);
				table += s.substring(0, Math.min(8, s.length()));
				table += "    </center></td>\n";
			}
			table += "  </tr>\n";
		}
		table += "</table><br>\n";
		
		return table;
	}
	
	public synchronized void UpdateClient(String name, String status, String num, String host, String join)
	{
		int row = GetClientRow(name);
		if (row != -1)
		{
			GetModel().setValueAt(status, row, 1);
			GetModel().setValueAt(name, row, 0);
			GetModel().setValueAt(num, row, 2);
			
			if (!status.equals("READY") && !status.equals("SENDING"))
			{
				GetModel().setValueAt("", row, 3);
				GetModel().setValueAt("", row, 4);
				GetModel().setValueAt("", row, 5);
				GetModel().setValueAt("", row, 6);
				GetModel().setValueAt("", row, 7);
			}
			else
			{
				for (int i=3; i<columnNames.length; ++i)
				{
					GetModel().setValueAt(mainTable.getValueAt(row, i), row, i);
				}
			}
		}
		else
		{
			GetModel().addRow(new Object[]{name, status, num, host, join, "", "", ""});
		}
	}
	
	public synchronized void UpdateRunningStats(String client, String self, String enemy, String map, String FrameCount, String win)
	{
		int row = GetClientRow(client);
		
		if (row != -1)
		{
			GetModel().setValueAt(self, row, 3);
			GetModel().setValueAt(enemy, row, 4);
			GetModel().setValueAt(map, row, 5);
			GetModel().setValueAt(FrameCount, row, 6);
			GetModel().setValueAt(win, row, 7);
		}
	}
	
	public synchronized int GetClientRow(String name)
	{
		for (int r=0; r<NumRows(); ++r)
		{
			String rowName = (String)(GetModel().getValueAt(r,0));
			if (rowName.equalsIgnoreCase(name))
			{
				return r;
			}
		}
		
		return -1;
	}
	
	public void RemoveClient(String name)
	{
		int row = GetClientRow(name);
		
		if (row != -1)
		{
			GetModel().removeRow(row);
		}
		else
		{
			
		}
	}
	
	public void logText(String s)
	{
		bottomText.append(s);
		bottomText.setCaretPosition(bottomText.getDocument().getLength());
	}
	
	public int NumRows()
	{
		return GetModel().getRowCount();
	}
	
	public int RowCount()
	{
		return GetModel().getColumnCount();
	}
	
	private DefaultTableModel GetModel()
	{
		return (DefaultTableModel)(mainTable.getModel());
	}
	
	class MyRenderer extends DefaultTableCellRenderer
	{
		private static final long serialVersionUID = -6642925623417572930L;

		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column)
		{
			Component cell = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
		
			String status = (String)table.getValueAt(row, 1);
			
			if(status.equals("RUNNING"))
			{
				cell.setBackground(Color.green);
			}
			else if (status.equals("STARTING"))
			{
				cell.setBackground(Color.yellow);
			}
			else if (status.equals("READY"))
			{
				cell.setBackground(Color.white);
			}
			else if (status.equals("SENDING"))
			{
				cell.setBackground(Color.orange);
			}
			else 
			{
				 //this shouldn't happen
				cell.setBackground(Color.red);
			}
			
			return cell;
		}
	}
}
