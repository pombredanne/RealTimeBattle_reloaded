#!/usr/bin/perl -w

$databasefile = "rw/robotdb/RDB.html";
$allow_html = 1;

&parse_form;

if($FORM{'robot'})
{
      $robot = "$FORM{'robot'}";
}
else{   &error("No robot name"); }

if($FORM{'version'})
{
    $version = "$FORM{'version'}";
}
else{    &error("No version number"); }

if ($FORM{'email'} =~ /.*\@.*\..*/) 
{
      $email = "$FORM{'email'}";
}

if($FORM{'url'}){ $url = "$FORM{'url'}"; }

if($FORM{'constructor'}){  $constructor = "$FORM{'constructor'}"; }


if($FORM{'requirements'}){  $requirements = "$FORM{'requirements'}"; }
if($FORM{'description'}){  $description = "$FORM{'description'}"; }
if($FORM{'comments'}){  $comments = "$FORM{'comments'}"; }

$buffer = "";
$entry_found = 0;
if(open(DBFILE, "<$databasefile"))
{
    
    while (<DBFILE>)
    {
        if(/^<TR><TD><A href=\"http:\/\/([^"]*)\">([^<]*)<\/A><TD>([^<]*)<TD><A href=\"mailto:([^"]*)\">([^<]*)<\/A><TD>([^<]*)<TD>([^<]*)<TD>(.*)/ )
        {
            if( $robot eq $2 )
            {
                if(!$url){ $url = $1; }
                if(!$email){  $email = $4;  }
                if(!$constructor){ $constructor = $5; }
                if(!$requirements){ $requirements = $6; }
                if(!$description){ $description = $7; }
                if(!$comments){ $comments = $8; }
                
                $buffer = "$buffer<TR><TD><A href=\"http://$url\">$robot</A>";
                $buffer = "$buffer<TD>$version";
                $buffer = "$buffer<TD><A href=\"mailto:$email\">$constructor</A>";
                $buffer = "$buffer<TD>$requirements";
                $buffer = "$buffer<TD>$description";
                $buffer = "$buffer<TD>$comments\n";

                $entry_found = 1;
            }
            else
            {
                $buffer = "$buffer$_";
            }
        }
    }
    close(DBFILE);
}
else
{ 
    &error("Couldn't read db file"); 
}
    

if( $entry_found == 0 )
{
    if( !$url ){ &error("No download url given"); }
    if( !$email ){ &error("Email address needed"); }
    if( !$constructor ){ &error("Please enter constructor"); }
    if( !$description ){ &error("Description missing"); }

    $buffer = "$buffer<TR><TD><A href=\"http://$url\">$robot</A>";
    $buffer = "$buffer<TD>$version";
    $buffer = "$buffer<TD><A href=\"mailto:$email\">$constructor</A>";
    $buffer = "$buffer<TD>$requirements";
    $buffer = "$buffer<TD>$description";
    $buffer = "$buffer<TD>$comments\n";
}

open(DBFILE, ">$databasefile") or &error("Can't open database file: $!\n");

print DBFILE "$buffer";

close(DBFILE);

print STDOUT "Content-type: text/html\n\n";
print STDOUT "<html><head><title>Entry Submitted</title></head>\n";
print STDOUT "<BODY bgcolor=\"#FAF0E6\" text=\"#000000\" lang=\"en\">";


if( $entry_found == 1 )
{
    print STDOUT "<H2>Entry updated!</H2>";
}
else
{
    print STDOUT "<H2>Entry submitted!</H2>";
}

print STDOUT "<center>[ <a href=\"../robotdb.html\">Back to the robot database</a> ] [ <a href=\"../Main.html\">Home</a> ] </center> <hr size=7 width=75%>";
print STDOUT "</body></html>\n";
exit 0;

sub parse_form 
{

   # Get the input
   read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

   # Split the name-value pairs
   @pairs = split(/&/, $buffer);

   foreach $pair (@pairs) {
      ($name, $value) = split(/=/, $pair);

      # Un-Webify plus signs and %-encoding
      $value =~ tr/+/ /;
      $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
      $value =~ s/<!--(.|\n)*-->//g;

      if ($allow_html != 1) {
         $value =~ s/<([^>]|\n)*>//g;
      }
      else {
         unless ($name eq 'body') {
	    $value =~ s/<([^>]|\n)*>//g;
         }
      }

      $FORM{$name} = $value;
   }

}

sub error
{   
    print STDOUT "Content-type: text/html\n\n";
    print STDOUT "<html><head><title>ERROR</title></head>\n";
    print STDOUT "<BODY bgcolor=\"#FAF0E6\" text=\"#000000\" lang=\"en\">";
    print STDOUT "Sorry, couldn't submit entry: $_[0].";
#    print STDOUT "\n$buffer\n";
    print STDOUT"</body></html>\n";
    exit 1;
}
