package DataSources;

/* This class correcponds to the C++ files RelationId.[ch] */

import java.io.*;

public class RelationId {
  private int serverId;
  private int localId;

  static final int DTE_SERVER_ID = 0;

  public RelationId(){ 
    serverId = DTE_SERVER_ID;
    localId = 0 ;
  }

  public RelationId(int c){
    serverId = DTE_SERVER_ID;
    localId = c;
  }

  public RelationId(int s, int c){
    serverId = s;
    localId  = c;
  }
  
  public int getServerId() {
    return serverId;
  }

  public int getLocalId()  {
    return localId;
  }
      
  public String getString() {
    return new String("ServerId :" + serverId + " LocalId:"+ localId);
  }    

  public void print() {
    String str = getString();
    System.out.println(str);
  }

 
}


