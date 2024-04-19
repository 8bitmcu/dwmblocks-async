# dwmblocks-async - dwm status bar (fork)

dwmblocks-async [(original)](https://github.com/UtkarshVerma/dwmblocks-async) is a dwm status bar that has a modular, async design, so it is always responsive. Imagine i3blocks, but for dwm. 

This fork adds a configuration file for the blocks. Check out [blocks.toml](blocks.toml)!


## Requirements

- dwm patched with [statuscmd](https://dwm.suckless.org/patches/statuscmd/).

## Building and installing

1. clone this repository locally on your machine
2. run `make clean && sudo make install` from within the repository folder
3. copy and edit the config file: `cp /etc/dwm/blocks.toml $XDG_CONFIG_HOME/dwm/blocks.toml`


## Usage

To set dwmblocks-async as your status bar, you need to run it as a background
process on startup. One way is to add the following to `~/.xinitrc`:

```sh
# The binary of `dwmblocks-async` is named `dwmblocks`
dwmblocks &
```

### Signaling changes

Most status bars constantly rerun all scripts every few seconds. This is an
option here, but a superior choice is to give your block a signal through which
you can indicate it to update on relevant event, rather than have it rerun
idly.

For example, the volume block has the update signal `5` by default. I run
`kill -39 $(pidof dwmblocks)` alongside my volume shortcuts in dwm to only
update it when relevant. Just add `34` to your signal number! You could also
run `pkill -RTMIN+5 dwmblocks`, but it's slower.

To refresh all the blocks, run `kill -10 $(pidof dwmblocks)` or
`pkill -SIGUSR1 dwmblocks`.

> All blocks must have different signal numbers!


### Clickable blocks

Like `i3blocks`, this build allows you to build in additional actions into your
scripts in response to click events. You can check out
[my status bar scripts](https://github.com/UtkarshVerma/dotfiles/tree/main/.local/bin/statusbar)
as references for using the `$BLOCK_BUTTON` variable.

To use this feature, set `clickable_blocks` to 1 in your config file
