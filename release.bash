cp -p linux/generic/generic /p/devise/public/bin/devise
cp -p linux/generic/devise /p/devise/public/bin/devisec
cp -p linux/generic/deviseb /p/devise/public/bin/deviseb
cp -p linux/generic/devised /p/devise/public/bin/devised
cp -p linux/generic/collaborator /p/devise/public/bin/collaborator
#cp -p linux/multi/disp /p/devise/public/bin/multi

cp -p lib/[a-z]* /p/devise/public/lib
cp -p lib/[A-Z]* /p/devise/public/lib
mkdir /p/devise/public/lib/LM
cp -p lib/LM/*.tk /p/devise/public/lib/LM
chmod 644 /p/devise/public/lib/*.t*
chmod 644 /p/devise/public/lib/LM/*.t*

cp run_public/devise /p/devise/run
cp run_public/devise.tasvir /p/devise/run
cp run_public/deviseweb /p/devise/run
cp run_public/dumpgif.script /p/devise/run
cp run_public/multi /p/devise/run
cp run_public/devise.etk /p/devise/run
cp run_public/tk2ds.tcl /p/devise/run
cp run_public/kill_devised /p/devise/run
cp run_public/dev_collab /p/devise/run
cp run_public/collab_cleanup /p/devise/run

tar cv --exclude=CVS public | tar xv --directory=/p/devise

cp run_public/devise /p/devise/public/devisepub

cp public/transcat.tcl /p/devise/public
