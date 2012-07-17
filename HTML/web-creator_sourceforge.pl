#!/usr/bin/perl -w
#
# Perl script to create webable files from normal html files using 'Template.html'
#

$homepage = 'http://RealTimeBattle.sourceforge.net/';
$lysatorhomepage = 'http://www.lysator.liu.se/realtimebattle/';
$emailerik = 'ejlo@users.sourceforge.net';
$emailragnar = 'ragnar@users.sourceforge.net';
$emailboth = 'rtb@users.sourceforge.net';
$today = gmtime;

$cgibindir = 'cgi-bin';

$docsname = "Documentation";
$listname = "Lists";
$downloadname = "Download";
$oldversionname = "$downloadname/old_versions";
$robotname = "Robots";

$htmlpostfix = "shtml";

$sourceforgehomepage = 'http://www.sourceforge.net';
$sourceforgedownload = "http://download.sourceforge.net/RealTimeBattle/";
$sourceforgefilelist = "$sourceforgehomepage/project/filelist.php?group_id=561";

$lysatordownload = "$lysatorhomepage\Download/";

$template = "Template.html";
$template_end = "Template_end.html";

$dir = 'Sourceforge';

$counter = '<CENTER> 
<!--#include virtual="/cgi-bin/stats.cgi?main"-->
 visitors since </CENTER>
<CENTER>
<!--#include virtual="/cgi-bin/showstatsdate.cgi?main"-->
 </CENTER>
<HR>
';

mkdir "$dir", 0755;
mkdir "$dir/Documentation", 0755;
mkdir "$dir/Lists", 0755;
#mkdir "$dir/wwwboard", 0755;
#mkdir "$dir/cgi-bin", 0755;

while( @ARGV )
{
    $filename = shift(@ARGV);
    $outputfilename = "$filename";

    if(  $filename =~ m/RealTimeBattle/ )
    {
        $counter_this_page = "";
    }
    else
    {
        $outputfilename =~ s/html/shtml/;
        $counter_this_page = "$counter";
    }


    select STDOUT;
    
    print "Parsing $filename ... ";

    open(INFILE, "<$filename") or die "Can't open input file: $!\n";

    open(OUTFILE, ">$dir/$outputfilename") or die "Can't open output file: $!\n";
    open(TMP, ">tmp") or die "Can't open tmp file: $!\n";

    select TMP;
    
    while (<INFILE>) 
    {
      SWITCH: 
        {
            /<\/HEAD>/ && do 
            { 
                print " \<LINK HREF=\"Main.html\" REL=Start\>\n\<\/HEAD\>\n";
                last SWITCH; 
            };
            /<BODY>/ && do 
            { 
                open(TEMPLATE, $template) or die "Can't find $template: $!\n";
                print <TEMPLATE>;
                last SWITCH; 
            };
            
            /<\/BODY>/ && do 
            { 
                open(TEMPLATE_END, $template_end) or die "Can't find $template_end: $!\n";
                print <TEMPLATE_END>;
                last SWITCH; 
            };
            print ;
        } 
    }

    close(INFILE);
    close(TMP);


# Get the VERSION
    
    open(CONFIGURE_IN, '<../configure.in') or die "Can't open configure.in: $!\n";
    
    $version = "";
    $release = "";
    
    while (<CONFIGURE_IN>)
    {
        if( /RTB_LAST_RELEASED_VERSION=(.+)/ ) { $version = $1; }
        if( /RTB_LAST_RELEASED_RELEASE=(.+)/ ) { $release = $1; }
    }
    
    close(CONFIGURE_IN);
    
# Parse the file again to replace, #VERSION#, #BASEDIR#, etc.
    
    open(TMP, "<tmp") or die "Can't open tmp file: $!\n";

    select OUTFILE;

    if( (index $filename, "$docsname") > -1)
    {
        # In the Documentation dir.
        $basedir = "../";
        $docsdir = "";
        $downloaddir = "../$downloadname/";
        $oldversiondir = "../$oldversionname/";
        $robotdir = "../$robotname/";
        $listdir = "../$listname/";
    }
    else
    {
        if( (index $filename, "$listname/") > -1)
        {
            # In the Mailing-list dir.
            $basedir = "../";
            $docsdir = "../$docsname/";
            $downloaddir = "../$downloadname/";
            $oldversiondir = "../$oldversionname/";
            $robotdir = "../$robotname/";
            $listdir = "";
        }
        else
        {
            # In the base dir.
            $basedir = "";
            $docsdir = "$docsname/";
            $downloaddir = "$downloadname/";
            $oldversiondir = "$oldversionname/";
            $robotdir = "$robotname/";
            $listdir = "$listname/";
        }
    }
    
    while (<TMP>) 
    {
        s/\#VERSION\#/$version/g;
        s/\#RELEASE\#/$release/g;
        s/\#BASEDIR\#/$basedir/g;
        s/\#DOCSDIR\#/$docsdir/g;
        s/\#DOWNLOADDIR\#/$downloaddir/g;
        s/\#OLDVERSIONDIR\#/$oldversiondir/g;
        s/\#LYSATORDOWNLOADGZ\#/$lysatordownload\RealTimeBattle-$version.tar.gz/g;
        s/\#LYSATORDOWNLOADBZ2\#/$lysatordownload\RealTimeBattle-$version.tar.bz2/g;
        s/\#SOURCEFORGEDOWNLOADGZ\#/$sourceforgedownload\RealTimeBattle-$version.tar.gz/g;
        s/\#SOURCEFORGEDOWNLOADBZ2\#/$sourceforgedownload\RealTimeBattle-$version.tar.bz2/g;
        s/\#LYSATOROLDVERSIONDIR\#/$lysatorhomepage$oldversiondir/g;
        s/\#SOURCEFORGEOLDVERSIONDIR\#/$sourceforgefilelist/g;
        s/\#ROBOTDIR\#/$robotdir/g;
        s/\#LISTDIR\#/$listdir/g;

        s/\#CGIBINDIR\#/$cgibindir/g;

        s/\#HTML\#/$htmlpostfix/g;
        s/\#COUNTER\#/$counter_this_page/g;

        s/\#HERESOURCEFORGE\#/(here)/g;
        s/\#HERELYSATOR\#//g;

        s/\#HOMEPAGE\#/$homepage/g;
        s/\#EMAILERIK\#/$emailerik/g;
        s/\#EMAILRAGNAR\#/$emailragnar/g;
        s/\#EMAILBOTH\#/$emailboth/g;
        s/\#TODAY\#/$today/g;
        
        s/\<A HREF=\"$filename\"\>([a-z A-Z\-]+)\<\/A\>/$1/g;
        print;
    }

    select STDOUT;
    print "done.\n";
}

