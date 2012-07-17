#!/usr/bin/perl  -w

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
    
if( !open(COUNTERFILE, "<$statdir/$id.counter") )
{
    print "???";
}
else
{
    print <COUNTERFILE>;
    close(COUNTERFILE);
}
