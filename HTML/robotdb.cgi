#!/usr/bin/perl -w

$databasefile = "rw/robotdb/RDB.html";

print "Content-type: text/html\n\n";

open(DBFILE, "<$databasefile") or die "Can't open database file: $!\n";

print <DBFILE>;
