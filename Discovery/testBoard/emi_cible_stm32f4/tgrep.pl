#!/usr/bin/perl
#
use strict;
use warnings;
use Cwd;
use Data::Dump qw(dump);

my $dossier_courant= getcwd();#donne le chemin courant

my $i;
my $tmp;
my $LOGFILE = "output.ref";


my @information;
@information = stat($LOGFILE);
if ("@savedstats[1..13]" ne "@currentstats");
dump(@information);

my @test=();
for ($i=0; $i<13; $i++){
		$tmp = `cat $LOGFILE | grep -a "^r$i " | awk '{print \$2}'`;
		chomp($tmp);
		$tmp = hex($tmp);
		push @test, $tmp;
		
	}
	
my $sp			= `cat $LOGFILE | grep -a "sp"			| awk '{print \$2}'`;
my $lr			= `cat $LOGFILE | grep -a "lr"			| awk '{print \$2}'`;
my $pc			= `cat $LOGFILE | grep -a "pc"			| awk '{print \$2}'`;
my $xpsr		= `cat $LOGFILE | grep -a "xpsr"		| awk '{print \$2}'`;
my $msp			= `cat $LOGFILE | grep -a "MSP"			| awk '{print \$2}'`;
my $psp			= `cat $LOGFILE | grep -a "PSP"			| awk '{print \$2}'`;
my $primask		= `cat $LOGFILE | grep -a "PRIMASK"		| awk '{print \$2}'`;
my $basepri		= `cat $LOGFILE | grep -a "BASEPRI"		| awk '{print \$2}'`;
my $faultmask	= `cat $LOGFILE | grep -a "FAULTMASK"	| awk '{print \$2}'`;
my $control		= `cat $LOGFILE | grep -a "CONTROL"		| awk '{print \$2}'`;

chomp($sp		);	
chomp($lr		);	
chomp($pc		);	
chomp($xpsr		);
chomp($msp		);
chomp($psp		);
chomp($primask	);	
chomp($basepri	);	
chomp($faultmask);	
chomp($control	);

push @test, hex($sp		);	
push @test, hex($lr		);	
push @test, hex($pc		);	
push @test, hex($xpsr	);
push @test, hex($msp	);
push @test, hex($psp	);
push @test, hex($primask);	
push @test, hex($basepri);	
push @test, hex($faultmask);	
push @test, hex($control);

#dump(@test);

for ($i=0; $i< $#test; $i++){
		printf ("R$i : 0x%x\n",$test[$i] ) ;
	}

