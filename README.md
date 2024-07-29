# `taralli`: Next generation screen edge pointer wrapping for X

This tiny service is a screen edge pointer wrapper. This means if you get off
the screen e.g. on the right edge the mouse pointer will move in again from the
left side. This might be of particular interest if you have a setup with
multiple screens. This tool makes working far more convenient because you do
not have to travel large distances with the mouse if you wanna get from the
right-most to the left-most screen.

the codebase was originally written by Keegan McAllister, autotools added by
[[https://github.com/joshumax/taralli]](Josh Max). The latter is the base for
This tool but is enhnced to autodetect a multiple screen setup. So there's no
need to do some manual configuration.

## How to build and install

To build the tool you need to have the following dev libraries installed:

```shell
sudo apt install libx11-dev libxi-dev libxrandr-dev
```

Download the latest tarball from the
[https://github.com/rahra/taralli-ng/releases](releases page) and unpack it.
Then change into this folder and run the following commands.

```shell
./configure
make
sudo make install
```

To start it automatically after login, copy the file
`systemd.user/taralli.service` to the folder `/usr/lib/systemd/user` and reload
the systemd. Finally either reboot or start it with `systemctl --user`.

```shell
sudo cp systemd.user/taralli.service /usr/lib/systemd/user
sudo systemctl --daemon-reload
systemctl --user start taralli
```

## License

`taralli-ng` is available under a BSD license.  See the `LICENSE` file.
