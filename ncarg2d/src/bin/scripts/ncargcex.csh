#!/bin/csh -f
#
#	$Id: ncargcex.csh,v 1.14 1994-08-11 19:15:43 haley Exp $
#

#********************#
#                    #
#   NCARGCEX USAGE   #
#                    #
#********************#
if ($#argv < 1) then
  echo "usage: ncargcex [-all,-A] [-autograph] [-bivar] [-conpack] [-ezmap]"
  echo "                [-gks] [-labelbar] [-plotchar] [-scrolled_title]   "
  echo "                [-softfill] [-inter] [-clean] [-n] [-onebyone]     "
  echo "                [-W n] names                                       "
  echo ""
  echo "See <man ncargcex>                                                  "
  exit
endif

#*********************************************#
#                                             #
# Make sure NCARG_ROOT is set for this script #
#                                             #
#*********************************************#
setenv NCARG_ROOT  `ncargpath root`

if ($status != 0) then
    exit 1
endif

#*********************************#
#                                 #
# Check for existing directories  #
#                                 #
#*********************************#
set example_dir = `ncargpath SED_EXAMPLESDIR`

if ($status != 0) then
        exit 1
endif

if (! -d "$example_dir") then
  echo "Example directory <$example_dir> does not exist."
  exit 1
endif

set tutorial_dir = `ncargpath SED_TUTORIALDIR`

if ($status != 0) then
        exit 1
endif

if (! -d "$tutorial_dir") then
  echo "Tutorial directory <$tutorial_dir> does not exist."
  exit 1
endif


#**************************#
#                          #
#  Set autograph examples  #
#                          #
#**************************#
set autograph_list = (c_agex07)
set ex_list = ($autograph_list)

#******************************#
#                              #
#  Set bivar/conpack examples  #
#                              #
#******************************#
set cbivar_list = (c_cbex01)
set ex_list = ($ex_list $cbivar_list)

#************************#
#                        #
#  Set conpack examples  #
#                        #
#************************#
set conpack_list = (c_colcon)
set ex_list = ($ex_list $conpack_list)

#**********************#
#                      #
#  Set ezmap examples  #
#                      #
#**********************#
set ezmap_list = (c_eezmpa c_mpex05)
set ex_list = ($ex_list $ezmap_list)

#******************#
#                  #
# set gks examples #
#                  #
#******************#

set gks_list = (c_gtxpac)
set ex_list = ($ex_list $gks_list)

#***********************#
#                       #
# set labelbar examples #
#                       #
#***********************#
set labelbar_list   = (c_elblba)
set ex_list = ($ex_list $labelbar_list)
#***********************#
#                       #
# set plotchar examples #
#                       #
#***********************#
set plotchar_list   = (c_epltch)
set ex_list = ($ex_list $plotchar_list)

#*****************************#
#                             #
# set scrolled title examples #
#                             #
#*****************************#
set scrlld_title_list   = (c_slex01)
set ex_list = ($ex_list $scrlld_title_list)

#***********************#
#                       #
# set softfill examples #
#                       #
#***********************#
set softfill_list   = (c_sfex02)
set ex_list = ($ex_list $softfill_list)

#**************************#
#                          #
# set interactive examples #
#                          #
#**************************#
set interactive_list = (c_xwndws)
#*********************************#
#                                 #
# Default is to load in X library #
#                                 #
#*********************************#
set X11_option

#***************#
#               #
# Parse options #
#               #
#***************#
set names

#*********************************#
#                                 #
# Default workstation type is "1" #
# Default graphic is NCGM         #
#                                 #
#*********************************#
set ncgmfile
set ws_type = "1"

while ($#argv > 0)
    
    switch ($1)

        case "-all":
        case "-A":
            shift
            set names=($ex_list)
            breaksw
        
        case "-autograph":
            shift
            set names=($names $autograph_list)
            breaksw

        case "-bivar":
            shift
            set names=($names $cbivar_list)
            breaksw

        case "-conpack":
            shift
            set names=($names $conpack_list)
            breaksw

        case "-ezmap":
            shift
            set names=($names $ezmap_list)
            breaksw

        case "-gks":
            shift
            set names=($names $gks_list)
            breaksw

        case "-labelbar":
            shift
            set names=($names $labelbar_list)
            breaksw

        case "-plotchar":
            shift
            set names=($names $plotchar_list)
            breaksw

        case "-scrolled_title":
            shift
            set names=($names ${scrlld_title_list})
            breaksw

        case "-softfill":
            shift
            set names=($names $softfill_list)
            breaksw

        case "-inter":
            shift
            set interfile
            set ws_type = "8"
            set names=($names $interactive_list)
            breaksw

        case "-clean":
            shift
            set CleanOption
            breaksw

        case "-n":
            shift
            set NoRunOption
            breaksw
        
        case "-onebyone":
            shift
            set OneByOneOption
            breaksw

        case "-unique"
            shift
            set Unique
            breaksw

        case "-list"
            shift
            set List
            breaksw

        case "-W":
            shift
            set ws_type = "$1"

            switch ($ws_type)
            
            case  "1":
            case "10":
                set ncgmfile
            breaksw

            case "8":
                set interfile
            breaksw


            case "20":
            case "23":
            case "26":
            case "29":
                set psfile
            breaksw

            case "21":
            case "24":
            case "27":
            case "30":
                set epsfile
            breaksw

            case "22":
            case "25":
            case "28":
            case "31":
                set epsifile
            breaksw

            default:
                echo ""
                echo "    ncargcex:  $ws_type is an invalid workstation type."
                echo ""
                exit 1
            endsw
            shift
            breaksw

        case "-noX11":
            shift
            set X11_option = "-noX11"
            breaksw

        case "-*":
            echo "$0 : Unknown option <$1>"
            exit 1
            breaksw

        default:
            set names=($names $1)
            shift
            breaksw
    endsw
end

#********************************************#
#                                            #
# Cannot have both interactive and noX11 set #
#                                            #
#********************************************#

if ($X11_option == "-noX11" && $?interfile) then
    echo ""
    echo "Warning:  You cannot use the '-noX11' option if you are"
    echo "          running an interactive example.  I will turn"
    echo "          the '-noX11' option off."
    echo ""
    set X11_option
endif

#***********************************************#
#                                               #
# Cannot have both interactive and ws_type != 8 #
#                                               #
#***********************************************#
if ($?interfile && $ws_type != "8") then
    echo ""
    echo "Warning:  You must have a workstation type of '8' if you"
    echo "          are running an interactive example.  I will force"
    echo "          ws_type to be '8'.  If you specify the '-inter'"
    echo "          option, ws_type will be set to 8 automatically."
    echo ""
    set ws_type = "8"
endif

foreach name ($names)

switch($name)
    case c_pgkex19:
    case c_pgkex20:
    case c_pgkex21:
        set graphic_type = "ps"
        set default_file = "gmeta1.ps"
        set message = "PostScript file is named"
    breaksw

    default:
        if ($?psfile) then
            set default_file = "gmeta1.ps"
            set graphic_type = "ps"
            set message = "PostScript file is named"
        else if ($?epsfile) then
            set default_file = "gmeta1.eps"
            set graphic_type = "eps"
            set message = "Encapsulated PostScript file is named"
        else if ($?epsifile) then
            set default_file = "gmeta1.epsi"
            set graphic_type = "epsi"
            set message = "Interchange Encapsulated PostScript file is named"
        else 
            set default_file = "gmeta"
            set graphic_type = "ncgm"
            set message = "Metafile file is named"
        endif
    breaksw
endsw

set graphic_file = $name.$graphic_type

#*************************************#
#                                     #
# Find out what type of example it is #
#                                     #
#*************************************#
set type="Unknown"

foreach known ($ex_list)
    if ("$name" == "$known") then
        set type="Example"
    endif
end

foreach known ($interactive_list)
    if ("$name" == "$known") then
        set type="Interactive_Example"
    endif
end

if ($?List) then
   echo $names
   exit
endif

#***********************************************#
#                                               #
# If you just want to see what list of examples #
# you have asked for, list them and exit        #
#                                               #
#***********************************************#
if ($?List) then
   echo $name
   goto theend
endif

#**************************#
#                          #
# Find out what type it is #
#                          #
#**************************#
switch ($type)
    case Example:
        echo "NCAR Graphics C Example <$name>"
    breaksw

    case Interactive_Example:
        echo "NCAR Graphics Interactive C Example <$name>"
    breaksw

    case Unknown:
        echo "ncargcex: <$name> is not a known example"
        goto theend
    breaksw
endsw
echo ""

#***********************************************#
#                                               #
# If the "-unique" option was selected and the  #
# NCGM already exists, don't generate it again. #
#                                               #
#***********************************************#

if ($?Unique && -f $graphic_file) goto theend

set c_files
set ncargf77flags
set rmfiles = "$name.c"
set copy_files

#***********************#
#                       #
# Copy the needed files #
#                       #
#***********************#
   
echo "  Copying $name.c"
ed << EOF - $example_dir/$name.c >& /dev/null
g/SED_WSTYPE/s//$ws_type/g
w ./$name.c
q
EOF

set c_files = ($c_files $name.c)

foreach file($copy_files)
    echo "  Copying $file:t"
    cp $file $file:t
end


#******************************#
#                              #
# Compile and link the example #
#                              #
#******************************#

unset not_valid_metafile
unset no_file

if (! $?NoRunOption) then
    if ($type == "Interactive_Example" && $?interfile) then
        echo ""
        echo "    This example is interactive and can only be executed if"
        echo "    you have X running and have your DISPLAY environment"
        echo "    variable set properly.  It will create an X11 window"
        echo "    that you must click on with your mouse to advance the"
        echo "    frame(s)."
        echo ""
        echo ""
        echo "Compiling and Linking..."
        ncargcc $ncargf77flags -o $name $c_files
    else
        echo ""
        echo "Compiling and Linking..."
        ncargcc $ncargf77flags $X11_option -o $name $c_files
    endif
    if ($status != 0) then
            echo ""
            echo "The compile and link failed"
            exit -1
    endif
    echo ""
    echo "Executing <$name>..."
#*****************#
#                 #
# Run the example #
#                 #
#*****************#
   
	if ($?ncgmfile) setenv NCARG_GKS_OUTPUT $name.ncgm
    ./$name
    if ( ! $?not_valid_metafile && ! $?no_file ) then
        if (! $?ncgmfile ) then
            mv ./$default_file $graphic_file
        endif
        echo ""
        echo "$message $graphic_file"
        echo ""
    endif

    set rmfiles = ($rmfiles $name.o $name)
endif

#******************************#
#                              #
# Keep track of unwanted files #
#                              #
#******************************#
switch ($name)
    case c_slex01:
        set rmfiles = ($rmfiles GNFB09)
    breaksw
endsw


#************************#
#                        #
# Remove unwanted files. #
#                        #
#************************#

if ($?CleanOption) then
    rm -f $rmfiles
endif

#************************#
#                        #
# Display NCGM on screen #
#                        #
#************************#
if ($?OneByOneOption) then
    ctrans -d X11 -geometry 1142x865+10+0 $name.ncgm
    rm -f $name.ncgm $rmfiles
endif

theend:

end

