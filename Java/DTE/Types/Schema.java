
class Schema {
	TypeID* typeIDs;
	string* attributeNames;
	int numFlds;
public:
	Schema() : typeIDs(NULL), attributeNames(NULL), numFlds(0) {}
	Schema(const string& str); 
	Schema(TypeID* typeIDs, string* attributeNames, int numFlds) :
		typeIDs(typeIDs),
		attributeNames(attributeNames), 
		numFlds(numFlds) {}
	Schema(int numFlds, const TypeID* typeIDs, const string* attributeNames); 
	Schema(const Schema& x);
	Schema& operator=(const Schema& x);
	Schema operator+(const Schema& x) const;
	~Schema(){
	//	delete [] typeIDs;
	//	delete [] attributeNames;	// causing core dumps?
	}
	istream& read(istream& in); // Throws Exception
	void write(ostream& out);
	int getNumFlds() const {
		return numFlds;
	}
	const string* getAttributeNames() const {
		assert(attributeNames);
		return attributeNames;
	}
	const TypeID* getTypeIDs() const {
		assert(typeIDs);
		return typeIDs;
	}
	static const Schema* getSchema(const Type* arg){
		return (const Schema*) arg;
	}
	friend istream& operator>>(istream& in, Schema& s);
	friend ostream& operator<<(ostream& out, const Schema& s);
};
