#!/usr/bin/perl

use strict;
use warnings;
use Cwd;
#my ($args,) = @ARGV;
my $numArgs = $#ARGV + 1;
print "thanks, you gave me $numArgs command-line arguments.\n";


my $LOAD="";

chomp($LOAD);

if(lc($ARGV[0]) eq "pgm"){
	print "chargement de l'elf sur la carte ... ";
	$LOAD= "";
}

else{
	$LOAD= "";
}


my $file = $ARGV[1];
my $binary = $ARGV[2];

my $HB1_line	= `sed -n '/GPIO_Write(GPIOD, 0xFF)/=' main.c` ;
$HB1_line ++; # On place le breakpoint post-macro apres le signal de triger
chomp($HB1_line);
print $HB1_line."\n" ;

my $HB2_line = `cat main.c | grep -n "GPIO_SetBits(GPIOD, GPIO_Pin_15" | cut -d: -f1`;
$HB2_line --;
chomp($HB2_line);
print $HB2_line."\n" ;

open (MYFILE, ">$file"); 

print MYFILE 
"
#set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
".$LOAD."load ".$binary."
#hb main
#commands 1
#	i r
#end
#c
#hb main.c:".$HB1_line."
#commands 2
#	i r
#	x/800xw 0x20000200
#end

#run
#c
set logging off
quit

";

my @args = ( "arm-none-eabi-gdb $binary --quiet --batch --command=$file " );	
system @args; 	
