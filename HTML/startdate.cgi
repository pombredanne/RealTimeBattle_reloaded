#!/usr/bin/perl -w

$statdir="rw/stats/";

if( @ARGV ) 
{
    $id=shift(@ARGV);
}
else
{
    $id="main";
}

select STDOUT;

print "Content-type: text/html\n\n";
    
if( !open(DATEFILE, "<$statdir/$id.startdate") )
{
    print "???";
}
else
{
    $tid = <DATEFILE>;
    close(DATEFILE);


    ($s,$s,$s,$mday,$mon,$year,$s,$s,$s) = gmtime($tid);
    
    select STDOUT;
    
    $year += 1900;
    
    
  SWITCH: {
      if ($mon == 0) { $month = "January";  last SWITCH; }
      if ($mon == 1) { $month = "February"; last SWITCH; }
      if ($mon == 2) { $month = "March";    last SWITCH; }
      if ($mon == 3) { $month = "April";    last SWITCH; }
      if ($mon == 4) { $month = "May";      last SWITCH; }
      if ($mon == 5) { $month = "June";     last SWITCH; }
      if ($mon == 6) { $month = "July";     last SWITCH; }
      if ($mon == 7) { $month = "August";   last SWITCH; }
      if ($mon == 8) { $month = "September";last SWITCH; }
      if ($mon == 9) { $month = "Oktober";  last SWITCH; }
      if ($mon ==10) { $month = "November"; last SWITCH; }
      if ($mon ==11) { $month = "December"; last SWITCH; }
  }

    print "$mday $month $year";
}
