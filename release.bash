cp -p linux/generic/generic /p/devise/public/bin/devise
cp -p linux/multi/disp /p/devise/public/bin/multi

cp -p lib/[a-z]* /p/devise/public/lib
cp -p lib/[A-Z]* /p/devise/public/lib

cp run_public/devise /p/devise/run
cp run_public/devise.tasvir /p/devise/run
cp run_public/deviseweb /p/devise/run
cp run_public/dumpgif.script /p/devise/run
cp run_public/multi /p/devise/run
cp run_public/devise.etk /p/devise/run

tar cv --exclude=CVS public | tar xv --directory=/p/devise

cp run_public/devise $target/devisepub
