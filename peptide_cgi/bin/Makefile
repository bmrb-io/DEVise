testusage:
	bmrb_vis.pl -usage

testver:
	bmrb_vis.pl -v

testmin:
	bmrb_vis.pl ../cgi-bin/test_data/minimal_3.0.str

test4267:
	bmrb_vis.pl ../cgi-bin/test_data/bmr4267_3.str

testnofile:
	bmrb_vis.pl non-existant_file

testbadfile:
	bmrb_vis.pl Makefile

testopera:
	bmrb_vis.pl -b opera ../cgi-bin/test_data/minimal_3.0.str

testcurl:
	curl --form fileupl=@../cgi-bin/test_data/minimal_3.0.str http://manatee.bmrb.wisc.edu/vis_serv/srv.php

