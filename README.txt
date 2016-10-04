
######################################################################
##### Open sourced by IBEX, an electronic product design company #####
#####    based in South East England.  http://www.ibexuk.com     #####
######################################################################

This project was previously available from our http://www.embedded-code.com source code store (owned by IBEX UK Ltd) and is a fully packaged and documented project.  After several years selling these source code projects to developers we took the decision to finally open source all of the projects hosted on the site.

The projects original page on the embedded-code.com web site can be found at: http://www.embedded-code.com/source-code/rfid/125khz-rfid-readers/htrc110-em4102-rfid-reader-driver


-------------------------
----- KNOWN ISSUES ------
-------------------------
There is a known issue with the driver which relates to its use with BiPhase Modulation RFID tags (which in practice is very few 125kHz RFID tags as it is not the widely used modualtion type).  It's not hard to solve but we've not had a need to use BiPhase Modulation tags so haven't made the fix ourselves - contribution welcome.

The issue is here:
//For BiPhase Modulation:
//	If rfid_carry=0
//		When rfid_bit_type=100 the next bit is 0
//		When rfid_bit_type=150 the next bit is 1, set rfid_carry <<<< THIS IS FLAWED - you don't know if a 1 or 0 just occured - fix required
//		When rfid_bit_type=200 the next bits are 11
//	If rfid_carry=1
//		When rfid_bit_type=100 the next bit is 0, don't clear rfid_carry
//		When rfid_bit_type=150 the next bits are 01, clear rfid_carry
//		When rfid_bit_type=200 - not possible
//(When rfid_carry=1 we are out of phase with the bitrate period)