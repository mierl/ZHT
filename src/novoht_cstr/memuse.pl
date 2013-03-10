#!/usr/bin/perl
use warnings;

$proc = $ARGV[0];
$file = $ARGV[1];
my $maxmem = 0;
$input = *STDIN; 
$SIG{'INT'} = 'interupt';
$SIG{'QUIT'} = 'interupt';
$SIG{'HUP'} = 'interupt';
$SIG{'TRAP'} = 'interupt';
$SIG{'ABRT'} = 'interupt';
$SIG{'STOP'} = 'interupt';
if (defined $file){
   open ($input, "<", $file) or die;
}
while (<$input>){
   if ($_ =~ $proc){
      @cols = split(' ',$_);
      if ($cols[4] =~ m/(\d+\.?\d*)(\w?)/){
         print "$1$2\n";
         $num =0;
         if ($2 eq 'g') { $num = $1 * 1000000; }
         if ($2 eq 'm') { $num = $1 * 1000; }
         if ($num > $maxmem) { $maxmem = $num; }
      }
     }
}
print "Max memory used: " . $maxmem/1000 . "MB \n";

sub interupt {
   my($signal)=@_;
   print "Max memory used: " . $maxmem/1000 . "MB \n";
   print "Caught interrupt\: $signal \n";
   print "Now Exiting \n";
   exit(1);
}
