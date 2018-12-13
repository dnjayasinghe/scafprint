#!/usr/bin/perl

use strict;
use warnings;
use Cwd;

my $numArgs = $#ARGV + 1;
#print "thanks, you gave me $numArgs command-line arguments.\n";

my $LOAD= "#";

chomp($LOAD);

if(lc($ARGV[0]) eq "pgm"){
	#print "chargement de l'elf sur la carte ... ";
	$LOAD= "#";
}
else{
	$LOAD= "#";
}

my $file = $ARGV[1];
my $binary = $ARGV[2];
my $kk = $ARGV[4];
my $rr = $ARGV[3];
chomp($file);
chomp($binary);
my @K = unpack '(a2)*', $kk;
my @R = unpack '(a2)*', $rr;
#print "K".$K[0]."\n" ;
#print "R".$R[0]."\n" ;

for (my $i = 0; $i < 12; $i++) {
    $K[$i] = $K[$i*4+3].$K[$i*4+2].$K[$i*4+1].$K[$i*4+0];
}
for (my $i = 0; $i < 12; $i++) {
    $R[$i] = $R[$i*4+3].$R[$i*4+2].$R[$i*4+1].$R[$i*4+0];
}
#print "K = ",@K,"\n";
my $ADDR_X = $ARGV[5];
chomp($ADDR_X);
my $ADDR_Y = $ARGV[6];
chomp($ADDR_Y);
my $ADDR_Z = $ARGV[7];
chomp($ADDR_Z);

my $ADDR_FLAG = $ARGV[8];
chomp($ADDR_FLAG);

#print $ADDR_X,"\n";

my $HB1_line	= `sed -n '/GPIO_Write(GPIOD, 0xFF)/=' main.c` ;
#$HB1_line =368; # On place le breakpoint post-macro apres le signal de triger
#chomp($HB1_line);
#print $HB1_line."\n" ;


my $HB2_line = `cat main.c | grep -n "GPIO_SetBits(GPIOD, GPIO_Pin_15" | cut -d: -f1`;
#$HB2_line =401;


my $HB3_line =355;
$HB2_line =404;
#$HB1_line =404;

#chomp($HB2_line);
#print $HB2_line."\n" ;
#$K[0]="00000000";
open (MYFILE, ">$file"); 


print MYFILE 
"set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
".$LOAD."load ".$binary."
hb main
commands 1
#        c
end
c
hb main.c:".$HB3_line."
commands 2
#        print K
#	x/12xw 0x20001a18
#	print 1
	set {int}0x20001ac4=0x".$K[11]."
	set {int}0x20001ac0=0x".$K[10]."
	set {int}0x20001abc=0x".$K[9]."
	set {int}0x20001ab8=0x".$K[8]."
	set {int}0x20001ab4=0x".$K[7]."
	set {int}0x20001ab0=0x".$K[6]."
	set {int}0x20001aac=0x".$K[5]."
	set {int}0x20001aa8=0x".$K[4]."
	set {int}0x20001aa4=0x".$K[3]."
	set {int}0x20001aa0=0x".$K[2]."
	set {int}0x20001a9c=0x".$K[1]."
	set {int}0x20001a98=0x".$K[0]."
        print K
#	x/12xw 0x20001a18
#	set {int}0x20001A0C=0x".$R[11]."
#	set {int}0x20001A08=0x".$R[10]."
#	set {int}0x20001A04=0x".$R[9]."
#	set {int}0x20001A00=0x".$R[8]."
#	set {int}0x200019fc=0x".$R[7]."
#	set {int}0x200019f8=0x".$R[6]."
#	set {int}0x200019f4=0x".$R[5]."
#	set {int}0x200019f0=0x".$R[4]."
#	set {int}0x200019eC=0x".$R[3]."
#	set {int}0x200019e8=0x".$R[2]."
#	set {int}0x200019e4=0x".$R[1]."
#	set {int}0x200019e0=0x".$R[0]."
#        c
end
c
hb main.c:".$HB2_line."
commands 3
#	i r
#	print 1
#	x/3xw 0x2001FF18
#	x/3xw 0x2001FF24
#	x/3xw 0x2001FF30
#        print ".'"BUFFER :"'."
#        x/30xw BUFFER
	print ".'"x"'."
	x/6xw 0x20000a14
	print ".'"y"'."
	x/6xw 0x20000a3c
	print ".'"z"'."
	x/1xw 0x20000a64
	print ".'"flag"'."
	x/1xw ".$ADDR_FLAG."
#	print ".'"x"'."
#	x/6xw ".$ADDR_X."
#	print ".'"y"'."
#	x/6xw ".$ADDR_Y."
#	print ".'"z"'."
#	x/1xw ".$ADDR_Z."
#	print ".'"flag"'."
#	x/1xw ".$ADDR_FLAG."
	set logging off
	quit
end
hb stm32f4xx_it.c:63
commands 4
	#i r
	#x/800xw 0x20000200
	set logging off
	quit
end
c
set logging off
quit
";


