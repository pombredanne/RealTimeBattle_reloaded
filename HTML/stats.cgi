#!/usr/bin/perl  -w

$statdir="rw/stats/";
$update_hours = 5;

if( @ARGV ) 
{
    $id=shift(@ARGV);
}
else
{
    $id="main";
}

if (defined $ENV{'REMOTE_HOST'})
{ 
    $host=$ENV{'REMOTE_HOST'}; 
}
elsif(defined $ENV{'REMOTE_ADDR'})
{ 
    $host=$ENV{'REMOTE_ADDR'}; 
}
else
{ 
    $host="??"; 
}



$http_agent=(defined $ENV{'HTTP_USER_AGENT'} ? $ENV{'HTTP_USER_AGENT'} : "??");
$url=(defined $ENV{'DOCUMENT_URI'} ? $ENV{'DOCUMENT_URI'} : "??");
$refer_url=(defined $ENV{'HTTP_REFERER'} ? $ENV{'HTTP_REFERER'} : "??");

$now=time;

# check if host has visited this page already.

$new_entry = "?";

if( !open(STATFILE, "<$statdir/$id.hosts") )
{
    $new_entry = "yes";
}
else
{
#    select STDOUT;
    
  LINE: while (<STATFILE>)
    {
        if( /^([^\t]+)\t([^\t]+)\t([^\t]+)\t([^\t]+)\t([^\t]+)\n$/ )
        {
#            print "1:$1  2:$2  3:$3  4:$4  5:$5\n";
#            print $now-$2;print "\n"; print 3600*$update_hours; print "\n";
            
            if( $1 eq $host && $5 eq $http_agent && $now - $2 <= 3600*$update_hours )
            { $new_entry = "no"; }
        }
#        print "$new_entry\n";
        last LINE if($new_entry ne "?" );
    }

    close(STATFILE);
}

if( $new_entry ne "no" )
{
    open(STATFILE, ">>$statdir/$id.hosts") or die "Can't open stat file: $!\n";

    select STATFILE;

    print "$host\t$now\t$url\t$refer_url\t$http_agent\n";

    close(STATFILE);


    if( !open(COUNTERFILE, "<$statdir/$id.counter") )
    {
        open(DATEFILE, ">$statdir/$id.startdate") or die "Can't open startdate file: $!\n";
        select DATEFILE;
        
        print "$now";

        close(DATEFILE);
 
       $current_counter = 0;
    }
    else
    {
        $current_counter = <COUNTERFILE>;
        close(COUNTERFILE);
    }

    open(COUNTERFILE, ">$statdir/$id.counter") or die "Can't open counter file: $!\n";
    select COUNTERFILE;
    print $current_counter+1;
    close(COUNTERFILE);
}
    
