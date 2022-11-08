## DLogo Conversion
# Constants
ast_dir=$1
dlm_dir=$ast_dir/DuneLogoMaker

# Printout
echo "DLM @ $ast_dir"
echo "Converting DuneLogo.png to DuneLogo.dres"
echo "Do make run @ $dlm_dir"

# Run make run at jobserver 2
(cd $dlm_dir && gmake -j2 run)
