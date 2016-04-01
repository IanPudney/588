import java.net.*; 
import java.io.*; 
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.Writer;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class EchoServer 
{ 
 public static void main(String[] args) throws IOException 
   { 
       while(true)
       {
            // Collect the session start time for use in log file names.
            Calendar cal = Calendar.getInstance();
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            String session_dt = sdf.format(cal.getTime());
        
            ServerSocket serverSocket = null;
            try { 
                serverSocket = new ServerSocket(9505); 
            } 
            catch (IOException e) 
            { 
                System.err.println("Could not listen on port: 10007."); 
                System.exit(1); 
            } 

            Socket clientSocket = null; 
            System.out.println ("Waiting for connection.....");

            try { 
                clientSocket = serverSocket.accept(); 
            } 
            catch (IOException e) 
            { 
                System.err.println("Accept failed."); 
                System.exit(1); 
            } 

            System.out.println ("Connection successful");
            System.out.println ("Waiting for input.....");

            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), 
                                              true); 
            BufferedReader in = new BufferedReader( 
                    new InputStreamReader( clientSocket.getInputStream())); 

            String inputLine; 

            // Collect messages. Store to disk.
            while ((inputLine = in.readLine()) != null) 
            { 
                System.out.println ("Server: " + inputLine); 
                 
                Writer output = new BufferedWriter(new FileWriter("log " + session_dt, true));
                output.append(inputLine + "\n");
                output.close();
            }
            
            // Cleanup.
            out.close(); 
            in.close(); 
            clientSocket.close(); 
            serverSocket.close();
        }
    } 
} 
