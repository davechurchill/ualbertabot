package objects;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import server.Server;

public class ImageWindow
{
	private BufferedImage bi;
	private JFrame frame;
	private JPanel panel;
	private String address;
	
	public ImageWindow(String address, BufferedImage im)
	{
		try
		{
			this.address = address.replaceAll("/", "");
			bi = im;

            SwingUtilities.invokeLater(new Runnable()
            {
                @Override
                public void run()
                {
                    createAndShowGUI();
                }
            });
        }
        catch (Exception e)
        {
            // handle exception
        }		
	}
	
    private synchronized void createAndShowGUI()
    {
        frame = new JFrame("Client Screenshot: " + address);
                
        JMenuBar menuBar = new JMenuBar();
        JMenu fileMenu = new JMenu("File");
        JMenuItem refreshMenuItem = new JMenuItem("Refresh Screenshot", KeyEvent.VK_R);
        JMenuItem saveMenuItem = new JMenuItem("Save Screenshot as PNG", KeyEvent.VK_S);
        refreshMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	Server.Instance().sendScreenshotRequestToClient(address);
            }
        });
        fileMenu.add(refreshMenuItem);
        saveMenuItem.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            {
            	try {
            		JFileChooser fc = new JFileChooser(".");
            		int returnVal = fc.showDialog(frame, "Save");

                    if (returnVal == JFileChooser.APPROVE_OPTION) {
                        File file = fc.getSelectedFile();
                        ImageIO.write(bi, "png", file);
                    } else {
                    }

				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
            }
        });
        fileMenu.add(saveMenuItem);
        menuBar.add(fileMenu);
        frame.setJMenuBar(menuBar);
        
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        panel = new JPanel()
        {
			private static final long serialVersionUID = 1L;

			@Override
            protected void paintComponent(Graphics g)
            {
				Image biScaled = bi.getScaledInstance(getWidth(), getHeight(), BufferedImage.SCALE_SMOOTH);
                Graphics g2 = g.create();
                g2.drawImage(biScaled, 0, 0, getWidth(), getHeight(), null);
                g2.dispose();
            }

            @Override
            public Dimension getPreferredSize()
            {
                return new Dimension(bi.getWidth(), bi.getHeight());
            }
        };

        frame.add(panel);
        frame.pack();
        frame.setLocationRelativeTo(null);
        frame.setSize(1024,768);
        frame.setVisible(true);
    }
    
    public synchronized void refresh(BufferedImage im)
    {
    	bi = im;
    	frame.setSize(frame.getSize());
    	frame.setVisible(false);
    	frame.setVisible(true);
    }
}
