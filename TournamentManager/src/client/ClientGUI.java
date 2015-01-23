
package client;

import java.awt.*;

import javax.swing.*;

import java.awt.Color;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.table.*;


public class ClientGUI 
{
	Client client;
    static JFrame 		mainFrame;
    static JTable 		mainTable;
	static JTextArea	textArea;
	
	static JPanel		rightPanel;
	
	String [] columnNames = {"Client", "Status", "State", "StarCraft", "Function", "Data"};
	
	Object [][] data = 	{
							
						};

	public ClientGUI(Client client)
	{
		this.client = client;
		CreateGUI();
	}
	
	public void CreateGUI()
	{
		mainTable 	= new JTable(new DefaultTableModel(data, columnNames));
		mainTable.setDefaultRenderer(Object.class, new MyRenderer());
		textArea	= new JTextArea();
		mainFrame 	= new JFrame("StarCraft AI Tournament");
		mainFrame.setLayout(new GridLayout(2,0));
		
		rightPanel = new JPanel();
		rightPanel.setLayout(new GridLayout(1,0));
		rightPanel.add(new JScrollPane(textArea));
	
        mainTable.setFillsViewportHeight(true);
	
	    mainFrame.setSize(800,600);
	    mainFrame.add(new JScrollPane(mainTable));
		mainFrame.add(rightPanel);
	    mainFrame.setVisible(true);
	    
		mainFrame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		
		mainFrame.addWindowListener(new WindowAdapter() 
	    {
	    	public void windowClosing(WindowEvent e) 
	    	{
    			int confirmed = JOptionPane.showConfirmDialog(mainFrame, "Shutdown Client: Are you sure?", "Shutdown Confirmation", JOptionPane.YES_NO_OPTION);
    			if (confirmed == JOptionPane.YES_OPTION)
    			{
					shutDown();
    			}     
            }
	    });
	}
	
	private void shutDown()
	{
		mainFrame.dispose();
		client.shutDown();
	}
	
	public void UpdateClient(String name, String status, String num, String host, String join, String data)
	{
		int row = GetClientRow(name);
		
		// if it exists, update it
		if (row != -1)
		{
			GetModel().setValueAt(status, row, 1);
			GetModel().setValueAt(name, row, 0);
			GetModel().setValueAt(num, row, 2);
			GetModel().setValueAt(host, row, 3);
			GetModel().setValueAt(join, row, 4);
			GetModel().setValueAt(data, row, 5);
		}
		// otherwise add a new row
		else
		{
			GetModel().addRow(new Object[]{name, status, num, host, join, "", data});
		}
		
		mainTable.scrollRectToVisible(mainTable.getCellRect(mainTable.getRowCount()-1, 0, true));
	}
	
	public void UpdateFunc(String func)
	{
		int row = GetClientRow("Me");
		
		// if it exists, update it
		if (row != -1)
		{
			GetModel().setValueAt(func, row, 4);
		}
		// otherwise add a new row
		else
		{
			
		}
	}
	
	public int GetClientRow(String name)
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
		
		// if it exists, update it
		if (row != -1)
		{
			GetModel().removeRow(row);
		}
		// otherwise add a new row
		else
		{
			
		}
	}
	
	public void logText(String s)
	{
		textArea.append(s);
		textArea.setCaretPosition(textArea.getDocument().getLength());
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
		private static final long serialVersionUID = -5043200306716386093L;

		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected,
				boolean hasFocus, int row, int column)
		{
			Component cell = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
		
			String status = (String)table.getValueAt(row, column);
			
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
			else if (status.equals("STATEFILE"))
			{
				cell.setBackground(Color.green);
			}
			else if (status.equals("NOSTATE"))
			{
				cell.setBackground(Color.white);
			}
			else if (status.equals("STARCRAFT"))
			{
				cell.setBackground(Color.green);
			}
			else if (status.equals("NOSTARCRAFT"))
			{
				cell.setBackground(Color.white);
			}
			else
			{
				cell.setBackground(Color.white);
			}
			
			return cell;
		}
	}
	
}
