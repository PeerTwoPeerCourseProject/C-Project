import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;


import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;


import java.io.*;
import java.net.*;




public class DHTjava extends JFrame implements ActionListener{
	/**
	 * DHTjava is GUI class for file transfer to DHT
	 * Idea is to connect through socket with DHT node and in that way handle file transfers;
	 */
	private static final long serialVersionUID = 1L;
	public JPanel cpanel = new JPanel();
	public JLabel port_label = new JLabel("GIVE PORT NUMBER:");
	public JButton connect_bnt = new JButton("CONNECT");
	public JTextField field = new JTextField("");
	
	public JPanel panel = new JPanel();
	public JPanel right_panel = new JPanel();
	public JButton send_btn = new JButton("SEND FILE TO DHT");
	public JButton receive_btn = new JButton("RECEIVE FILE FROM DHT");
	public JButton dump_btn = new JButton("DUMP DATA FROM DHT");
	public JButton close_fra_btn = new JButton("Close Window");
	public JTextArea inf = new JTextArea("");
	public JMenuItem quit;
	public String text = "";
	
	public JButton sendToDHTBtn = new JButton("Send File To DHT");
	public JButton receiveDHTBtn = new JButton("Receive File From DHT");
	public JButton dumpDHTbtn = new JButton("Dump File From DHT");
	public JProgressBar progressBar;

	
	Socket soc = null;
	ServerSocket servsock = null;
	DataInputStream in = null;
	DataOutputStream out = null;
	File fi = null;
	byte[] hashChek = null;
	JFrame fra;
	JPanel pan;


	public DHTjava(){
		/**
		 * Initiation method
		 */
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		// Default Close Operation
		this.setBtnListeners();								// Action listener are add to buttons
		this.setLocationRelativeTo(null);					// Relative location is set null witch opens window in middle of the screen
		this.connectPanel();								
		
	}
	
	public void connectPanel(){
		/**
		 * connectionPanel() opens JFrame witch ask port number for DHT java <-> c node connection
		 */
		cpanel.setLayout(new GridLayout(3, 1, 15, 15));
		cpanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		cpanel.add(this.port_label, 0);
		cpanel.add(this.field, 1);
		cpanel.add(this.connect_bnt, 2);
		this.setSize(300, 300);
		this.setTitle("DHT NODE HANDELER");
		this.add(cpanel);
	}
	public void controllPanel(){
		/**
		 * controllPanel() transforms JFrame from connectionPanel() and opens main user interface
		 */
		cpanel.removeAll();
		this.make_menu();
		this.setSize(700, 400);
		cpanel.setLayout(new GridLayout(1, 2, 15, 15));
		cpanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		this.make_right_panel();
		inf.setText(text);
		cpanel.add(right_panel);
		cpanel.add(inf);
		cpanel.validate();
	}
	
	public void make_right_panel(){
		/**
		 * make_right_panel() makes panel for three buttons
		 */
		right_panel.setLayout(new GridLayout(3, 1, 15, 15));
		right_panel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		right_panel.add(this.send_btn);
		right_panel.add(this.receive_btn);
		right_panel.add(this.dump_btn);
	}
	public void init_progresBar(){
		/**
		 * init_progresBar() is initial method when progress bar is used
		 */
		 progressBar = new JProgressBar();
	     progressBar.setMaximumSize(new Dimension(150, 20));
	     progressBar.setMinimumSize(new Dimension(150, 20));
	     progressBar.setPreferredSize(new Dimension(150, 20));

	     progressBar.setAlignmentX(0f);
		
	}
	
	public File fileChooser(String info){
		/**
		 * fileChooser() opens file chooser dialog
		 */
		JFileChooser fileopen = new JFileChooser();
		int ret = fileopen.showDialog(this, info);
		if (ret == JFileChooser.APPROVE_OPTION){
			return fileopen.getSelectedFile();
		}
		return null;
	}
	
	public void send_handeller(){
		/**
		 * send_handller() opens JFrame witch is used to control file sending for DHT
		 */
		fi = this.fileChooser("Choose File");
		if (fi == null)return;
		fra = new JFrame();
		pan = new JPanel();
		pan.setLayout(new GridLayout(3, 2, 15, 15));
		pan.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		this.init_progresBar();
		pan.add(new JLabel("FILE PATH:"));
		pan.add(new JLabel(fi.toString()));
		pan.add(new JLabel("Give Saving Name:"));
		pan.add(field);
		field.setText(fi.getName());
		pan.add(this.sendToDHTBtn);
		pan.add(this.progressBar);
		pan.validate();
		fra.add(pan);
		fra.validate();
		fra.pack();
		fra.setVisible(true);
		fra.setLocationRelativeTo(null);

	}
	public void receive_handeler(){
		/**
		 * receive_handeler() opens JFrame witch is used to control file receiving from DHT
		 */
		fra = new JFrame();
		pan = new JPanel();
		pan.setLayout(new GridLayout(3, 1, 15, 15));
		pan.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		this.init_progresBar();
		pan.add(field);
		pan.add(this.progressBar);
		field.setText("");
		pan.add(this.receiveDHTBtn);
		
		fra.add(pan);
		
		fra.pack();
		fra.validate();
		
		fra.setVisible(true);
		fra.pack();
		fra.setLocationRelativeTo(null);
	}
	public void dump_handeler(){
		/**
		 * dump_handeler() opens JFrame witch is used to control dumps
		 */
		fra = new JFrame();
		pan = new JPanel();
		pan.setLayout(new GridLayout(3, 1, 15, 15));
		pan.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		
		pan.add(field);
		field.setText("");
		pan.add(this.dumpDHTbtn);
		this.init_progresBar();
		pan.add(this.progressBar);
		
		fra.add(pan);

		fra.pack();
		fra.validate();
		
		fra.setVisible(true);this.setLocationRelativeTo(null);
		fra.pack();
		fra.setLocationRelativeTo(null);
	}
	public void consoleText(String str){
		/**
		 * consoleText() is used to give messages for user
		 * consoleText() show messages in main GUI console/text field
		 */
		text = str + "\n" + text;
		this.inf.setText(text);
	}
	
	
	public void doConnection(int port){
		/**
		 * doConnection() handles connection start between DHT java and DHT node (c node) 
		 */
		try{
			servsock = new ServerSocket(port);
			soc = servsock.accept();
			out = new DataOutputStream(soc.getOutputStream());
            in = new DataInputStream(soc.getInputStream());
            this.consoleText("Connection has been established");

		}catch (UnknownHostException e){
			this.consoleText("UnknownHostError "+e.getMessage());
		}catch (IOException e) {
			this.consoleText("IOExeption "+e.getMessage());
		}
	}
	
	
	public void setBtnListeners(){
		/**
		 * setBtnListener() add all buttons action listeners
		 * all actions listeners are this because DHTjava implements actionlistener
		 */
		this.connect_bnt.addActionListener(this);
		this.dump_btn.addActionListener(this);
		this.send_btn.addActionListener(this);
		this.receive_btn.addActionListener(this);
		this.sendToDHTBtn.addActionListener(this);
		this.receiveDHTBtn.addActionListener(this);
		this.dumpDHTbtn.addActionListener(this);
		this.close_fra_btn.addActionListener(this);
	}

	public void make_menu(){
		/**
		 * make_menu() makes menu bar for main GUI and adds all menu items action listeners
		 */
		JMenuBar menuBar = new JMenuBar();
		JMenu menu = new JMenu("DHT JAVA");
		menuBar.add(menu);
		quit = new JMenuItem("Quit");
		quit.addActionListener(this);
		menu.add(quit);
		this.setJMenuBar(menuBar);
	}

	public void btn_actions(ActionEvent push){
		/**
		 * btn_actions() handles all buttons actions
		 */
		JButton btn = (JButton) push.getSource();
		if (btn == this.connect_bnt){
			if (this.field.getText().equals("")) return;
			int port = Integer.parseInt( field.getText());
			this.controllPanel();
			this.doConnection(port);
		}
		else if (btn == this.close_fra_btn){
			fra.setVisible(false);
		}
		else if (btn == this.send_btn){
			this.send_handeller();
		}
		else if (btn == this.dump_btn){
			this.dump_handeler();
		}
		else if (btn == this.receive_btn){
			this.receive_handeler();
		}
		else if (btn == this.sendToDHTBtn){
			/**
			 * send data is to DHT is implemented hear
			 * 
			 * socket packet send packet 	=	 1 + hash + block length + block data
			 * socket packet receive packet =	 1
			 * 
			 * send data to DHT supports multiple block sized files
			 */
			try{
				
				if (fi.exists() == false){
					pan.remove(4);
					pan.add(this.close_fra_btn, 4);
					pan.validate();
					return;
				}
				if (fi == null) {
					pan.remove(4);
					pan.add(this.close_fra_btn, 4);
					pan.validate();
					return;
					
				}
				int no_of_files = (int)fi.length() / 10000;
				int lastfilesize = (int)fi.length() % 10000;
				int counter = 1;
				int bulbasaur = no_of_files + 1;
				FileInputStream stream = new FileInputStream(fi);
			
				while (no_of_files > 0){
					
					out.write(1);
					
					this.hashChek =SHA1.SHA_1((field.getText()+Integer.toString(counter))); 
					out.write(this.hashChek);
				
					out.write(SHA1.int_to_byte(10000));
					
					byte[] temp = new byte[10000];
					stream.read(temp);
					out.write(temp);
					
					
					this.consoleText("Packet sended to C Node: "+field.getText()+" "+Integer.toString(counter)+"/"+Integer.toString(bulbasaur));
					byte[] btype = new byte[2];
					in.read(btype);
					
					pan.validate();
					counter++;
					no_of_files--;
				}
				
				out.write(1);
				
				this.hashChek = SHA1.SHA_1(field.getText()+Integer.toString(0));
				out.write(this.hashChek);
				
				out.write(SHA1.int_to_byte(lastfilesize));
				
				byte[] temp = new byte[lastfilesize];
				stream.read(temp);
				out.write(temp);
				
				stream.close();
				
				
				this.consoleText("Packet sended to C Node: "+field.getText());
				byte[] btype = new byte[2];
				in.read(btype);
				this.progressBar.setValue(100);
				pan.remove(4);
				pan.add(this.close_fra_btn, 4);
				pan.validate();
				if (SHA1.bytes_to_int(btype) == 1){
					this.consoleText("DHT has received file "+field.getText());
					this.consoleText("Everything is ok");
					this.consoleText("File: "+field.getText()+" has been saved to DHT");
				}
				else {
					this.consoleText("\nHouston, we have a problem\n");
					this.consoleText("File is not saved to DHT");
				}
				}	
		catch (IOException e){
				this.consoleText("IOExeption: Some thing gone wrong\n"+e.getMessage());
				pan.remove(4);
				pan.add(this.close_fra_btn, 4);
				pan.validate();
			}
		}
		else if (btn == this.receiveDHTBtn){
			/**
			 * receive data for DHT is implemented hear
			 * 
			 * socket packet send packet 	=	 2 + hash
			 * socket packet receive packet =	 3 + data length + hash
			 * 
			 * receive data from DHT support multiple block sized data/files
			 */
			try{	
				fi = this.fileChooser("Save file");
				if (fi==null){
					pan.remove(2);
					pan.add(this.close_fra_btn, 2);
					this.consoleText("ERROR: No file store path");
					pan.validate();
					return;
				}
				
				FileOutputStream fos = new FileOutputStream(fi);
				
				int pikachu = 3;
				int counter = 1;
				
				while (pikachu > 0){
					
					out.write(2);
					
					this.hashChek =SHA1.SHA_1(field.getText()+Integer.toString(counter)); 
					out.write(this.hashChek);
					
					byte[] btype = new byte[2];
					in.read(btype);
					
					if (SHA1.bytes_to_int(btype) == 4){
						pikachu--;
					}
					
					else {
						byte[] tmp = new byte[2];
						in.read(tmp);
						int size = SHA1.bytes_to_int(tmp);
						byte[] incoming = new byte[size];
						in.read(incoming);
						
						fos.write(incoming);
						pikachu = 3;
					}
					
					counter++;
					
				}
				
				
				out.write(2);
				this.hashChek =SHA1.SHA_1(field.getText()+Integer.toString(0)); 
				out.write(this.hashChek);
				byte[] btype = new byte[2];
				in.read(btype);
				if (SHA1.bytes_to_int(btype) == 4){
					this.consoleText("Not found from DHT");
					pan.remove(2);
					pan.add(this.close_fra_btn, 2);
					pan.validate();
					fos.close();
					return;
				}
				byte[] tmp = new byte[2];
				in.read(tmp);
				int size = SHA1.bytes_to_int(tmp);
				this.consoleText("FILE RECEIVED");
				byte[] incoming = new byte[size];
				in.read(incoming);
				this.progressBar.setValue(100);

				fos.write(incoming);
				fos.close();
				
				this.consoleText("File is saved to location: "+ fi.toString());
				this.consoleText("Everything is ok");
				pan.remove(2);
				pan.add(this.close_fra_btn, 2);
				pan.validate();
			}catch (IOException e){
				this.consoleText("IOExeption: Some thing gone wrong\n"+e.getMessage());
				pan.remove(2);
				pan.add(this.close_fra_btn, 2);
				pan.validate();
				
			}
		}
		if (btn == this.dumpDHTbtn){
			/**
			 * dump is implement data dump
			 * socket packet send  		 =	 3 + hash
			 * socket packet receive	 =	 5
			 * 
			 * dump send dump only for zero data block
			 */
			try{
				out.write(3);
				this.progressBar.setValue(30);
				this.hashChek =SHA1.SHA_1(field.getText()+0); 
				out.write(this.hashChek);
				this.consoleText("Dump send to C Node: "+field.getText());
				this.progressBar.setValue(70);
				byte[] btype = new byte[2];
				in.read(btype);
				this.progressBar.setValue(100);
				if (SHA1.bytes_to_int(btype) == 5){
					pan.remove(1);
					pan.add(this.close_fra_btn, 1);
					pan.validate();
					this.consoleText("File has been dumbed from DHT");
				}
				else{
					pan.remove(1);
					pan.add(this.close_fra_btn, 1);
					pan.validate();
					this.consoleText("File was not foud or couldn't be dumped from DHT");
				}
			}catch (IOException e){
				this.consoleText("IOException: "+e.getMessage());
				pan.remove(1);
				pan.add(this.close_fra_btn, 1);
				pan.validate();
			}
		}
	}
	public void menu_action(ActionEvent push){
		/**
		 * menu_action() handles menu actions
		 */
		JMenuItem item = (JMenuItem) push.getSource();
		if (item == this.quit)
			System.exit(0);					// Exit programs
	}
	
	@Override
	public void actionPerformed(ActionEvent push) {
		/**
		 * actionPerformed handles all actions
		 * actionPerformed checks if action is from button or menu and forwards it to right methods
		 */
		if (push.getSource() instanceof JMenuItem)
			menu_action(push);
		else if (push.getSource() instanceof JButton)
			btn_actions(push);
	}
	
	
	public static void main(String[] args) {
		/**
		 * main starts program
		 * In main program is started by SwingUtilities
		 */
		SwingUtilities.invokeLater(new Runnable() {
            public void run() {
            	DHTjava ex = new DHTjava();
                ex.setVisible(true);
            }
        });

	}
}
