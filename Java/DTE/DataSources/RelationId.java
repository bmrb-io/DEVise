package DataSources;

/* This class correcponds to the C++ files RelationId.[ch] */

class RelationId {
     int serverId;
     int localId;
public:
     RelationId() : serverId(0), localId(0) {}
     RelationId(int s, int l) : serverId(s), localId(l) {}
     int getServerId() const {
          return serverId;
     }
     int getLocalId() const {
          return localId;
     }
     string toString() const;
     friend ostream& operator<<(ostream&, const RelationId&);
}
