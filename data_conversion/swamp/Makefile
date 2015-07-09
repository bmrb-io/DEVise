all:
	javac Anonymizer.java

clean:
	rm Anonymizer.class

test:
	java -cp . Anonymizer example.csv converted.csv

testf1:
	java -cp . Anonymizer doesnt_exist.csv converted.csv

testf2:
	java -cp . Anonymizer example.csv
