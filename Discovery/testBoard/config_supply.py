#!/usr/bin/python
# -*- coding: utf-8 -*-
# @author Laurent Sauvage <laurent.sauvage@telecom-paristech.fr>

# -----------------
# Script parameters
# -----------------
POW_DEV="/dev/ttyUSB0"      # Device for the power supply

# Campaign parameters
volt=1.12

# -------
# Imports
# -------
import sys
import logging
import time


if len( sys.argv ) != 5:
	raise Exception( "Usage: %s <Supply Select> (6|25) <Volt> )"%sys.argv[0] )
	# Example of argv[1]: rsm_sr/rsm_hocpa_sigma0.000000_M2_16_d3.csv

supply1 = 'P'+sys.argv[1]+'V'
print supply1
volt1 = float( sys.argv[2] )

supply2 =  'P'+sys.argv[3]+'V'
print supply2
volt2 = float( sys.argv[4] )

# pyAnalyzer libraries
import E3631A as E3631A

try:
   # -----------
   # Preparation 
   # -----------
   logging.basicConfig(level= logging.INFO) # Logging level
   power= E3631A.E3631A(POW_DEV)            # Creates the power supply object
   power.set_output("ON")                   # Enables all outputs (at 0.0 V after creation)

   # -----
   #  GO !
   # -----
   logging.info("Please Take Care")
   logging.info("The Communication Between the PC and Supply is Unidirectional PC -> Supply")
   logging.info("The RS-232 Cable should be Cross")
   logging.info("New round with {0:1.2f} V".format(volt))
   # Updates the 6V or 25 V power supply output
   power.apply(supply1, volt1)
   power.apply(supply2, volt2)
   time.sleep(1.0) # Wait a little for the power to be stabilized

except Exception as detail:
   print "ERROR: %s" % (detail)
   sys.exit(1)
