all:
	javac *.java

clean:
	rm *.class

test:
	java -cp . Aggregator 339GIS_2006.csv converted.csv 60 60

testf1:
	java -cp . Aggregator doesnt_exist.csv converted.csv 60 60

test2:
	java -cp . Aggregator 339GIS_2006.csv
