#!/usr/bin/perl -w
use strict;
use File::Temp qw( :POSIX );
use lib "./lib";
use Time::HiRes qw(time);

use Curses::UI;
use Curses;
use Cwd;
use Time::HiRes qw(usleep nanosleep);
#@Author Zakaria najm.
#@Brief : This is an interface for analysis of fault injection on ARM processor. This interface is independant from
#the device driver, and also from the fault injection platfrom. Any JTAG GDB sever can be used in combination with arm-gdb client
#to dum internal state from the DUT, then the dumped file is analysed by the tool by parsing internal state of the DIT .
#The parsing is perfomed using unix sh format command and is wrote on a temporary log file. Those log files are in compressed fomat and contains only 
#instantaneous data. This list is actualised at each dump. Thus only a limited amound of data is keept in ram . statistics ar computed cumulatively.
#A second lof file that contains the full fault history is also savend on the hardribve for further analysis . The post tretment of this file
#can be none using the reference_analyse log file that contains tow dumps : the dump befor he trigger, just after the trigger , and after the injection. 
#this alow to identify modification on the cash process flow as wll as pipeline corruptions .
#
#TODO: It could be great to add few more features to this interface : 1- A fault model tree descibing fault models that occure with the internal value of faulted registers,
#occurence rate of each fault models, as well a way to sort fault models depending on few fetures such as occurence rate, register number, number of faulted registers in the fault
#model.
#
#Moreover it could be intersting to output data trough a socket so that further analysis is performed by another tool. The question is : how to synchronize this script with 
#this tool ? Maybe this interface can be the primary interface , but in could be interesting to disable this interface to make the campaign qicker. So for exemple:
# add a cammand line option that unable/ disable Curses interface , and generate an object.sock() output with the data stream + a header that identify faulted registers or the
# actual payload being sent to the outside tool.
#
# If the outside tool is able to synchronize its plot using a single header, it could be great beacause it could increase by several magnitude the overall perfomane of the campaing since few data nedd 
#
# to be send.

#   make KEY_BTAB (shift-tab) working in XTerm
#   and also at the same time enable colors
#$ENV{TERM} = "xterm-vt220" if ($ENV{TERM} eq 'xterm');

my $debug = 0;
if (@ARGV and $ARGV[0] eq '-d') {
    my $fh = tmpfile();
    open STDERR, ">&fh";
    $debug = 1;
} else {
    # We do not want STDERR to clutter our screen.
    my $fh = tmpfile();
    open STDERR, ">&fh";
}

# Use the libraries from the distribution, instead of 
# system wide libraries.

use FindBin;
use lib "$FindBin::RealBin/../lib";
# We don't want STDERR output to clutter the screen.
#
# Hint: If you need STDERR, write it out to a file and put 
# a tail on that file to see the STDERR output. Example:
#open STDERR, ">>/tmp/editor_errors.$$";
open STDERR, ">/dev/null";


# Load an initial file if an argument given on the command line.
# If the file can't be found, assume that this is a new file.
my $text = "";
my $currentfile = shift;
if (defined $currentfile and -f $currentfile) 
{
	open F, "<$currentfile" or die "Can't read $currentfile: $!\n";
	while (<F>) { $text .= $_ }
	$currentfile = $currentfile;
	close F;
}

init_pair(10, COLOR_RED, -1 );
init_pair(11, COLOR_GREEN, -1 );
init_pair(12, COLOR_BLUE, -1 );


# Create the root object.
my $cui = new Curses::UI ( 
    -clear_on_exit => 1, 
	-color_support => 1,
    -debug => $debug
);

# Demo injection index
my $current_demo = 1;

# Demo injection windows
my %w = ();


# ----------------------------------------------------------------------
# Create a menu
# ----------------------------------------------------------------------

sub select_demo($;)
{
    my $nr = shift;
    $current_demo = $nr;
    $w{$current_demo}->focus;
}


my $file_menu = [
    { -label => 'Quit program',       -value => sub {exit(0)}        },
],

my $widget_demo_menu = [
    { -label => 'Progressbar',        -value => sub{select_demo(1)}  },
   ];

my $dialog_demo_menu = [];

my $demo_menu = [
    { -label => 'Configuration [TODO]',       -submenu => $widget_demo_menu  },
    { -label => 'Test ',       -submenu => $dialog_demo_menu  },
    { -label => '------------',       -value   => sub{}              },
    { -label => 'Next step [TODO]',          -value   => \&goto_next_demo   },
    { -label => 'Options [TODO]',      -value   => \&goto_prev_demo   },
];

my $menu = [
    { -label => 'File',               -submenu => $file_menu         },
    { -label => 'Select option',        -submenu => $demo_menu         },
];

$cui->add('menu', 'Menubar', -menu => $menu,	-fg   => "-1",
        -bg   => "-1",
);

# ----------------------------------------------------------------------
# Create the explanation window TODO: update
# ----------------------------------------------------------------------

my $w0 = $cui->add(
    'w0', 'Window', 
    -border        => 1, 
    -y             => -1, 
    -height        => 3,
);
$w0->add('explain', 'Label', 
  -text => "CTRL+P: previous step (TODO)  CTRL+N: next step (TODO)  "
         . "CTRL+X: menu  CTRL+Q: quit"
);


my $LOGFILE = ".gdb.txt";

my @information;
my @saveinformation = stat($LOGFILE);
my $flag = 0;
my @fault_count=(0)x16;
my @fault_count_percent=(0)x16;



my $injections =0 ;
my $i1;
my $tmp1;
my $LOGFILE1 = "output1.ref";
my $MAXX=100;

my @reff=();
for ($i1=0; $i1<13; $i1++){
		$tmp1 = `cat $LOGFILE1 | grep -a "^r$i1 " | awk '{print \$2}'`;
		chomp($tmp1);
		$tmp1 = hex($tmp1);
		push @reff, $tmp1;
		
	}
	
my $sp1			= `cat $LOGFILE1 | grep -a "sp"			| awk '{print \$2}'`;
my $lr1			= `cat $LOGFILE1 | grep -a "lr"			| awk '{print \$2}'`;
my $pc1			= `cat $LOGFILE1 | grep -a "pc"			| awk '{print \$2}'`;
my $xpsr1		= `cat $LOGFILE1 | grep -a "xpsr"		| awk '{print \$2}'`;
my $msp1		= `cat $LOGFILE1 | grep -a "MSP"		| awk '{print \$2}'`;
my $psp1		= `cat $LOGFILE1 | grep -a "PSP"		| awk '{print \$2}'`;
my $primask1	= `cat $LOGFILE1 | grep -a "PRIMASK"	| awk '{print \$2}'`;
my $basepri1	= `cat $LOGFILE1 | grep -a "BASEPRI"	| awk '{print \$2}'`;
my $faultmask1	= `cat $LOGFILE1 | grep -a "FAULTMASK"	| awk '{print \$2}'`;
my $control1	= `cat $LOGFILE1 | grep -a "CONTROL"	| awk '{print \$2}'`;

chomp($sp1		);	
chomp($lr1		);	
chomp($pc1		);	
chomp($xpsr1	);
chomp($msp1		);
chomp($psp1		);
chomp($primask1	);	
chomp($basepri1	);	
chomp($faultmask1);	
chomp($control1	);

push @reff, hex($sp1		);	
push @reff, hex($lr1		);	
push @reff, hex($pc1		);	
push @reff, hex($xpsr1		);
push @reff, hex($msp1		);
push @reff, hex($psp1		);
push @reff, hex($primask1	);	
push @reff, hex($basepri1	);	
push @reff, hex($faultmask1	);	
push @reff, hex($control1	);


# ----------------------------------------------------------------------
# Create the demo injection windows
# ----------------------------------------------------------------------
my $width ;
my %screens = (
    '1'  => 'Progressbar',
);

my @screens = sort {$a<=>$b} keys %screens;

my %args = (
    -border       => 0, 
    -titlereverse => 0, 
    -padtop       => 2, 
    -padbottom    => 3, 
    -ipad         => 1,
);

while (my ($nr, $title) = each %screens)
{
	$width = $cui->width_by_windowscrwidth(1, -border => 1); 
    my $id = "window_$nr";
    $w{$nr} = $cui->add(
        $id, 'Window', 
        -title => "Fault Injection Platform::Real-Time Check ", #$title ($nr/" . @screens . ")",
        %args
    );
}



# ----------------------------------------------------------------------
# Progressbar
# ----------------------------------------------------------------------
#

$w{1}->{-canvasscr}->attron(COLOR_PAIR(12));
$w{1}->add( undef, "Label", -y => 0, -text => "R0", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan' );
$w{1}->{-canvasscr}->attroff(COLOR_PAIR(12));



$w{1}->add( 'p1', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2 + 1) , -y => 0,  -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );

$w{1}->add( undef, "Label", -y => 1, -text => "R1", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p2', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+1) , -y => 1, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );

$w{1}->add( undef, "Label", -y => 2, -text => "R2", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p3', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2 +1) , -y => 2, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );


$w{1}->add( undef, "Label", -y => 3, -text => "R3", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p4', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+ 1) , -y => 3, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );





$w{1}->add( undef, "Label", -y => 4, -text => "R4", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p5', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+ 1) , -y => 4, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );

$w{1}->add( undef, "Label", -y => 5, -text => "R5", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p6', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+ 1) , -y => 5, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );


$w{1}->add( undef, "Label", -y => 6, -text => "R6", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p7', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+ 1) , -y => 6, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4) );

$w{1}->add( undef, "Label", -y => 7, -text => "R7", -x => ($w{1}->canvaswidth /2 -3), -fg => 'cyan'  );
$w{1}->add( 'p8', 'Progressbar', -max => $MAXX, 
            -x => ($w{1}->canvaswidth /2+ 1) , -y => 7, -nopercentage => 0, -width => ($w{1}->canvaswidth /2-4));





$w{1}->add( undef, "Label", -y => 0, -text => "R8", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p9', 'Progressbar', -max => $MAXX, 
            -width => (($w{1}->canvaswidth) /2 -8) , -y => 0,  -nopercentage => 0,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );

$w{1}->add( undef, "Label", -y => 1, -text => "R9", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p10', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 1, -nopercentage => 0 ,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 )) );


$w{1}->add( undef, "Label", -y => 2, -text => "R10", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p11', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 2, -nopercentage => 0 ,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );

$w{1}->add( undef, "Label", -y => 3, -text => "R11", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p12', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 3, -nopercentage => 0 ,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );





$w{1}->add( undef, "Label", -y => 4, -text => "R12", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p13', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 4, -nopercentage => 0 ,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );

$w{1}->add( undef, "Label", -y => 5, -text => "R13", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p14', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 5, -nopercentage => 0 ,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );

$w{1}->add( undef, "Label", -y => 6, -text => "R14", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p15', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 6, -nopercentage => 0,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );

$w{1}->add( undef, "Label", -y => 7, -text => "R15", -x => 0, -fg => 'cyan' );
$w{1}->add( 'p16', 'Progressbar', -max => $MAXX, 
            -width => ($w{1}->canvaswidth /2-8) , -y => 7, -nopercentage => 0,-x => ($w{1}->canvaswidth -($w{1}->canvaswidth -4 ))  );


		
		
$w{1}->add( undef, "Label",-fg => 'cyan' , -x=> (($w{1}->canvaswidth) -25)  , -y => 8, -text => "Injections :");
$w{1}->add( 
    'progressbarlabel', 'Label',
    -x =>  (($w{1}->canvaswidth)  -13) , -y => 8, -width => 10, -border => -0,
    -text => "NB Injections", -bold => 1
);
$w{1}->add( undef, "Label",-fg => 'cyan' , -x=> (($w{1}->canvaswidth) -25)  , -y => 9, -text => "Time :");
$w{1}->add( 
    'dureelabel', 'Label',
    -x =>  (($w{1}->canvaswidth)  -13) , -y => 9, -width => 10, -border => -0,
    -text => "dt injection", -bold => 1
);



#$w{1}->refresh();

my $values = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16 ];
my $labels = {
    1	=> 'r0',		2	=> 'r1',
    3	=> 'r2',		4	=> 'r3',
    5	=> 'r4',		6	=> 'r5',
    7	=> 'r6',		8	=> 'r7',
    9	=> 'r8',		10	=> 'r9',
    11  => 'r10',		12  => 'r11',
    13  => 'r12',		14	=> 'msp',
    15  => 'sp',		16  => 'xpsr',
};


sub listbox_callback()
{
    my $listbox = shift;
    my $label = $listbox->parent->getobj('listboxlabel');
    my @sel = $listbox->get;
    @sel = ('<none>') unless @sel;
    my $sel = "selected: " . join (", ", @sel);
    $label->text($listbox->title . " $sel \ndsd");
}

$w{1}->add(
    undef, 'Listbox',
    -y          => 20,
	-x			=> (2 - $width ),
    
	-padbottom  => 2,
    -values     => $values,
    -labels     => $labels,
	-border		=> -1,
	-width      => 20,
    -border     => 1,
    -multi      => 1,
    -title      => 'Target registers',
    -vscrollbar => 1,
    -onchange   => \&listbox_callback,
);


$w{1}->add(
    'listboxlabel', 'Label',
    -y => 18,
    -bold => 1,
	-x	=> (2 - $width ),
    -text => "Select variables to display....",
    -width => -1,
);


# ----------------------------------------------------------------------
# Some buttons (reset fault injection demo etc)
# ----------------------------------------------------------------------

$w{1}->add(
    undef, 'Buttonbox',
    -y => $w{1}->canvasheight -1, 
	-x => ($w{1}->canvaswidth -  40 )  ,
    -buttons => [
      { 
	   -label => " Resetss ",
	   -onpress => sub {
						my $button = shift;
						$button->root->dialog("Reset stats");
						@saveinformation = stat($LOGFILE);
						$flag = 0;
						@fault_count=(0)x16;
						@fault_count_percent=(0)x16;
						$injections =0 ;
					} 
    },{
        -label => "< Button 2 >", 
        -value => "the second button", 
        -onpress => \&button_callback, 
        },{
            -label => "< Button 3 >", 
        -value => "the third button", 
        -onpress => \&button_callback, 
        },   
    ],
);

#---------------------------------------------------------------------
# Model Bar TODO: implement an application bar that alow to sort models
# depending on occurence rate | number of faulted register 
# Then, implement a tree that show values exemple :  r0_I+r15_C -+
#																 |- r0  = val
#																 |- r15 = val
#---------------------------------------------------------------------



$w{1}->add(
    undef, 'Buttonbox',
    -y => 17, 
	-x => ($w{1}->canvaswidth -  40 )  ,
	-bg => 'cyan' ,
	-fg => 'white',
    -buttons => [
      { 
	   -label => " Resetss ",
	   -onpress => sub {
						my $button = shift;
						$button->root->dialog("Reset stats");
						@saveinformation = stat($LOGFILE);
						$flag = 0;
						@fault_count=(0)x16;
						@fault_count_percent=(0)x16;
						$injections =0 ;
					} 
    },{
            -label => "< Button 2 >", 
        -value => "the second button", 
        -onpress => \&button_callback, 
        },{
            -label => "< Button 3 >", 
        -value => "the third button", 
        -onpress => \&button_callback, 
        },   
    ],
);

$w{1}->add( 
    'labellll', 'Label',
    -x => 0 , -y => 16, -width => $w{1}->canvaswidth , -border => -0,
    -text => " "x $w{1}->canvaswidth , -bold => 1 , -bg => 'cyan' 
);




my $file_menu2 = [
    { -label => 'Quit program',       -value => sub {exit(0)}        },
],

my $widget_demo_menu2 = [
    { -label => 'Progressbar',        -value => sub{select_demo(1)}  },
   ];

my $dialog_demo_menu2 = [];

my $demo_menu2 = [
    { -label => 'Widget demos',       -submenu => $widget_demo_menu  },
    { -label => 'Dialog demos',       -submenu => $dialog_demo_menu  },
    { -label => '------------',       -value   => sub{}              },
    { -label => 'Next step',          -value   => \&goto_next_demo   },
    { -label => 'Options',			  -value   => \&goto_prev_demo   },
];

my $menu2 = [
    { -label => 'File',               -submenu => $file_menu         },
    { -label => 'Select demo',        -submenu => $demo_menu         },
];



$w{1}->add(
	'menu2','Appbar', 
	-fg   => "white",
        -bg   => "blue",
		-menu => $menu2,
	-y => 14,
	-width=> $w{1}->canvaswidth +4 ,
);

# ----------------------------------------------------------------------
# Labels
# ----------------------------------------------------------------------

$w{1}->add('RR0',,'Label',-text=>"R0" ,-y=>8 ,-bold=>1 );
$w{1}->add('RR1',,'Label',-text=>"R1" ,-y=>9 ,-bold=>1 );
$w{1}->add('RR2',,'Label',-text=>"R2" ,-y=>10,-bold=>1 );
$w{1}->add('RR3',,'Label',-text=>"R3" ,-y=>11,-bold=>1 );
$w{1}->add('RR4',,'Label',-text=>"R4" ,-y=>8 ,-bold=>1,-x=> 25 );
$w{1}->add('RR5',,'Label',-text=>"R5" ,-y=>9 ,-bold=>1,-x=> 25 );
$w{1}->add('RR6',,'Label',-text=>"R6" ,-y=>10,-bold=>1,-x=> 25 );
$w{1}->add('RR7',,'Label',-text=>"R7" ,-y=>11,-bold=>1,-x=> 25 );
$w{1}->add('RR8',,'Label',-text=>"R8" ,-y=>8 ,-bold=>1,-x=> 50 );
$w{1}->add('RR9',,'Label',-text=>"R9" ,-y=>9 ,-bold=>1,-x=> 50 );
$w{1}->add('RR10','Label',-text=>"R10",-y=>10,-bold=>1,-x=> 50 );
$w{1}->add('RR11','Label',-text=>"R11",-y=>11,-bold=>1,-x=> 50 );
$w{1}->add('RR12','Label',-text=>"R12",-y=>8,-bold=>1,-x=> 75  );
$w{1}->add('RR13','Label',-text=>"R13",-y=>9,-bold=>1,-x=> 75  );
$w{1}->add('RR14','Label',-text=>"R14",-y=>10,-bold=>1,-x=> 75  );
$w{1}->add('RR15','Label',-text=>"R15",-y=>11,-bold=>1,-x=> 75  );


my @labels;

my $label= $w{1}->add('R0' ,'Label',-text=>"0x00000000000000",-y=>8,-bold=>1,-x => 4, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R1' ,'Label',-text=>"0x00000000000000",-y=>9,-bold=>1,	-x => 4, -fg => 'cyan'  );
push @labels, $label;
 $label= $w{1}->add('R2' ,'Label',-text=>"0x00000000000000",-y=>10,-bold=>1,-x => 4, -fg => 'cyan'  );
push @labels, $label;
 $label= $w{1}->add('R3' ,'Label',-text=>"0x00000000000000",-y=>11,-bold=>1,-x => 4, -fg => 'cyan'  );
push @labels, $label;
 $label= $w{1}->add('R4' ,'Label',-text=>"0x00000000000000",-y=>8,-bold=>1,	-x => 29, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R5' ,'Label',-text=>"0x00000000000000",-y=>9,-bold=>1,	-x => 29, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R6' ,'Label',-text=>"0x00000000000000",-y=>10,-bold=>1,-x => 29, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R7' ,'Label',-text=>"0x00000000000000",-y=>11,-bold=>1,-x => 29, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R8' ,'Label',-text=>"0x00000000000000",-y=>8,-bold=>1,	-x => 54, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R9' ,'Label',-text=>"0x00000000000000",-y=>9,-bold=>1,	-x => 54, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R10','Label',-text=>"0x00000000000000",-y=>10,-bold=>1,-x => 54, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R11','Label',-text=>"0x00000000000000",-y=>11,-bold=>1,-x => 54, -fg => 'cyan' );	
push @labels, $label;
 $label= $w{1}->add('R12','Label',-text=>"0x00000000000000",-y=>8,-bold=>1,	-x => 79, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R13','Label',-text=>"0x00000000000000",-y=>9,-bold=>1,	-x => 79, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R14','Label',-text=>"0x00000000000000",-y=>10,-bold=>1,-x => 79, -fg => 'cyan' );
push @labels, $label;
 $label= $w{1}->add('R15','Label',-text=>"0x00000000000000",-y=>11,-bold=>1,-x => 79, -fg => 'cyan' );
push @labels, $label;


sub progressbar_timer_callback($;)
{

my $i;
my $tmp;

if (-e $LOGFILE) {
	@information = stat($LOGFILE);

	if ("@saveinformation[1..13]" ne "@information"){
		@saveinformation[1..13] = @information ;
		$injections +=1;
		usleep(10000);
		$flag = 1;
		
	};

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

    my $cui = shift;
    my @l = localtime;
    $w{1}->getobj('p1')->pos($fault_count_percent[0]);
	      
	
	$w{1}->getobj('p1')->pos($fault_count_percent[0]);
	$w{1}->getobj('p2')->pos($fault_count_percent[1]);
	$w{1}->getobj('p3')->pos($fault_count_percent[2]);
	$w{1}->getobj('p4')->pos($fault_count_percent[3]);
	$w{1}->getobj('p5')->pos($fault_count_percent[4]);
	$w{1}->getobj('p6')->pos($fault_count_percent[5]);
	$w{1}->getobj('p7')->pos($fault_count_percent[6]);
	$w{1}->getobj('p8')->pos($fault_count_percent[7]);


	$w{1}->getobj('p9') ->pos($fault_count_percent[8]);
	$w{1}->getobj('p10')->pos($fault_count_percent[9]);
	$w{1}->getobj('p11')->pos($fault_count_percent[10]);
	$w{1}->getobj('p12')->pos($fault_count_percent[11]);
	$w{1}->getobj('p13')->pos($fault_count_percent[12]);
	$w{1}->getobj('p14')->pos($fault_count_percent[13]);
	$w{1}->getobj('p15')->pos($fault_count_percent[14]);
	$w{1}->getobj('p16')->pos($fault_count_percent[15]);

    $w{1}->getobj('progressbarlabel')->text(
         sprintf("%02d", $injections)
    );

	for ($i=0; $i< 16; $i++){
		if($test[$i] != $reff[$i]){
			$labels[$i]->set_color_fg('red');
			if($flag==1){
				$fault_count[$i]++;
				$fault_count_percent[$i] = 	($fault_count[$i]/ $injections) * 100;

			}
		}else{
			$labels[$i]->set_color_fg(-1);
				if($flag==1){
					$fault_count_percent[$i] = 	($fault_count[$i]/ $injections) * 100;
			}

		}

		$w{1}->getobj('R'.$i)->text(sprintf ("0x%x\n",$test[$i] ) );
			
	}

	$flag = 0;

}

	`rm -rf $LOGFILE`;

}     

$cui->set_timer(
    'progressbar_demo',
    \&progressbar_timer_callback, 1
);
$cui->disable_timer('progressbar_demo');

$w{1}->onFocus( sub{$cui->enable_timer  ('progressbar_demo')} );
$w{1}->onBlur(  sub{$cui->disable_timer ('progressbar_demo')} );



# ----------------------------------------------------------------------
# Setup bindings and focus 
# ----------------------------------------------------------------------

# Bind <CTRL+Q> to quit.
$cui->set_binding( sub{ exit }, "\cQ" );

# Bind <CTRL+X> to menubar.
$cui->set_binding( sub{ shift()->root->focus('menu') }, "\cX" );

sub goto_next_demo()
{
    $current_demo++;
    $current_demo = @screens if $current_demo > @screens;
    $w{$current_demo}->focus;
}
$cui->set_binding( \&goto_next_demo, "\cN" );

sub goto_prev_demo()
{
    $current_demo--;
    $current_demo = 1 if $current_demo < 1;
    $w{$current_demo}->focus;
}

$cui->set_binding( \&goto_prev_demo, "\cP" );

$w{$current_demo}->focus;


# ----------------------------------------------------------------------
# Get things rolling...
# ----------------------------------------------------------------------

$cui->mainloop;

