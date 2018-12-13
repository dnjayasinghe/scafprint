#!/usr/bin/env perl

open(my $results, "<:encoding(UTF-8)", $ARGV[0])
    or die "can't open UTF-8 encoded filename: $!";

my $r = $ARGV[1];
my $k;
my @x;
my @y;
my $z;
my $flag;

my $mode = "k";

while (<$results>) {
  # print STDERR $mode;
  if ($mode == "k") {
    if (m/0x20001a98/) {
      $k = (split /"/)[1];
      $mode = "x";
      next;
    }
  }
  if ($mode == "x") {
    if (m/0x20001a98/) { # failed injection, starting over
      $k = (split /"/)[1];
      $mode = "x";
      print "AAAA";
      exit;
    }
    if (m/0x20000a14/) {
      chomp;
      ($x[5], $x[4], $x[3], $x[2]) = (split /\t0x/)[1..4];
      next;
    }
    if (m/0x20000a24/) {
      chomp;
      ($x[1], $x[0]) = (split /\t0x/)[1..2];
      $mode = "y";
      next;
    }
  }
  if ($mode == "y") {
    if (m/0x20000a3c/) {
      chomp;
      ($y[5], $y[4], $y[3], $y[2]) = (split /\t0x/)[1..4];
      next;
    }
    if (m/0x20000a4c/) {
      chomp;
      ($y[1], $y[0]) = (split /\t0x/)[1..2];
      $mode = "z";
      next;
    }
  }
  if ($mode == "z") {
    if (m/0x20000a64/) {
      chomp;
      $z = (split /\t0x/)[1];
      $mode = "flag";
      next;
    }
  }
  if ($mode == "flag") {
    if (m/0x2001fef0/) {
      chomp;
      $flag = substr((split /\t0x/)[1], -1);

      print $r," ",$k," ",@x," ",@y," ",$z," ", $flag,"\n";
      
      $mode = "k";
      next;
    }
  }
}

close ($results);

# 0x20000a14:	0xc65d1267	0x35265068	0xa7615d57	0x8da055a1
# 0x20000a24:	0x72e6d372	0x82bc199d
