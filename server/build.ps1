$env:RUSTFLAGS = '-L /usr/aarch64-linux-gnu/lib/ -L /usr/lib/aarch64-linux-gnu/'

cross build

#scp .\target\aarch64-unknown-linux-gnu\debug\astro_server astro@192.168.1.88:/home/astro
