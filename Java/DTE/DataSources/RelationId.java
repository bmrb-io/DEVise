package DataSources;

/* This class correcponds to the C++ files RelationId.[ch] */
/** This class implements the relation id of each relation, which consists
    of serverid and localid.
    */

import java.io.*;
import java.lang.*;

public class RelationId {
  private int serverId;
  private int localId;

  static final int DTE_SERVER_ID = 1;

  /** constructor RelationId() with default ServerId 1 and LocalId 0. */
  public RelationId(){ 
    serverId = DTE_SERVER_ID;
    localId = 0 ;                //LocalId starts from 0
  }

  /** constructor RelationId(int) with default ServerId 1 and given LocalId
      as argument. */
  public RelationId(int c){
    serverId = DTE_SERVER_ID;
    localId = c;
  }

  /** constructor(int, int) with the given ServerId and LocalId. */
  public RelationId(int s, int c){
    serverId = s;
    localId  = c;
  }
  
  /** This method returns the ServerId of this relation. */
  public int getServerId() {
    return serverId;
  }

  /** This method returns the LocalId of this relation. */
  public int getLocalId()  {
    return localId;
  }
      
  /** This method returns the string representation of this relationid. */
  public String getString() {
    return new String("ServerId :" + serverId + " LocalId:"+ localId);
  }    

  /** This method overwrites the toString() method of the String class. */
  public String toString(){
    return String.valueOf( serverId ) + "." + String.valueOf( localId );
  }  
  
}


