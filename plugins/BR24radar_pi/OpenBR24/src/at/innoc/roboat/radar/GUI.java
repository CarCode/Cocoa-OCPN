/*
 * OpenBR24 GUI - http://www.roboat.at/technologie/radar/
 *
 * Copyright (C) 2011 Adrian Dabrowski, Sebastian Busch
 * 
 * OpenBR24 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * OpenBR24 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBR24; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
package at.innoc.roboat.radar;


import javax.swing.SwingUtilities;
import java.awt.BorderLayout;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.awt.Dimension;
import java.awt.FileDialog;
import java.awt.Graphics;
import java.awt.GridBagLayout;
import java.awt.Image;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import java.awt.FlowLayout;
import javax.swing.JList;
import javax.swing.ListSelectionModel;
import java.awt.Insets;
import java.awt.Color;
import javax.swing.JLabel;
import java.awt.GridBagConstraints;
import java.awt.Font;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.UnknownHostException;
import java.util.ResourceBundle.Control;

import javax.swing.SwingConstants;

import org.omg.CORBA.portable.ApplicationException;

import at.innoc.roboat.radar.control.LiveControl;
import at.innoc.roboat.radar.control.ZoomLevel;
import javax.swing.JScrollPane;
import javax.swing.BorderFactory;
import java.awt.SystemColor;
import javax.swing.JCheckBox;
import javax.swing.JSlider;
import javax.swing.DefaultBoundedRangeModel;

/**
 * 
 * GUI to show live and recorded BR24 Data and demonstrate the use of 
 * the OpenBR24 Framework.
 * 
 * @author Adrian Dabrowski, Sebastian Busch
 * Parts of this class are generated code.
 * Open it using Eclipse Visual Editor (extension):
 * install site:  http://download.eclipse.org/tools/ve/updates/1.4/
 */
public class GUI extends JFrame implements RadarRendererListener {
	
	double timeStretch = 1.0;  // factor
	
	private at.innoc.roboat.radar.control.LiveControl control;  //  @jve:decl-index=0:
	private RadarSource radarsource;
	private RadarRenderer radarrenderer;
	
	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private JPanel jControlPanel = null;
	private JPanel jViewerPanel = null;
	private JPanel jSizePanel = null;
	private JButton jFileButton = null;
	private JButton jNetButton = null;
	private JList jZoomList = null;
	private JButton jOnButton = null;
	private JButton jOffButton = null;
	private JPanel jSeperatorPanel = null;
	private JLabel jMajorZoomLabel = null;
	private JLabel jMinorZoomLabel = null;
	private JButton jFileCloseButton = null;
	private JButton jNetCloseButton = null;
	private FileDialog filedialog;
	private JScrollPane jZoomScrollPane = null;
	private JList jRateList = null;
	private JLabel jZoomLabel = null;
	
	/**
	 * This method initializes jControlPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJControlPanel() {
		if (jControlPanel == null) {
			jSpeedLabel = new JLabel();
			jSpeedLabel.setText("Speed: 1x");
			jSpeedLabel.setVisible(false);
			jControlPanel = new JPanel();
			jControlPanel.setLayout(new FlowLayout());
			jControlPanel.setPreferredSize(new Dimension(100, 0));
			jControlPanel.add(getJSizePanel(), null);
			jControlPanel.add(getJViewCheckBox(), null);
			jControlPanel.add(getJZoomScrollPane(), null);
			jControlPanel.add(getJOnButton(), null);
			jControlPanel.add(getJOffButton(), null);
			jControlPanel.add(getJSeperatorPanel(), null);
			jControlPanel.add(getJFileButton(), null);
			jControlPanel.add(getJFileCloseButton(), null);
			jControlPanel.add(getJSpeedSlider(), null);
			jControlPanel.add(jSpeedLabel, null);
			jControlPanel.add(getJNetButton(), null);
			jControlPanel.add(getJNetCloseButton(), null);
		}
		return jControlPanel;
	}

	/**
	 * This method initializes jViewerPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJViewerPanel() {
		if (jViewerPanel == null) {
			jViewerPanel = new JPanel() {
				public void paint(Graphics g) {
					super.paint(g);
					BufferedImage i=null;
					if (radarrenderer!=null)
						i=radarrenderer.getImage();
					if (i!=null) {
						float scalex=(float)jViewerPanel.getWidth()/i.getWidth();
						float scaley=(float)jViewerPanel.getHeight()/i.getHeight();
						double realscale=scaley<scalex?scaley:scalex;
						double scale=squareview?realscale*Math.sqrt(2):realscale;

						int icorr=(int)(((float)i.getWidth()-(float)i.getWidth()/Math.sqrt(2))/2); // otherwise 0
						if (!squareview) icorr=0;

						g.drawImage(i, 0, 0, (int)(realscale*i.getWidth()), (int)(realscale*i.getHeight()),
								icorr, icorr, i.getWidth()-icorr, i.getHeight()-icorr, null);

						// minor scale circles
						//g.setColor(Color.MAGENTA);
						int midx=(int)(realscale*i.getWidth()/2);
						int midy=(int)(realscale*i.getHeight()/2);
						for (int j=minormarker;j<=(zoomlevel+zoomlevel/10);j+=minormarker) {
							int r=(int)((double)midx*j/zoomlevel);
							if (squareview) r=(int)((double)midx*j/zoomlevel*Math.sqrt(2));
							if (j==majormarker) g.setColor(Color.WHITE); 
							else g.setColor(Color.MAGENTA);
							g.drawOval(midx-r, midy-r, r+r, r+r);
						}
						// gradstriche
						g.setColor(Color.WHITE);
						for (int j=0;j<360;j+=10) {
							int r;
							if (squareview) r=(int)((double)midx*majormarker/zoomlevel*Math.sqrt(2));
							else r=midx*majormarker/zoomlevel;
							int l=(j%30)==0?28:29; // länge der striche
							g.drawLine(midx+(int)(Math.sin(Math.toRadians(j))*r),
									midy+(int)(Math.cos(Math.toRadians(j))*r),
									midx+(int)(Math.sin(Math.toRadians(j))*(r*l/30)),
									midy+(int)(Math.cos(Math.toRadians(j))*(r*l/30)));
						}
						// square view corners
						// zeigt den ausschnitt des "square view"
						if (!squareview) {
							g.setColor(Color.LIGHT_GRAY);
							int x1=midx-(int)((float)midx/Math.sqrt(2));
							int y1=midx+(int)((float)midx/Math.sqrt(2));
							// left upper
							g.drawLine(x1, x1, x1+10, x1);
							g.drawLine(x1, x1, x1, x1+10);
							// left lower
							g.drawLine(x1, y1, x1+10, y1);
							g.drawLine(x1, y1, x1, y1-10);
							// right upper
							g.drawLine(y1, x1, y1-10, x1);
							g.drawLine(y1, x1, y1, x1+10);
							// right lower
							g.drawLine(y1, y1, y1-10, y1);
							g.drawLine(y1, y1, y1, y1-10);
						}

					} // if i!=null
					/* else {
						g.fillRect(0, 0, jViewerPanel.getWidth(), jViewerPanel.getHeight());
					}*/
						
				}
			};
			jViewerPanel.setLayout(new GridBagLayout());
			jViewerPanel.setBackground(Color.gray);
		}
		return jViewerPanel;
	}

	/**
	 * This method initializes jSizePanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJSizePanel() {
		if (jSizePanel == null) {
			GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
			gridBagConstraints11.gridx = 0;
			gridBagConstraints11.anchor = GridBagConstraints.WEST;
			gridBagConstraints11.gridy = 0;
			jZoomLabel = new JLabel();
			jZoomLabel.setText("141m");
			jZoomLabel.setForeground(Color.yellow);
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.gridx = 0;
			gridBagConstraints1.anchor = GridBagConstraints.EAST;
			gridBagConstraints1.gridy = 2;
			jMinorZoomLabel = new JLabel();
			jMinorZoomLabel.setText("50m");
			jMinorZoomLabel.setForeground(Color.magenta);
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.gridy = 1;
			jMajorZoomLabel = new JLabel();
			jMajorZoomLabel.setText("100m");
			jMajorZoomLabel.setForeground(Color.white);
			jMajorZoomLabel.setFont(new Font("Dialog", Font.BOLD, 24));
			jSizePanel = new JPanel();
			jSizePanel.setLayout(new GridBagLayout());
			jSizePanel.setPreferredSize(new Dimension(100, 100));
			jSizePanel.setBackground(Color.black);
			jSizePanel.setBorder(BorderFactory.createLineBorder(SystemColor.control, 2));
			jSizePanel.add(jMajorZoomLabel, gridBagConstraints);
			jSizePanel.add(jMinorZoomLabel, gridBagConstraints1);
			jSizePanel.add(jZoomLabel, gridBagConstraints11);
		}
		return jSizePanel;
	}

	/**
	 * This method initializes jFileButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJFileButton() {
		final RadarRendererListener radarlistener=this;
		if (jFileButton == null) {
			jFileButton = new JButton();
			jFileButton.setText("Open File");
			jFileButton.setMargin(new Insets(2, 10, 2, 10));
			jFileButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					filedialog.setVisible(true);
					String fn=filedialog.getFile();
					String fdir=filedialog.getDirectory();
					if (fn!=null) {
						fn=fdir+fn;
						System.out.println("Open PCAP:"+fn);
						radarsource=new RadarPcapSource(fn, timeStretch);
						radarrenderer=new RadarRenderer(radarsource, radarlistener);
						jNetButton.setVisible(false);
						jNetCloseButton.setVisible(false);
						jFileButton.setVisible(false);
						jFileCloseButton.setVisible(true);
						jSpeedSlider.setVisible(true);
						jSpeedLabel.setVisible(true);
						jViewerPanel.repaint();
					}
				}
			});
		}
		return jFileButton;
	}

	/**
	 * This method initializes jNetButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJNetButton() {
		final RadarRendererListener radarlistener=this;
		if (jNetButton == null) {
			jNetButton = new JButton();
			jNetButton.setText("Open Live");
			jNetButton.setMargin(new Insets(2, 10, 2, 10));
			jNetButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					try {
						radarsource=new RadarLiveSource(control);
						radarrenderer=new RadarRenderer(radarsource, radarlistener);
						jNetButton.setVisible(false);
						jNetCloseButton.setVisible(true);
						jFileButton.setVisible(false);
						jFileCloseButton.setVisible(false);
						jViewerPanel.repaint();
					} catch (UnknownHostException e1) {
						e1.printStackTrace();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				}
			});
		}
		return jNetButton;
	}

	/**
	 * This method initializes jZoomList	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getJZoomList() {
		if (jZoomList == null) {
			jZoomList = new JList();
			jZoomList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			jZoomList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
						public void valueChanged(javax.swing.event.ListSelectionEvent e) {
							System.out.println("valueChanged()"); // TODO Auto-generated Event stub valueChanged()
							Object o=jZoomList.getSelectedValue();
							if (o instanceof ZoomLevel) { 
								try {
									control.setZoom(((ZoomLevel)o).getIndex());
								} catch (IOException e1) {
									e1.printStackTrace();
								}
							}
						}
					});
			jZoomList.setListData(control.getAvailableZoomSettings());
		}
		return jZoomList;
	}

	/**
	 * This method initializes jOnButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJOnButton() {
		if (jOnButton == null) {
			jOnButton = new JButton();
			jOnButton.setText("On");
			jOnButton.setMargin(new Insets(2, 2, 2, 2));
			jOnButton.setPreferredSize(new Dimension(40, 26));
			jOnButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					try {
						control.Power(true);
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				}
			});
		}
		return jOnButton;
	}

	/**
	 * This method initializes jOffButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJOffButton() {
		if (jOffButton == null) {
			jOffButton = new JButton();
			jOffButton.setText("Off");
			jOffButton.setMargin(new Insets(2, 2, 2, 2));
			jOffButton.setPreferredSize(new Dimension(40, 26));
			jOffButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					try {
						control.Power(false);
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				}
			});
		}
		return jOffButton;
	}

	/**
	 * This method initializes jSeperatorPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJSeperatorPanel() {
		if (jSeperatorPanel == null) {
			jSeperatorPanel = new JPanel();
			jSeperatorPanel.setLayout(new GridBagLayout());
			jSeperatorPanel.setPreferredSize(new Dimension(100, 100));
		}
		return jSeperatorPanel;
	}

	/**
	 * This method initializes jFileCloseButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJFileCloseButton() {
		if (jFileCloseButton == null) {
			jFileCloseButton = new JButton();
			jFileCloseButton.setText("Close File");
			jFileCloseButton.setVisible(false);
			jFileCloseButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					jNetButton.setVisible(true);
					jNetCloseButton.setVisible(false);
					jFileButton.setVisible(true);
					jFileCloseButton.setVisible(false);
					jSpeedSlider.setVisible(false);
					jSpeedLabel.setVisible(false);
					radarsource.close();				
				}
			});
		}
		return jFileCloseButton;
	}

	/**
	 * This method initializes jNetCloseButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJNetCloseButton() {
		if (jNetCloseButton == null) {
			jNetCloseButton = new JButton();
			jNetCloseButton.setText("Close Live");
			jNetCloseButton.setVisible(false);
			jNetCloseButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					jNetButton.setVisible(true);
					jNetCloseButton.setVisible(false);
					jFileButton.setVisible(true);
					jFileCloseButton.setVisible(false);
					radarsource.close();
				}
			});
		}
		return jNetCloseButton;
	}

	/**
	 * This method initializes jZoomScrollPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getJZoomScrollPane() {
		if (jZoomScrollPane == null) {
			jZoomScrollPane = new JScrollPane();
			jZoomScrollPane.setPreferredSize(new Dimension(100, 90));
			jZoomScrollPane.setViewportView(getJZoomList());
		}
		return jZoomScrollPane;
	}

	boolean squareview=true;
	/**
	 * This method initializes jViewCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getJViewCheckBox() {
		if (jViewCheckBox == null) {
			jViewCheckBox = new JCheckBox();
			jViewCheckBox.setText("Square View");
			jViewCheckBox.setFont(new Font("Dialog", Font.PLAIN, 12));
			jViewCheckBox.setSelected(squareview);
			jViewCheckBox.addItemListener(new java.awt.event.ItemListener() {
				public void itemStateChanged(java.awt.event.ItemEvent e) {
					squareview=jViewCheckBox.isSelected();
					jViewerPanel.repaint();
				}
			});
		}
		return jViewCheckBox;
	}

	/**
	 * This method initializes jSpeedSlider	
	 * 	
	 * @return javax.swing.JSlider	
	 */
	private JSlider getJSpeedSlider() {
		if (jSpeedSlider == null) {
			jSpeedSlider = new JSlider();
			jSpeedSlider.setPreferredSize(new Dimension(100, 27));
			jSpeedSlider.setMaximum(5);
			jSpeedSlider.setSnapToTicks(true);
			jSpeedSlider.setValue(0);
			jSpeedSlider.setPaintTicks(true);
			jSpeedSlider.setPaintLabels(true);
			jSpeedSlider.setMinorTickSpacing(1);
			jSpeedSlider.setMinimum(-5);
			jSpeedSlider.setVisible(false);
			jSpeedSlider.addChangeListener(new javax.swing.event.ChangeListener() {
				public void stateChanged(javax.swing.event.ChangeEvent e) {
					
					double timeFactor=Math.pow(2,jSpeedSlider.getValue());
					timeStretch=1/timeFactor;
					if (radarsource instanceof RadarPcapSource) 
						((RadarPcapSource)radarsource).setTimeStretch(timeStretch);
					jSpeedLabel.setText("Speed: "+timeFactor+"x");
				}
			});
		}
		return jSpeedSlider;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				GUI thisClass = new GUI();
				thisClass.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
				thisClass.setVisible(true);
			}
		});
	}

	/**
	 * This is the default constructor
	 */
	public GUI() {
		super();
		op_init();
		initialize();
	}

	
	public void op_init() {
		
		try {
			control=new LiveControl();
			control.open();
		} catch (IOException e) {
			System.out.println(e.toString());
			e.printStackTrace();
		}
		
	}
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(700, 620);
		this.setContentPane(getJContentPane());
		this.setTitle("InnoC OpenBR24 Busch/Dabrowski Radar GUI");
		this.addWindowListener(new java.awt.event.WindowAdapter() {
			public void windowClosing(java.awt.event.WindowEvent e) {
				System.out.println("windowClosing()"); // TODO Auto-generated Event stub windowClosing()
				System.exit(0);
			}
		});
		filedialog=new FileDialog(this,"Open PCAP",FileDialog.LOAD);
		filedialog.setModal(true);
		filedialog.setFilenameFilter(new FilenameFilter() {
			public boolean accept(File f, String name) {
				return name.endsWith(".pcap") || name.endsWith(".cap") || name.endsWith(".capture") || f.isDirectory();
			}});
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BorderLayout());
			jContentPane.add(getJControlPanel(), BorderLayout.WEST);
			jContentPane.add(getJViewerPanel(), BorderLayout.CENTER);
		}
		return jContentPane;
	}


	@Override
	public void onImageUpdate(int fromAngle, int toAngle, int x, int y, int w,
			int h) {
		
		Image i=radarrenderer.getImage();
		
		float scalex=(float)jViewerPanel.getWidth()/i.getWidth(null);
		float scaley=(float)jViewerPanel.getHeight()/i.getHeight(null);
		float scale=scaley<scalex?scaley:scalex;
		//scale*=Math.sqrt(2);
		
		jViewerPanel.repaint((int)(scale*x-1), (int)(scale*y-1), (int)(scale*w+1), (int)(scale*h+1));
		
	}

	
	int minormarker=25;
	int majormarker=50;
	int zoomlevel=73;

	private JCheckBox jViewCheckBox = null;

	private JSlider jSpeedSlider = null;

	private JLabel jSpeedLabel = null;

	@Override
	public void onZoomChange(int meter) {
		System.out.println("ZoomChange");
		zoomlevel=meter;
		jZoomLabel.setText(meter+"m");
		if (zoomlevel<=150) minormarker=25;
		else if (zoomlevel<=300) minormarker=50;
		else if (zoomlevel<=700) minormarker=100;
		else if (zoomlevel<=1300) minormarker=250;
		else if (zoomlevel<=2700) minormarker=500;
		else if (zoomlevel<=7000) minormarker=1000;
		else if (zoomlevel<=15000) minormarker=2500;
		else if (zoomlevel<=25000) minormarker=5000;
		else minormarker=10000;
		
		majormarker=minormarker*(int)((double)zoomlevel/Math.sqrt(2)/minormarker);
		
		jMajorZoomLabel.setText(majormarker+"m");
		jMinorZoomLabel.setText(minormarker+"m");
	}
	
	public void onClose() {
		jNetButton.setVisible(true);
		jNetCloseButton.setVisible(false);
		jFileButton.setVisible(true);
		jFileCloseButton.setVisible(false);
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
