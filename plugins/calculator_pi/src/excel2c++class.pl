#!/usr/bin/perl -w

use strict;
use Spreadsheet::ParseExcel;

my @FunctionArray=();
#'m_ShortDesc','m_LongDesc','m_Category','m_Units','m_Source','m_Formula','m_Result_Unit','m_Input_parameter','m_Input_unit','m_Input_parameter1','m_Input_unit1','m_Input_parameter2','m_Input_unit2','m_Input_parameter3','m_Input_unit3','m_Input_parameter4','m_Input_unit4','m_Input_parameter5','m_Input_unit5','m_Input_parameter6','m_Input_unit6','m_Input_parameter7','m_Input_unit7','m_Input_parameter8','m_Input_unit8','m_Input_parameter9','m_Input_unit9'

my @headerc = "";
my @footerc = "";
my @headerh = "";
my @headerh1 = "";
my @footerh = "";
my @license ="";
my $filename="";
my $Class="";
my @MemberRow="";
my @TypeRow="";
my @DescriptonRow="";

my $parser = Spreadsheet::ParseExcel->new();
my $workbook = $parser->parse('functions.xls');

my $empty_line="yes";
my $FunctionSheet="0";
my $FunctionRow="0";
my $ProcessTab='false';

my ( $row_min, $row_max ) = (1,1);
my ( $col_min, $col_max ) = (1,1);


#open(my $fh, '>', "$filename");

if ( !defined $workbook ) {
	die $parser->error(), ".\n";
	}

#Check whether sheet should be processed
				my $count = 0;
				my $b="";
for my $worksheet ( $workbook->worksheets() ) 
	{
	#Reset variables
	$filename="";
	$Class="";

	#print "Worksheet: ",$worksheet,"\n";
        ( $row_min, $row_max ) = $worksheet->row_range();
        ( $col_min, $col_max ) = $worksheet->col_range();

	for my $row ( $row_min .. $row_max )
		{
		my $col = $col_min;
	        my $cell = $worksheet->get_cell( $row, $col );
	        next unless $cell;
		$a=$cell->unformatted();
		if ($a =~ /{Start --------------------------->}/)
			{
			print "Processing tab, starting on row: ",$row,"\n";
			$ProcessTab='true'
			}
		#Load filename
		if ($a =~ /{Cpp_file}/) 
			{
			$cell = $worksheet->get_cell( $row, $col+1 );
			$filename=$cell->unformatted();
			print "Filename: ",$filename,"\n";
			}
		#Load Classname
		if ($a =~ /{Class}/) 
			{
			$cell = $worksheet->get_cell( $row, $col+1 );
			$Class=$cell->unformatted();
			print "Class: ",$Class,"\n";
			}
		#Load License
		if ($a =~ /{License_Start}/) 
			{
			$count = 0;
			$b=" ";
			@license = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {
					$b=$cell->unformatted();
					last if ($b =~ /{License_End}/);  
					$b=$b."\n";
					push (@license, $b);}
				else {push (@license, "");}		
				}
			#print "License: ",@license,"\n";
			}
		#Load Header C
		if ($a =~ /{HEADER_C_START}/)  
			{
			$count = 0;
			$b=" ";
			@headerc = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {$b=$cell->unformatted();
				last if ($b =~ /{HEADER_C_END}/);  
				$b=$b."\n";
				push (@headerc, $b);}
				else {push (@headerc, "");}
				}
			#print "Header: ",@headerc,"\n";
			}

		#Load Footer C
		if ($a =~ /{FOOTER_C_START}/)  
			{
			$count = 0;
			$b=" ";
			@footerc = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {$b=$cell->unformatted();
				last if ($b =~ /{FOOTER_C_END}/);  
				$b=$b."\n";
				push (@footerc, $b);}
				else {push (@footerc, "");}
				}
			#print "Footer: ",@footerc,"\n";	
			}

		#Load Header H
		if ($a =~ /{HEADER_H_START}/)  
			{
			$count = 0;
			$b=" ";
			@headerh = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {$b=$cell->unformatted();
				last if ($b =~ /{HEADER_H_END}/);  
				$b=$b."\n";
				push (@headerh, $b);}
				else {push (@headerh, "");}
				}
			#print "Header: ",@headerh,"\n";
			}

		#Load Header H1
		if ($a =~ /{HEADER_H1_START}/)  
			{
			$count = 0;
			$b=" ";
			@headerh1 = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {$b=$cell->unformatted();
				last if ($b =~ /{HEADER_H1_END}/);  
				$b=$b."\n";
				push (@headerh1, $b);}
				else {push (@headerh1, "");}
				}
			#print "Header: ",@headerh1,"\n";
			}

		#Load Footer H
		if ($a =~ /{FOOTER_H_START}/)  
			{
			$count = 0;
			$b=" ";
			@footerh = '';
			while(1)
				{
				last if $count == 1000;  
				$count++;
				$cell = $worksheet->get_cell( $row + $count, $col );
				if ($cell) {$b=$cell->unformatted();
				last if ($b =~ /{FOOTER_H_END}/);  
				$b=$b."\n";
				push (@footerh, $b);}
				else {push (@footerh, "");}
				}
			#print "Footer: ",@footerh,"\n";	
			}


		#Member, type, Description
		if ($a =~ /{Member,_type,_Description_Start}/)  
			{
			$FunctionRow=$row+4;
			$count = 0;
			$b=" ";
			@MemberRow=();
			@TypeRow=();
			@DescriptonRow=();
			for my $coltemp ( $col_min .. $col_max )
				{
				$cell = $worksheet->get_cell( $row + 1 , $coltemp );
				if ($cell) {
					$b=$cell->unformatted();
					#$b=$b."\n";					
					push (@MemberRow, $b);}
				#else {push (@MemberRow, "");}
				$cell = $worksheet->get_cell( $row + 2 , $coltemp );
				if ($cell) {
					$b=$cell->unformatted();
					#$b=$b."\n";					
					push (@TypeRow, $b);}
				else {push (@TypeRow, "");}
				$cell = $worksheet->get_cell( $row + 3 , $coltemp );
				if ($cell) {
					$b=$cell->unformatted();
					#$b=$b."\n";					
					push (@DescriptonRow, $b);}
				else {push (@DescriptonRow, "");}
				}
			#print "MemberRow: ",@MemberRow,"\n";
			#print "TypeRow: ",@TypeRow,"\n";
			#print "DescriptonRow: ",@DescriptonRow,"\n";
			}

		} #END MYROW
		if ($ProcessTab =~ /true/) {
		#write CPP file
		#Open file
		open(my $fh, '>', "$filename.cpp");
		print $fh "@license\n";
		print $fh "#include \"$filename.h\"\n";
		print $fh "$Class"."::"."$Class(void)\n{\n";
		print $fh "@headerc\n";

		for my $row ( $FunctionRow .. $row_max )
			{
			for my $col ( $col_min .. $col_max )
				{
				my $cell = $worksheet->get_cell( $row, $col );
				#next unless $cell;
				if ($cell and $col==0) {$empty_line="no";} 
				if (!$cell and $col==0) {$empty_line="yes";print "Empty:"} 
				if ($cell)
					{
					#print "Row, Col    = ($row, $col)\n";
					#print "Value       = ", $cell->value(),       "\n";
					#print "Unformatted = ", $cell->unformatted(), "\n";
					$a=$cell->unformatted();
					$a=~ s/\R/\\n/g; #Re~ s/\R//g; #remove line feed, page feed, CR
					$a =~ s/[^\w @.:+-?=\*()\^\/\\]//g;
					$a=~ s/ {3,}/\\t/g; #replace double space with single space
					$a=~ s/\t* {2}/ /g; #replace double space with single space
					}
				else
					{
						$a="";
					}

				if ($empty_line =~ /no/ and $col<@MemberRow)
					{
					print $fh "this->$MemberRow[$col].Add(_(\"$a\"))\;\n"; 
					#print  "this->$MemberRow[$col].Add(_(\"$a\"))\;\n"; 
					}    
			    	}
				$empty_line="yes"; 
				print $fh "\n";
			}

		print $fh "@footerc\n";
		print $fh "}";
		#Close File
		close ($fh); 	

		#Write H file
		open($fh, '>', "$filename.h");
		print $fh "@license\n";
		print $fh "@headerh\n";
		print $fh "class $Class\n{\n\tpublic:\n";
		print $fh "\t$Class(void);\t\t\t\t//Class constructor\n";
		print $fh "@headerh1\n";



		for my $i (0 .. $#MemberRow) {
		    print $fh "\t\t$TypeRow[$i] $MemberRow[$i];\t\t\t\t\t//$DescriptonRow[$i]\n";
		}
		print $fh "};\n";
		print $fh "@footerh\n";


		#Close File
		close ($fh);
		}
	
	} #END WORKSHEET


